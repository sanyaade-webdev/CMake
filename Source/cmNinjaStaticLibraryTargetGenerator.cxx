/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmNinjaStaticLibraryTargetGenerator.h"
#include "cmLocalNinjaGenerator.h"
#include "cmGlobalNinjaGenerator.h"
#include "cmSourceFile.h"
#include "cmGeneratedFileStream.h"
#include "cmMakefile.h"

cmNinjaStaticLibraryTargetGenerator::
cmNinjaStaticLibraryTargetGenerator(cmTarget* target)
  : cmNinjaTargetGenerator(target)
  , TargetNameOut()
  , TargetNameSO()
  , TargetNameReal()
  , TargetNameImport()
  , TargetNamePDB()
{
  this->GetTarget()->GetLibraryNames(this->TargetNameOut,
                                     this->TargetNameSO,
                                     this->TargetNameReal,
                                     this->TargetNameImport,
                                     this->TargetNamePDB,
                                     GetLocalGenerator()->GetConfigName());
}

cmNinjaStaticLibraryTargetGenerator::~cmNinjaStaticLibraryTargetGenerator()
{
}

void
cmNinjaStaticLibraryTargetGenerator
::WriteLinkRule(const std::string& language)
{
  cmLocalGenerator::RuleVariables vars;
  vars.RuleLauncher = "RULE_LAUNCH_LINK";
  vars.CMTarget = this->GetTarget();
  std::string lang = language;
  vars.Language = lang.c_str();
  vars.Objects = "$in";
  std::string objdir = cmake::GetCMakeFilesDirectoryPostSlash();
  objdir += this->GetTargetName();
  objdir += ".dir";
  objdir = this->GetLocalGenerator()->Convert(objdir.c_str(),
                                              cmLocalGenerator::START_OUTPUT,
                                              cmLocalGenerator::SHELL);
  vars.ObjectDir = objdir.c_str();
  vars.Target = "$out";

  // Setup the target version.
  std::string targetVersionMajor;
  std::string targetVersionMinor;
  {
  cmOStringStream majorStream;
  cmOStringStream minorStream;
  int major;
  int minor;
  this->GetTarget()->GetTargetVersion(major, minor);
  majorStream << major;
  minorStream << minor;
  targetVersionMajor = majorStream.str();
  targetVersionMinor = minorStream.str();
  }
  vars.TargetVersionMajor = targetVersionMajor.c_str();
  vars.TargetVersionMinor = targetVersionMinor.c_str();

  vars.LinkLibraries = "$LINK_LIBRARIES";
  vars.Flags = "$FLAGS";
  vars.LinkFlags = "$LINK_FLAGS";

  std::string langFlags;
  this->GetLocalGenerator()->AddLanguageFlags(langFlags, language.c_str(),
                                              this->GetConfigName());
  langFlags += "$ARCHITECTURE_FLAGS";
  vars.LanguageCompileFlags = langFlags.c_str();

  // Rule for linking library.
  std::vector<std::string> linkCmds = this->ComputeLinkCmd(language);
  std::string linkCmd;
  bool isLast = false;
  for(std::vector<std::string>::const_iterator i = linkCmds.begin();
      i != linkCmds.end();
      ++i)
    {
    if(!isLast && i + 1 == linkCmds.end())
      isLast = true;
    std::string cmd = *i;
    this->GetLocalGenerator()->ExpandRuleVariables(cmd, vars);
    linkCmd += cmd;
    // TODO(Nicolas Despres): This will work only on Unix platforms. I don't
    // want to use a link.txt file because I will loose the benefit of the
    // $in variables. A discussion about dealing with multiple commands in
    // a rule is started here:
    // http://groups.google.com/group/ninja-build/browse_thread/thread/d515f23a78986008
    if(!isLast)
      linkCmd += " && ";
    }

  // Write the rule for linking an library.
  std::ostringstream comment;
  comment << "Rule for linking " << language << " static library.";
  std::ostringstream description;
  description << "Linking " << language << " static library $out";
  std::string depfile = "";
  cmNinjaVars emptyVars;
  this->GetGlobalGenerator()->AddRule(this->LanguageLinkerRule(language),
                                      linkCmd,
                                      comment.str(),
                                      description.str(),
                                      depfile,
                                      emptyVars);

  std::string cmakeCommand =
    this->GetMakefile()->GetRequiredDefinition("CMAKE_COMMAND");
  this->GetGlobalGenerator()->AddRule("CMAKE_SYMLINK_LIBRARY",
                                      cmakeCommand +
                                      " -E cmake_symlink_library"
                                      " $in $SONAME $out",
                                      "Rule for creating library symlink.",
                                      "Creating library symlink $out",
                                      depfile,
                                      emptyVars);
}

