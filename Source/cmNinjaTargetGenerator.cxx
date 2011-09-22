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
#include "cmNinjaNormalTargetGenerator.h"
#include "cmNinjaUtilityTargetGenerator.h"
#include "cmSystemTools.h"
#include "cmMakefile.h"
#include "cmComputeLinkInformation.h"
#include "cmSourceFile.h"
#include "cmCustomCommandGenerator.h"

#include <algorithm>

cmNinjaTargetGenerator *
cmNinjaTargetGenerator::New(cmTarget* target)
{
  switch (target->GetType())
    {
      case cmTarget::EXECUTABLE:
      case cmTarget::SHARED_LIBRARY:
      case cmTarget::STATIC_LIBRARY:
      case cmTarget::MODULE_LIBRARY:
        return new cmNinjaNormalTargetGenerator(target);

      case cmTarget::UTILITY:
        return new cmNinjaUtilityTargetGenerator(target);;

      case cmTarget::GLOBAL_TARGET: {
        // We only want to process global targets that live in the home
        // (i.e. top-level) directory.  CMake creates copies of these targets
        // in every directory, which we don't need.
        cmMakefile *mf = target->GetMakefile();
        if (strcmp(mf->GetStartDirectory(), mf->GetHomeDirectory()) == 0)
          return new cmNinjaUtilityTargetGenerator(target);;
        // else fallthrough
      }

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
cmNinjaTargetGenerator::ComputeFlagsForObject(cmSourceFile *source,
                                              const std::string& language)
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

  // Add target-specific and source-specific flags.
  this->LocalGenerator->AppendFlags(flags,
                                    this->Target->GetProperty("COMPILE_FLAGS"));
  this->LocalGenerator->AppendFlags(flags,
                                    source->GetProperty("COMPILE_FLAGS"));

  // TODO(Nicolas Despres): Handle Apple frameworks.
  // Add include directory flags.
  // this->LocalGenerator->AppendFlags(flags, this->GetFrameworkFlags().c_str());

  return flags;
}

// TODO(Nicolas Despres): Refact with
// void cmMakefileTargetGenerator::WriteTargetLanguageFlags().
std::string
cmNinjaTargetGenerator::
ComputeDefines(cmSourceFile *source, const std::string& language)
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
  this->LocalGenerator->AppendDefines
    (defines,
     source->GetProperty("COMPILE_DEFINITIONS"),
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
  this->LocalGenerator->AppendDefines
    (defines,
     source->GetProperty(defPropName.c_str()),
     language.c_str());
  }

  return defines;
}

std::string cmNinjaTargetGenerator::ConvertToNinjaPath(const char *path) const
{
  return this->LocalGenerator->Convert(path,
                                       cmLocalGenerator::HOME_OUTPUT,
                                       cmLocalGenerator::MAKEFILE);
}

cmNinjaDeps cmNinjaTargetGenerator::ComputeLinkDeps() const
{
  // Static libraries never depend on anything for linking.
  if (this->Target->GetType() == cmTarget::STATIC_LIBRARY)
    return cmNinjaDeps();

  cmComputeLinkInformation* cli =
    this->Target->GetLinkInformation(this->GetConfigName());
  if(!cli)
    return cmNinjaDeps();

  const std::vector<std::string> &deps = cli->GetDepends();
  cmNinjaDeps result(deps.size());
  std::transform(deps.begin(), deps.end(), result.begin(), MapToNinjaPath());
  return result;
}

std::string
cmNinjaTargetGenerator
::GetSourceFilePath(cmSourceFile* source) const
{
  return ConvertToNinjaPath(source->GetFullPath().c_str());
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
  std::string dir = this->Target->GetDirectory(this->GetConfigName());
  cmSystemTools::MakeDirectory(dir.c_str());
  return ConvertToNinjaPath(dir.c_str());
}

std::string
cmNinjaTargetGenerator
::GetTargetFilePath(const std::string& name) const
{
  std::string path = this->GetTargetOutputDir();
  if (path.empty() || path == ".")
    return name;
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
                                            deps,
                                            emptyDeps,
                                            emptyDeps,
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
                                            deps,
                                            emptyDeps,
                                            emptyDeps,
                                            emptyVars);
    }
}

