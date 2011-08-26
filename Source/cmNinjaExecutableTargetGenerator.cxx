/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmNinjaExecutableTargetGenerator.h"
#include "cmLocalNinjaGenerator.h"
#include "cmGlobalNinjaGenerator.h"
#include "cmSourceFile.h"
#include "cmGeneratedFileStream.h"
#include "cmMakefile.h"

cmNinjaExecutableTargetGenerator::
cmNinjaExecutableTargetGenerator(cmTarget* target)
  : cmNinjaTargetGenerator(target)
  , TargetNameOut()
  , TargetNameReal()
  , TargetNameImport()
  , TargetNamePDB()
{
  this->GetTarget()->GetExecutableNames(this->TargetNameOut,
                                        this->TargetNameReal,
                                        this->TargetNameImport,
                                        this->TargetNamePDB,
                                        GetLocalGenerator()->GetConfigName());
}

cmNinjaExecutableTargetGenerator::~cmNinjaExecutableTargetGenerator()
{
}

void
cmNinjaExecutableTargetGenerator
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

  // Rule for linking executable.
  std::string linkCmdVar = "CMAKE_";
  linkCmdVar += language;
  linkCmdVar += "_LINK_EXECUTABLE";
  std::string linkCmd =
    this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());

  this->GetLocalGenerator()->ExpandRuleVariables(linkCmd, vars);

  // Write the rule for linking an executable.
  std::ostringstream comment;
  comment << "Rule for linking " << language << " executable.";
  std::ostringstream description;
  description << "Linking " << language << " executable $out";
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
  this->GetGlobalGenerator()->AddRule("CMAKE_SYMLINK_EXECUTABLE",
                                      cmakeCommand +
                                      " -E cmake_symlink_executable"
                                      " $in $out",
                                      "Rule for creating executable symlink.",
                                      "Creating executable symlink $out",
                                      depfile,
                                      emptyVars);
}

void cmNinjaExecutableTargetGenerator::WriteLinkStatement()
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
  comment << "Link the executable " << targetOutputReal;

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
    cmGlobalNinjaGenerator::WriteBuild(this->GetBuildFileStream(),
                                       "Create executable symlink " + targetOutput,
                                       "CMAKE_SYMLINK_EXECUTABLE",
                                       cmNinjaDeps(1, targetOutput),
                                       cmNinjaDeps(1, targetOutputReal),
                                       emptyDeps,
                                       emptyDeps,
                                       cmNinjaVars());
  }

  // Write a shortcut rule with the target name.
  this->WriteTargetBuild(this->TargetNameOut, targetOutput);
}
