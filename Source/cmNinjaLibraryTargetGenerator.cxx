/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmNinjaLibraryTargetGenerator.h"
#include "cmLocalNinjaGenerator.h"
#include "cmGlobalNinjaGenerator.h"
#include "cmSourceFile.h"
#include "cmGeneratedFileStream.h"
#include "cmMakefile.h"

cmNinjaLibraryTargetGenerator::
cmNinjaLibraryTargetGenerator(cmTarget* target)
  : cmNinjaTargetGenerator(target)
  , TargetNameOut()
  , TargetNameSO()
  , TargetNameReal()
  , TargetNameImport()
  , TargetNamePDB()
  , Objects()
{
  this->GetTarget()->GetLibraryNames(this->TargetNameOut,
                                     this->TargetNameSO,
                                     this->TargetNameReal,
                                     this->TargetNameImport,
                                     this->TargetNamePDB,
                                     GetLocalGenerator()->GetConfigName());
}

cmNinjaLibraryTargetGenerator::~cmNinjaLibraryTargetGenerator()
{
}

void cmNinjaLibraryTargetGenerator::Generate()
{
  // Write the rules for each language.
  this->WriteLanguagesRules();

  // Write the build statements
  this->WriteObjectBuildStatements();

  // // Write the link statement.
  this->WriteLinkStatement();

  this->GetBuildFileStream() << "\n";
  this->GetRulesFileStream() << "\n";
}

void cmNinjaLibraryTargetGenerator::WriteLanguagesRules()
{
  cmGlobalNinjaGenerator::WriteDivider(this->GetRulesFileStream());
  this->GetRulesFileStream()
    << "# Rules for each languages for "
    << cmTarget::TargetTypeNames(this->GetTarget()->GetType())
    << " target "
    << this->GetTarget()->GetName()
    << "\n\n";

  std::set<cmStdString> languages;
  this->GetTarget()->GetLanguages(languages);
  for(std::set<cmStdString>::const_iterator l = languages.begin();
      l != languages.end();
      ++l)
    this->WriteLanguageRules(*l);
}

void
cmNinjaLibraryTargetGenerator
::WriteLanguageRules(const std::string& language)
{
  this->GetRulesFileStream()
    << "# Rules for language " << language << "\n\n";
  this->WriteCompileRule(language);
  this->GetRulesFileStream() << "\n";
  this->WriteLinkRule(language);
}

// TODO(Nicolas Despres): Refactor with
// void cmNinjaExecutableTargetGenerator::WriteCompileRule(const std::string& language);
void
cmNinjaLibraryTargetGenerator
::WriteCompileRule(const std::string& language)
{
  cmLocalGenerator::RuleVariables vars;
  vars.RuleLauncher = "RULE_LAUNCH_COMPILE";
  vars.CMTarget = this->GetTarget();
  std::string lang = language;
  vars.Language = lang.c_str();
  vars.Source = "$in";
  vars.Object = "$out";

  // FIXME(Nicolas Despres): Factor flags construction with the
  // cmMakefileTargetGenerator::WriteObjectBuildFile
  // This is mandatory to use a local variables because the string must be
  // in memory until ExpandRuleVariables() is called.
  std::string flags = this->LanguageFlagsVarName(language, true);
  vars.Flags = flags.c_str();
  // FIXME(Nicolas Despres): Factor defines construction with the
  // cmMakefileTargetGenerator::WriteObjectBuildFile
  std::string defines = this->LanguageDefinesVarName(language, true);
  vars.Defines = defines.c_str();

  // Rule for compiling object file.
  std::string compileCmdVar = "CMAKE_";
  compileCmdVar += language;
  compileCmdVar += "_COMPILE_OBJECT";
  std::string compileCmd =
    this->GetMakefile()->GetRequiredDefinition(compileCmdVar.c_str());
  std::cout << "DEBUG NINJA: COMPILE CMD: " << compileCmd << std::endl;

  this->GetLocalGenerator()->ExpandRuleVariables(compileCmd, vars);

  // Write the rule for compiling file of the given language.
  std::ostringstream comment;
  comment << "Rule for compiling " << language << " files.";
  std::ostringstream description;
  description << "Building " << language << " object $out";
  std::string depfile = "";
  cmNinjaVars emptyVars;
  this->GetGlobalGenerator()->AddRule(this->LanguageCompilerRule(language),
                                      compileCmd,
                                      comment.str(),
                                      description.str(),
                                      depfile,
                                      emptyVars);
}

