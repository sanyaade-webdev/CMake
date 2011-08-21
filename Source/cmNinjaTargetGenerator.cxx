/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmNinjaTargetGenerator.h"
#include "cmGlobalNinjaGenerator.h"
#include "cmLocalNinjaGenerator.h"
#include "cmGeneratedFileStream.h"
#include "cmNinjaExecutableTargetGenerator.h"
#include "cmNinjaSharedLibraryTargetGenerator.h"
#include "cmNinjaStaticLibraryTargetGenerator.h"
#include "cmSystemTools.h"
#include "cmMakefile.h"
#include "cmComputeLinkInformation.h"
#include "cmSourceFile.h"

cmNinjaTargetGenerator *
cmNinjaTargetGenerator::New(cmTarget* target)
{
  switch (target->GetType())
    {
      case cmTarget::EXECUTABLE:
        return new cmNinjaExecutableTargetGenerator(target);

      case cmTarget::SHARED_LIBRARY:
        return new cmNinjaSharedLibraryTargetGenerator(target);

      case cmTarget::STATIC_LIBRARY:
        return new cmNinjaStaticLibraryTargetGenerator(target);

      default:
        return 0;
    }
}

cmNinjaTargetGenerator::cmNinjaTargetGenerator(cmTarget* target)
  : Target(target)
  , Makefile(target->GetMakefile())
  , LocalGenerator(static_cast<cmLocalNinjaGenerator*>(Makefile->GetLocalGenerator()))
  , Objects()
{
}

cmNinjaTargetGenerator::~cmNinjaTargetGenerator()
{
}

void cmNinjaTargetGenerator::Generate()
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

cmGeneratedFileStream& cmNinjaTargetGenerator::GetBuildFileStream() const
{
  return *this->GetGlobalGenerator()->GetBuildFileStream();
}

cmGeneratedFileStream& cmNinjaTargetGenerator::GetRulesFileStream() const
{
  return *this->GetGlobalGenerator()->GetRulesFileStream();
}

cmGlobalNinjaGenerator* cmNinjaTargetGenerator::GetGlobalGenerator() const
{
  return this->LocalGenerator->GetGlobalNinjaGenerator();
}

const char* cmNinjaTargetGenerator::GetConfigName() const
{
  return this->LocalGenerator->ConfigName.c_str();
}

std::string
cmNinjaTargetGenerator
::LanguageLinkerRule(const std::string& lang) const
{
  return lang
    + "_"
    + cmTarget::TargetTypeNames(this->GetTarget()->GetType())
    + "_LINKER";
}

std::string
cmNinjaTargetGenerator
::LanguageFlagsVarName(const std::string& language, bool ref) const
{
  std::ostringstream varname;
  if(ref)
    varname << "$";
  varname << language << "_FLAGS";
  return varname.str();
}

std::string
cmNinjaTargetGenerator
::LanguageDefinesVarName(const std::string& language, bool ref) const
{
  std::ostringstream varname;
  if(ref)
    varname << "$";
  varname << language << "_DEFINES";
  return varname.str();
}

// TODO(Nicolas Despres): Picked up from cmMakefileTargetGenerator.
// Refactor it.
const char* cmNinjaTargetGenerator::GetFeature(const char* feature)
{
  return this->Target->GetFeature(feature, this->GetConfigName());
}

// TODO(Nicolas Despres): Picked up from cmMakefileTargetGenerator.
// Refactor it.
bool cmNinjaTargetGenerator::GetFeatureAsBool(const char* feature)
{
  return cmSystemTools::IsOn(this->GetFeature(feature));
}

// TODO(Nicolas Despres): Picked up from cmMakefileTargetGenerator.
// Refactor it.
void cmNinjaTargetGenerator::AddFeatureFlags(std::string& flags,
                                             const char* lang)
{
  // Add language-specific flags.
  this->LocalGenerator->AddLanguageFlags(flags, lang, this->GetConfigName());

  if(this->GetFeatureAsBool("INTERPROCEDURAL_OPTIMIZATION"))
    {
    this->LocalGenerator->AppendFeatureOptions(flags, lang, "IPO");
    }
}

// TODO(Nicolas Despres): Most of the code is picked up from
// void cmMakefileExecutableTargetGenerator::WriteExecutableRule(bool relink),
// void cmMakefileTargetGenerator::WriteTargetLanguageFlags()
// Refactor it.
std::string
cmNinjaTargetGenerator::ComputeFlagsForObject(const std::string& language)
{
  std::string flags;

  this->AddFeatureFlags(flags, language.c_str());

  this->GetLocalGenerator()->AddArchitectureFlags(flags,
                                                  this->GetTarget(),
                                                  language.c_str(),
                                                  this->GetConfigName());

  // TODO(Nicolas Despres): Will see later for the Fortran support.
  // // Fortran-specific flags computed for this target.
  // if(*l == "Fortran")
  //   {
  //   this->AddFortranFlags(flags);
  //   }

  // Add shared-library flags if needed.
  {
  bool shared = ((this->Target->GetType() == cmTarget::SHARED_LIBRARY) ||
                 (this->Target->GetType() == cmTarget::MODULE_LIBRARY));
  this->GetLocalGenerator()->AddSharedFlags(flags, language.c_str(), shared);
  }

  // TODO(Nicolas Despres): Handle response file.
  // Add include directory flags.
  {
  std::string includeFlags =
    this->LocalGenerator->GetIncludeFlags(language.c_str(), false);
  this->LocalGenerator->AppendFlags(flags, includeFlags.c_str());
  }

  // Append old-style preprocessor definition flags.
  this->LocalGenerator->AppendFlags(flags, this->Makefile->GetDefineFlags());

  // TODO(Nicolas Despres): Handle Apple frameworks.
  // Add include directory flags.
  // this->LocalGenerator->AppendFlags(flags, this->GetFrameworkFlags().c_str());

  return flags;
}

