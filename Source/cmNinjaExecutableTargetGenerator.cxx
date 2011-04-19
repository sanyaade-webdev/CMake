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

void cmNinjaExecutableTargetGenerator::Generate()
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

  vars.LinkLibraries = "$LDLIBS";
  // This is mandatory to use a local variables because the string must be
  // in memory until ExpandRuleVariables() is called.
  std::string flags = this->LanguageFlagsVarName(language, true);
  vars.Flags = flags.c_str();
  vars.LinkFlags = "$LDFLAGS";

  // Rule for linking executable.
  std::string linkCmdVar = "CMAKE_";
  linkCmdVar += language;
  linkCmdVar += "_LINK_EXECUTABLE";
  std::string linkCmd =
    this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());
  std::cout << "DEBUG NINJA: " << linkCmdVar << " = " << linkCmd << std::endl;

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

  // Compute the comment.
  std::ostringstream comment;
  comment << "Link the executable " << targetOutput;

  // Compute outputs.
  cmNinjaDeps outputs;
  outputs.push_back(targetOutput);
  // Add this executable to the all target.
  this->GetLocalGenerator()->AddDependencyToAll(this->GetTargetName());

  const char* linkLanguage =
    this->GetTarget()->GetLinkerLanguage(this->GetConfigName());

  // Compute specific libraries to link with.
  cmNinjaDeps explicitDeps = this->GetObjects();
  {
  cmNinjaDeps linkDeps = this->ComputeLinkDeps();
  for(cmNinjaDeps::const_iterator i = linkDeps.begin();
      i != linkDeps.end();
      ++i)
    explicitDeps.push_back(*i);
  std::ostringstream linkLibraries;
  this->GetLocalGenerator()->OutputLinkLibraries(linkLibraries,
                                                 *this->GetTarget(),
                                                 false);
  vars["LDLIBS"] = linkLibraries.str();
  }

  // Compute specific link flags.
  vars[this->LanguageFlagsVarName(linkLanguage)] =
    this->ComputeFlagsForLink(linkLanguage);
  vars["LDFLAGS"] = this->ComputeLinkFlags(linkLanguage);

  // Write the build statement for this target.
  cmGlobalNinjaGenerator::WriteBuild(this->GetBuildFileStream(),
                                     comment.str(),
                                     this->LanguageLinkerRule(linkLanguage),
                                     outputs,
                                     explicitDeps,
                                     emptyDeps,
                                     emptyDeps,
                                     vars);

  // Write a shortcut rule with the target name.
  this->WriteTargetBuild(this->TargetNameOut, targetOutput);
}