void
cmNinjaLibraryTargetGenerator
::WriteLinkRule(const std::string& language)
{
  cmLocalGenerator::RuleVariables vars;
  vars.RuleLauncher = "RULE_LAUNCH_LINK";
  vars.CMTarget = this->GetTarget();
  std::string lang = language;
  vars.Language = lang.c_str();
  vars.Objects = "$in";
  std::string objdir = cmake::GetCMakeFilesDirectoryPostSlash();
  objdir += this->GetTarget()->GetName();
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

  vars.LinkLibraries = "$LDLIBS";

  // Add language feature flags.
  // This is mandatory to use a local variables because the string must be
  // in memory until ExpandRuleVariables() is called.
  std::string langFlags = this->LanguageFlagsVarName(language, true);
  vars.LanguageCompileFlags = langFlags.c_str();
  vars.LinkFlags = "$LDFLAGS";

  // Rule for linking library.
  std::string linkCmdVar = "CMAKE_";
  linkCmdVar += language;
  linkCmdVar += "_CREATE_SHARED_LIBRARY";
  std::string linkCmd =
    this->GetMakefile()->GetRequiredDefinition(linkCmdVar.c_str());
  std::cout << "DEBUG NINJA: " << linkCmdVar << " = " << linkCmd << std::endl;

  this->GetLocalGenerator()->ExpandRuleVariables(linkCmd, vars);

  // Write the rule for linking an library.
  std::ostringstream comment;
  comment << "Rule for linking " << language << " shared library.";
  std::ostringstream description;
  description << "Linking " << language << " shared library $out";
  std::string depfile = "";
  cmNinjaVars emptyVars;
  this->GetGlobalGenerator()->AddRule(this->LanguageLinkerRule(language),
                                      linkCmd,
                                      comment.str(),
                                      description.str(),
                                      depfile,
                                      emptyVars);
}

void
cmNinjaLibraryTargetGenerator
::WriteObjectBuildStatements()
{
  // Write comments.
  cmGlobalNinjaGenerator::WriteDivider(this->GetBuildFileStream());
  this->GetBuildFileStream()
    << "# Object build statements for "
    << cmTarget::TargetTypeNames(this->GetTarget()->GetType())
    << " target "
    << this->GetTarget()->GetName()
    << "\n\n";

  // For each source files of this target.
  for(std::vector<cmSourceFile*>::const_iterator i =
        this->GetTarget()->GetSourceFiles().begin();
      i != this->GetTarget()->GetSourceFiles().end();
      ++i)
    this->WriteObjectBuildStatement(*i);

  this->GetBuildFileStream() << "\n";
}

void
cmNinjaLibraryTargetGenerator
::WriteObjectBuildStatement(cmSourceFile* source)
{
  cmNinjaDeps emptyDeps;

  std::string comment;
  const char* language = source->GetLanguage();
  // If we cannot get the language this is probably a non-source file provided in the list
  // (typically an header file).
  if (!language)
    return;
  std::string rule = this->LanguageCompilerRule(language);

  cmNinjaDeps outputs;
  std::string objectFileName =
    this->GetLocalGenerator()->GetObjectFileName(*this->GetTarget(), *source);
  outputs.push_back(objectFileName);
  // Add this object to the list of object files.
  this->Objects.push_back(objectFileName);

  cmNinjaDeps explicitDeps;
  std::string sourceFileName =
    this->GetLocalGenerator()->Convert(source->GetFullPath().c_str(),
                                       cmLocalGenerator::HOME_OUTPUT,
                                       cmLocalGenerator::MAKEFILE);
  explicitDeps.push_back(sourceFileName);

  const char* linkLanguage =
    this->GetTarget()->GetLinkerLanguage(this->GetConfigName());

  cmNinjaVars vars;
  vars[this->LanguageFlagsVarName(linkLanguage)] =
    this->ComputeFlagsForObject(linkLanguage);
  vars[this->LanguageDefinesVarName(linkLanguage)] =
    this->ComputeDefines(linkLanguage);

  cmGlobalNinjaGenerator::WriteBuild(this->GetBuildFileStream(),
                                     comment,
                                     rule,
                                     outputs,
                                     explicitDeps,
                                     emptyDeps,
                                     emptyDeps,
                                     vars);
}