std::vector<std::string>
cmNinjaStaticLibraryTargetGenerator
::ComputeLinkCmd(const std::string& language)
{
  std::vector<std::string> linkCmds;
  // Check if you have a non archive way to create the static library.
  {
  std::string linkCmdVar = "CMAKE_";
  linkCmdVar += language;
  linkCmdVar += "_CREATE_STATIC_LIBRARY";
  std::string linkCmd =
    this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());
  if(!linkCmd.empty())
    {
    linkCmds.push_back(linkCmd);
    return linkCmds;
    }
  }

  // We have archive link commands set.
  {
  std::string linkCmdVar = "CMAKE_";
  linkCmdVar += language;
  linkCmdVar += "_ARCHIVE_CREATE";
  std::string linkCmd =
    this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());
  std::cerr << "DEBUG NINJA: " << linkCmdVar << " = " << linkCmd << std::endl;
  linkCmds.push_back(linkCmd);
  }
  // TODO(Nicolas Despres): I'll see later how to deals with that.
  // {
  // std::string linkCmdVar = "CMAKE_";
  // linkCmdVar += language;
  // linkCmdVar += "_ARCHIVE_APPEND";
  // std::string linkCmd =
  //   this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());
  // std::cerr << "DEBUG NINJA: " << linkCmdVar << " = " << linkCmd << std::endl;
  // linkCmds.push_back(linkCmd);
  // }
  {
  std::string linkCmdVar = "CMAKE_";
  linkCmdVar += language;
  linkCmdVar += "_ARCHIVE_FINISH";
  std::string linkCmd =
    this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());
  std::cerr << "DEBUG NINJA: " << linkCmdVar << " = " << linkCmd << std::endl;
  linkCmds.push_back(linkCmd);
  }
  return linkCmds;
}

void cmNinjaStaticLibraryTargetGenerator::WriteLinkStatement()
{
  // Write comments.
  cmGlobalNinjaGenerator::WriteDivider(this->GetBuildFileStream());
  this->GetBuildFileStream()
    << "# Link build statements for "
    << cmTarget::TargetTypeNames(this->GetTarget()->GetType())
    << " target "
    << this->GetTargetName()
    << "\n\n";

  cmNinjaDeps emptyDeps;
  cmNinjaVars vars;

  std::string targetOutput = this->GetTargetFilePath(this->TargetNameOut);
  std::string targetOutputReal = this->GetTargetFilePath(this->TargetNameReal);

  // Compute the comment.
  std::ostringstream comment;
  comment << "Link the static library " << targetOutputReal;

  // Compute outputs.
  cmNinjaDeps outputs;
  outputs.push_back(targetOutputReal);
  // Add this executable to the all target.
  this->GetLocalGenerator()->AddDependencyToAll(this->GetTargetName());

  const char* linkLanguage =
    this->GetTarget()->GetLinkerLanguage(this->GetConfigName());

  // Compute specific libraries to link with.
  cmNinjaDeps explicitDeps = this->GetObjects(),
              implicitDeps = this->ComputeLinkDeps();

  this->GetLocalGenerator()->GetTargetFlags(vars["LINK_LIBRARIES"],
                                            vars["FLAGS"],
                                            vars["LINK_FLAGS"],
                                            *this->GetTarget());

  // Compute specific link flags.
  this->GetLocalGenerator()->AddArchitectureFlags(vars["ARCHITECTURE_FLAGS"],
                                                  this->GetTarget(),
                                                  linkLanguage,
                                                  this->GetConfigName());
  vars["SONAME"] = this->TargetNameSO;

  // Write the build statement for this target.
  cmGlobalNinjaGenerator::WriteBuild(this->GetBuildFileStream(),
                                     comment.str(),
                                     this->LanguageLinkerRule(linkLanguage),
                                     outputs,
                                     explicitDeps,
                                     implicitDeps,
                                     emptyDeps,
                                     vars);

  if (targetOutput != targetOutputReal) {
    cmNinjaVars symlinkVars;
    symlinkVars["SONAME"] = this->TargetNameSO;
    cmGlobalNinjaGenerator::WriteBuild(this->GetBuildFileStream(),
                                       "Create library symlink " + targetOutput,
                                       "CMAKE_SYMLINK_LIBRARY",
                                       cmNinjaDeps(1, targetOutput),
                                       cmNinjaDeps(1, targetOutputReal),
                                       emptyDeps,
                                       emptyDeps,
                                       symlinkVars);
  }

  // Write a shortcut rule with the target name.
  this->WriteTargetBuild(this->TargetNameOut, targetOutput);
}