// TODO(Nicolas Despres): Refact with
// void cmMakefileTargetGenerator::WriteTargetLanguageFlags().
std::string
cmNinjaTargetGenerator::
ComputeDefines(const std::string& language)
{
  std::string defines;

  // Add the export symbol definition for shared library objects.
  if(const char* exportMacro = this->Target->GetExportMacro())
    {
    this->LocalGenerator->AppendDefines(defines, exportMacro, language.c_str());
    }

  // Add preprocessor definitions for this target and configuration.
  this->LocalGenerator->AppendDefines
    (defines,
     this->Makefile->GetProperty("COMPILE_DEFINITIONS"),
     language.c_str());
  this->LocalGenerator->AppendDefines
    (defines,
     this->Target->GetProperty("COMPILE_DEFINITIONS"),
     language.c_str());
  {
  std::string defPropName = "COMPILE_DEFINITIONS_";
  defPropName += cmSystemTools::UpperCase(this->GetConfigName());
  this->LocalGenerator->AppendDefines
    (defines,
     this->Makefile->GetProperty(defPropName.c_str()),
     language.c_str());
  this->LocalGenerator->AppendDefines
    (defines,
     this->Target->GetProperty(defPropName.c_str()),
     language.c_str());
  }

  return defines;
}

cmNinjaDeps cmNinjaTargetGenerator::ComputeLinkDeps() const
{
  cmComputeLinkInformation* cli =
    this->Target->GetLinkInformation(this->GetConfigName());
  if(!cli)
    return cmNinjaDeps();

  return cli->GetDepends();
}

std::string
cmNinjaTargetGenerator
::GetSourceFilePath(cmSourceFile* source) const
{
  return this->LocalGenerator->Convert(source->GetFullPath().c_str(),
                                       cmLocalGenerator::HOME_OUTPUT,
                                       cmLocalGenerator::MAKEFILE);
}

std::string
cmNinjaTargetGenerator
::GetObjectFilePath(cmSourceFile* source) const
{
  std::string path = this->LocalGenerator->GetHomeRelativeOutputPath();
  if(!path.empty())
    path += "/";
  path += this->LocalGenerator->GetObjectFileName(*this->Target, *source);
  return path;
}

std::string cmNinjaTargetGenerator::GetTargetOutputDir() const
{
  return this->LocalGenerator->GetHomeRelativeOutputPath();
}

std::string
cmNinjaTargetGenerator
::GetTargetFilePath(const std::string& name) const
{
  std::string path = this->GetTargetOutputDir();
  if(!path.empty())
    path += "/";
  path += name;
  return path;
}

void
cmNinjaTargetGenerator
::WriteTargetBuild(const std::string& outputName,
                   const std::string& outputPath)
{
  cmNinjaDeps emptyDeps;
  cmNinjaVars emptyVars;

  if(outputName != outputPath)
    {
    std::string comment = "Shortcut target for the output name.";
    cmNinjaDeps outputs;
    outputs.push_back(outputName);
    cmNinjaDeps deps;
    deps.push_back(outputPath);
    cmGlobalNinjaGenerator::WritePhonyBuild(this->GetBuildFileStream(),
                                            comment,
                                            outputs,
                                            emptyDeps,
                                            emptyDeps,
                                            deps,
                                            emptyVars);
    }

  std::string targetName = this->GetTargetName();
  if(targetName != outputName)
    {
    std::string comment = "Shortcut target for the target name.";
    cmNinjaDeps outputs;
    outputs.push_back(targetName);
    cmNinjaDeps deps;
    deps.push_back(outputName);
    cmGlobalNinjaGenerator::WritePhonyBuild(this->GetBuildFileStream(),
                                            comment,
                                            outputs,
                                            emptyDeps,
                                            emptyDeps,
                                            deps,
                                            emptyVars);
    }
}

std::string cmNinjaTargetGenerator::GetTargetName() const
{
  return this->Target->GetName();
}

void cmNinjaTargetGenerator::WriteLanguagesRules()
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
}

void
cmNinjaTargetGenerator
::WriteLanguageRules(const std::string& language)
{
  this->GetRulesFileStream()
    << "# Rules for language " << language << "\n\n";
  this->WriteCompileRule(language);
  this->GetRulesFileStream() << "\n";
  this->WriteLinkRule(language);
}

void
cmNinjaTargetGenerator
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
  std::cerr << "DEBUG NINJA: COMPILE CMD: " << compileCmd << std::endl;

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
cmNinjaTargetGenerator
::WriteObjectBuildStatements()
{
  // Write comments.
  cmGlobalNinjaGenerator::WriteDivider(this->GetBuildFileStream());
  this->GetBuildFileStream()
    << "# Object build statements for "
    << cmTarget::TargetTypeNames(this->GetTarget()->GetType())
    << " target "
    << this->GetTargetName()
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
cmNinjaTargetGenerator
::WriteObjectBuildStatement(cmSourceFile* source)
{
  cmNinjaDeps emptyDeps;

  std::string comment;
  const char* language = source->GetLanguage();
  // If we cannot get the language this is probably a non-source file provided
  // in the list (typically an header file).
  if (!language)
    return;
  std::string rule = this->LanguageCompilerRule(language);

  cmNinjaDeps outputs;
  std::string objectFileName = this->GetObjectFilePath(source);
  outputs.push_back(objectFileName);
  // Add this object to the list of object files.
  this->Objects.push_back(objectFileName);

  cmNinjaDeps explicitDeps;
  std::string sourceFileName = this->GetSourceFilePath(source);
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

std::string
cmNinjaTargetGenerator
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