void cmNinjaLibraryTargetGenerator::WriteLinkStatement()
{
  // Write comments.
  cmGlobalNinjaGenerator::WriteDivider(this->GetBuildFileStream());
  this->GetBuildFileStream()
    << "# Link build statements for "
    << cmTarget::TargetTypeNames(this->GetTarget()->GetType())
    << " target "
    << this->GetTarget()->GetName()
    << "\n\n";

  cmNinjaDeps emptyDeps;
  cmNinjaVars vars;

  // Compute the comment.
  std::ostringstream comment;
  comment << "Link the shared library " << this->TargetNameOut;

  // Compute outputs.
  cmNinjaDeps outputs;
  outputs.push_back(this->TargetNameOut);
  // Add this executable to the all target.
  this->GetLocalGenerator()->AddDependencyToAll(this->TargetNameOut);

  const char* linkLanguage =
    this->GetTarget()->GetLinkerLanguage(this->GetConfigName());

  // Compute specific libraries to link with.
  cmNinjaDeps explicitDeps = this->Objects;
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
  vars["SONAME"] = this->TargetNameSO;

  // Write the build statement for this target.
  cmGlobalNinjaGenerator::WriteBuild(this->GetBuildFileStream(),
                                     comment.str(),
                                     this->LanguageLinkerRule(linkLanguage),
                                     outputs,
                                     explicitDeps,
                                     emptyDeps,
                                     emptyDeps,
                                     vars);
}

// TODO(Nicolas Despres): Most of the code is picked up from
// void cmMakefileExecutableTargetGenerator::WriteExecutableRule(bool relink).
// Refactor it.
std::string
cmNinjaLibraryTargetGenerator
::ComputeLinkFlags(const std::string& linkLanguage)
{
  // The returned variables.
  std::string linkFlags;
  // Convenience variables.
  cmLocalNinjaGenerator* lg = this->GetLocalGenerator();
  cmTarget* target = this->GetTarget();
  cmMakefile* makefile = this->GetMakefile();

  // Add flags to create an executable.
  lg->AddConfigVariableFlags(linkFlags,
                             "CMAKE_EXE_LINKER_FLAGS",
                             this->GetConfigName());

  if(target->GetPropertyAsBool("WIN32_EXECUTABLE"))
    lg->AppendFlags(linkFlags,
                    makefile->GetDefinition("CMAKE_CREATE_WIN32_EXE"));
  else
    lg->AppendFlags(linkFlags,
                    makefile->GetDefinition("CMAKE_CREATE_CONSOLE_EXE"));

  // Add symbol export flags if necessary.
  if(target->IsExecutableWithExports())
    {
    std::string export_flag_var = "CMAKE_EXE_EXPORTS_";
    export_flag_var += linkLanguage;
    export_flag_var += "_FLAG";
    lg->AppendFlags(linkFlags,
                    makefile->GetDefinition(export_flag_var.c_str()));
    }

  // Target specific link flags.
  lg->AppendFlags(linkFlags, target->GetProperty("LINK_FLAGS"));

  // Configuration specific link flags.
  std::string linkFlagsConfig = "LINK_FLAGS_";
  linkFlagsConfig += cmSystemTools::UpperCase(this->GetConfigName());
  lg->AppendFlags(linkFlags, target->GetProperty(linkFlagsConfig.c_str()));

  // TODO(Nicolas Despres): Let's see later for the definition files.
  // It looks like something specific to Windows and Ninja is not available
  // on Windows yet.
  // this->AddModuleDefinitionFlag(linkFlags);

  return linkFlags;
}