std::string cmNinjaTargetGenerator::GetTargetName() const
{
  return this->Target->GetName();
}

void
cmNinjaTargetGenerator
::WriteLanguageRules(const std::string& language)
{
  this->GetRulesFileStream()
    << "# Rules for language " << language << "\n\n";
  this->WriteCompileRule(language);
  this->GetRulesFileStream() << "\n";
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

  if (this->GetTargetName() == "cmTryCompileExec")
    vars.Flags = "$FLAGS";
  else
    vars.Flags = "-MMD -MF $out.d $FLAGS";
  vars.Defines = "$DEFINES";

  // Rule for compiling object file.
  std::string compileCmdVar = "CMAKE_";
  compileCmdVar += language;
  compileCmdVar += "_COMPILE_OBJECT";
  std::string compileCmd =
    this->GetMakefile()->GetRequiredDefinition(compileCmdVar.c_str());

  this->GetLocalGenerator()->ExpandRuleVariables(compileCmd, vars);

  // Write the rule for compiling file of the given language.
  std::ostringstream comment;
  comment << "Rule for compiling " << language << " files.";
  std::ostringstream description;
  description << "Building " << language << " object $out";
  std::string depfile = "$out.d";
  this->GetGlobalGenerator()->AddRule(this->LanguageCompilerRule(language),
                                      compileCmd,
                                      description.str(),
                                      comment.str(),
                                      depfile);
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
  if (cmCustomCommand *cc = source->GetCustomCommand())
    this->GetLocalGenerator()->AddCustomCommandTarget(cc, this->GetTarget());

  cmNinjaDeps emptyDeps;

  std::string comment;
  const char* language = source->GetLanguage();
  // If we cannot get the language this is probably a non-source file provided
  // in the list (typically an header file).
  if (!language) {
    if (source->GetPropertyAsBool("EXTERNAL_OBJECT"))
      this->Objects.push_back(this->GetSourceFilePath(source));
    return;
  }

  if (source->GetPropertyAsBool("HEADER_FILE_ONLY"))
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

  // Ensure that the target dependencies are built before any source file in the
  // target, using order-only dependencies.
  cmNinjaDeps orderOnlyDeps;
  this->GetLocalGenerator()->AppendTargetDepends(this->Target, orderOnlyDeps);

  if(const char* objectDeps = source->GetProperty("OBJECT_DEPENDS")) {
    std::vector<std::string> depList;
    cmSystemTools::ExpandListArgument(objectDeps, depList);
    std::transform(depList.begin(), depList.end(),
                   std::back_inserter(orderOnlyDeps), MapToNinjaPath());
  }

  // Add order-only dependency on any header file with a custom command.
  {
    const std::vector<cmSourceFile*>& sources =
      this->GetTarget()->GetSourceFiles();
    for(std::vector<cmSourceFile*>::const_iterator si = sources.begin();
        si != sources.end(); ++si) {
      if (!(*si)->GetLanguage()) {
        if (cmCustomCommand* cc = (*si)->GetCustomCommand()) {
          const std::vector<std::string>& ccoutputs = cc->GetOutputs();
          std::transform(ccoutputs.begin(), ccoutputs.end(),
                         std::back_inserter(orderOnlyDeps), MapToNinjaPath());
        }
      }
    }
  }

  // If the source file is GENERATED and does not have a custom command
  // (either attached to this source file or another one), assume that one of
  // the target dependencies, OBJECT_DEPENDS or header file custom commands
  // will rebuild the file.
  if (source->GetPropertyAsBool("GENERATED") && !source->GetCustomCommand() &&
      !this->GetGlobalGenerator()->HasCustomCommandOutput(sourceFileName)) {
    this->GetGlobalGenerator()->AddAssumedSourceDependencies(sourceFileName,
                                                             orderOnlyDeps);
  }
  
  cmNinjaVars vars;
  vars["FLAGS"] = this->ComputeFlagsForObject(source, language);
  vars["DEFINES"] = this->ComputeDefines(source, language);

  cmGlobalNinjaGenerator::WriteBuild(this->GetBuildFileStream(),
                                     comment,
                                     rule,
                                     outputs,
                                     explicitDeps,
                                     emptyDeps,
                                     orderOnlyDeps,
                                     vars);
}
