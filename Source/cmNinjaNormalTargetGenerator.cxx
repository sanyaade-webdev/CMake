/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmNinjaNormalTargetGenerator.h"
#include "cmLocalNinjaGenerator.h"
#include "cmGlobalNinjaGenerator.h"
#include "cmSourceFile.h"
#include "cmGeneratedFileStream.h"
#include "cmMakefile.h"

cmNinjaNormalTargetGenerator::
cmNinjaNormalTargetGenerator(cmTarget* target)
  : cmNinjaTargetGenerator(target)
  , TargetNameOut()
  , TargetNameSO()
  , TargetNameReal()
  , TargetNameImport()
  , TargetNamePDB()
  , TargetLinkLanguage(target->GetLinkerLanguage(this->GetConfigName()))
{
  if (target->GetType() == cmTarget::EXECUTABLE)
    target->GetExecutableNames(this->TargetNameOut,
                               this->TargetNameReal,
                               this->TargetNameImport,
                               this->TargetNamePDB,
                               GetLocalGenerator()->GetConfigName());
  else
    target->GetLibraryNames(this->TargetNameOut,
                            this->TargetNameSO,
                            this->TargetNameReal,
                            this->TargetNameImport,
                            this->TargetNamePDB,
                            GetLocalGenerator()->GetConfigName());
}

cmNinjaNormalTargetGenerator::~cmNinjaNormalTargetGenerator()
{
}

void cmNinjaNormalTargetGenerator::Generate()
{
  // Write the rules for each language.
  this->WriteLanguagesRules();

  // Write the build statements
  this->WriteObjectBuildStatements();

  // Write the link statement.
  this->WriteLinkStatement();

  this->GetBuildFileStream() << "\n";
  this->GetRulesFileStream() << "\n";
}

void cmNinjaNormalTargetGenerator::WriteLanguagesRules()
{
  cmGlobalNinjaGenerator::WriteDivider(this->GetRulesFileStream());
  this->GetRulesFileStream()
    << "# Rules for each languages for "
    << cmTarget::TargetTypeNames(this->GetTarget()->GetType())
    << " target "
    << this->GetTargetName()
    << "\n\n";

  std::set<cmStdString> languages;
  this->GetTarget()->GetLanguages(languages);
  for(std::set<cmStdString>::const_iterator l = languages.begin();
      l != languages.end();
      ++l)
    this->WriteLanguageRules(*l);
  this->WriteLinkRule();
}

const char *cmNinjaNormalTargetGenerator::GetVisibleTypeName() const {
  switch (this->GetTarget()->GetType()) {
    case cmTarget::STATIC_LIBRARY:
      return "static library";
    case cmTarget::SHARED_LIBRARY:
      return "shared library";
    case cmTarget::EXECUTABLE:
      return "executable";
    default:
      return 0;
  }
}

std::string
cmNinjaNormalTargetGenerator
::LanguageLinkerRule() const
{
  return std::string(this->TargetLinkLanguage)
    + "_"
    + cmTarget::TargetTypeNames(this->GetTarget()->GetType())
    + "_LINKER";
}

void
cmNinjaNormalTargetGenerator
::WriteLinkRule()
{
  std::string ruleName = this->LanguageLinkerRule();

  if (!this->GetGlobalGenerator()->HasRule(ruleName)) {
    cmLocalGenerator::RuleVariables vars;
    vars.RuleLauncher = "RULE_LAUNCH_LINK";
    vars.CMTarget = this->GetTarget();
    vars.Language = this->TargetLinkLanguage;
    vars.Objects = "$in";
    std::string objdir = cmake::GetCMakeFilesDirectoryPostSlash();
    objdir += this->GetTargetName();
    objdir += ".dir";
    objdir = this->GetLocalGenerator()->Convert(objdir.c_str(),
                                                cmLocalGenerator::START_OUTPUT,
                                                cmLocalGenerator::SHELL);
    vars.ObjectDir = objdir.c_str();
    vars.Target = "$out";
    vars.TargetSOName = "$SONAME";

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
    this->GetLocalGenerator()->AddLanguageFlags(langFlags,
                                                this->TargetLinkLanguage,
                                                this->GetConfigName());
    langFlags += "$ARCHITECTURE_FLAGS";
    vars.LanguageCompileFlags = langFlags.c_str();

    // Rule for linking library.
    std::vector<std::string> linkCmds = this->ComputeLinkCmd();
    for(std::vector<std::string>::iterator i = linkCmds.begin();
        i != linkCmds.end();
        ++i)
      {
      this->GetLocalGenerator()->ExpandRuleVariables(*i, vars);
      }
    std::string linkCmd = BuildCommandLine(linkCmds);

    // Write the linker rule.
    std::ostringstream comment;
    comment << "Rule for linking " << this->TargetLinkLanguage << " "
            << this->GetVisibleTypeName() << ".";
    std::ostringstream description;
    description << "Linking " << this->TargetLinkLanguage << " "
                << this->GetVisibleTypeName() << " $out";
    this->GetGlobalGenerator()->AddRule(ruleName,
                                        linkCmd,
                                        comment.str(),
                                        description.str(),
                                        /*depfile*/ "",
                                        cmNinjaVars());
  }

  if (this->TargetNameOut != this->TargetNameReal) {
    std::string cmakeCommand =
      this->GetMakefile()->GetRequiredDefinition("CMAKE_COMMAND");
    if (this->GetTarget()->GetType() == cmTarget::EXECUTABLE)
      this->GetGlobalGenerator()->AddRule("CMAKE_SYMLINK_EXECUTABLE",
                                          cmakeCommand +
                                          " -E cmake_symlink_executable"
                                          " $in $out",
                                          "Rule for creating executable symlink.",
                                          "Creating executable symlink $out",
                                          /*depfile*/ "",
                                          cmNinjaVars());
    else
      this->GetGlobalGenerator()->AddRule("CMAKE_SYMLINK_LIBRARY",
                                          cmakeCommand +
                                          " -E cmake_symlink_library"
                                          " $in $SONAME $out",
                                          "Rule for creating library symlink.",
                                          "Creating library symlink $out",
                                          /*depfile*/ "",
                                          cmNinjaVars());
  }
}

std::vector<std::string>
cmNinjaNormalTargetGenerator
::ComputeLinkCmd()
{
  switch (this->GetTarget()->GetType()) {
    case cmTarget::STATIC_LIBRARY: {
      // Check if you have a non archive way to create the static library.
      {
      std::string linkCmdVar = "CMAKE_";
      linkCmdVar += this->TargetLinkLanguage;
      linkCmdVar += "_CREATE_STATIC_LIBRARY";
      if (const char *linkCmd =
            this->GetMakefile()->GetDefinition(linkCmdVar.c_str()))
        {
        return std::vector<std::string>(1, linkCmd);
        }
      }

      // We have archive link commands set.
      std::vector<std::string> linkCmds;
      {
      std::string linkCmdVar = "CMAKE_";
      linkCmdVar += this->TargetLinkLanguage;
      linkCmdVar += "_ARCHIVE_CREATE";
      const char *linkCmd =
        this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());
      linkCmds.push_back(linkCmd);
      }
      // TODO(Nicolas Despres): I'll see later how to deals with that.
      // {
      // std::string linkCmdVar = "CMAKE_";
      // linkCmdVar += this->TargetLinkLanguage;
      // linkCmdVar += "_ARCHIVE_APPEND";
      // const char *linkCmd =
      //   this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());
      // linkCmds.push_back(linkCmd);
      // }
      {
      std::string linkCmdVar = "CMAKE_";
      linkCmdVar += this->TargetLinkLanguage;
      linkCmdVar += "_ARCHIVE_FINISH";
      const char *linkCmd =
        this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());
      linkCmds.push_back(linkCmd);
      }
      return linkCmds;
    }
    case cmTarget::SHARED_LIBRARY: {
      std::string linkCmdVar = "CMAKE_";
      linkCmdVar += this->TargetLinkLanguage;
      linkCmdVar += "_CREATE_SHARED_LIBRARY";
      const char *linkCmd =
        this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());
      return std::vector<std::string>(1, linkCmd);
    }
    case cmTarget::EXECUTABLE: {
      std::string linkCmdVar = "CMAKE_";
      linkCmdVar += this->TargetLinkLanguage;
      linkCmdVar += "_LINK_EXECUTABLE";
      const char *linkCmd =
        this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());
      return std::vector<std::string>(1, linkCmd);
    }
  }
}

void cmNinjaNormalTargetGenerator::WriteLinkStatement()
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
  comment << "Link the " << this->GetVisibleTypeName() << " " << targetOutputReal;

  // Compute outputs.
  cmNinjaDeps outputs;
  outputs.push_back(targetOutputReal);
  // Add this executable to the all target.
  this->GetLocalGenerator()->AddDependencyToAll(this->GetTargetName());

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
                                                  this->TargetLinkLanguage,
                                                  this->GetConfigName());
  vars["SONAME"] = this->TargetNameSO;

  // Write the build statement for this target.
  cmGlobalNinjaGenerator::WriteBuild(this->GetBuildFileStream(),
                                     comment.str(),
                                     this->LanguageLinkerRule(),
                                     outputs,
                                     explicitDeps,
                                     implicitDeps,
                                     emptyDeps,
                                     vars);

  if (targetOutput != targetOutputReal) {
    if (this->GetTarget()->GetType() == cmTarget::EXECUTABLE) {
      cmGlobalNinjaGenerator::WriteBuild(this->GetBuildFileStream(),
                                         "Create executable symlink " + targetOutput,
                                         "CMAKE_SYMLINK_EXECUTABLE",
                                         cmNinjaDeps(1, targetOutput),
                                         cmNinjaDeps(1, targetOutputReal),
                                         emptyDeps,
                                         emptyDeps,
                                         cmNinjaVars());
    } else {
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
  }

  // Write a shortcut rule with the target name.
  this->WriteTargetBuild(this->TargetNameOut, targetOutput);
}
