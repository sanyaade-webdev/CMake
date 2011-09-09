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
::AppendTargetOutputs(cmTarget* target, cmNinjaDeps& outputs) const {
  switch (target->GetType()) {
  case cmTarget::EXECUTABLE:
  case cmTarget::SHARED_LIBRARY:
  case cmTarget::STATIC_LIBRARY: {
    std::string name = target->GetFullName(this->GetConfigName());
    std::string dir = target->GetDirectory(this->GetConfigName());
    std::string path = ConvertToNinjaPath(dir.c_str());
    if (path.empty() || path == ".")
      outputs.push_back(name);
    else {
      path += "/";
      path += name;
      outputs.push_back(path);
    }
    break;
  }

  case cmTarget::UTILITY: {
    const std::vector<cmSourceFile*>& sources = target->GetSourceFiles();
    for(std::vector<cmSourceFile*>::const_iterator source = sources.begin();
        source != sources.end(); ++source) {
      if(cmCustomCommand* cc = (*source)->GetCustomCommand()) {
        if (!cc->GetCommandLines().empty()) {
          const std::vector<std::string>& ccoutputs = cc->GetOutputs();
          std::transform(ccoutputs.begin(), ccoutputs.end(),
                         std::back_inserter(outputs), MapToNinjaPath());
        }
      }
    }
  }
  }
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
  if (cmCustomCommand *cc = source->GetCustomCommand())
    WriteCustomCommandBuildStatement(cc);

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

  // Ensure that the target dependencies are built before any source file in the
  // target, using order-only dependencies.  These dependencies should later
  // resolve to direct header dependencies in the depfile.
  cmNinjaDeps orderOnlyDeps;
  cmTargetDependSet const& targetDeps =
    this->GetGlobalGenerator()->GetTargetDirectDepends(*this->Target);
  for (cmTargetDependSet::const_iterator i = targetDeps.begin();
       i != targetDeps.end(); ++i) {
    this->AppendTargetOutputs(*i, orderOnlyDeps);
  }

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

void cmNinjaTargetGenerator::AppendCustomCommandDeps(const cmCustomCommand *cc, cmNinjaDeps &ninjaDeps) {
  const std::vector<std::string> &deps = cc->GetDepends();
  for (std::vector<std::string>::const_iterator i = deps.begin();
       i != deps.end(); ++i) {
    std::string dep;
    if (this->LocalGenerator->GetRealDependency(i->c_str(),
                                                this->GetConfigName(), dep))
      ninjaDeps.push_back(ConvertToNinjaPath(dep.c_str()));
  }
}

std::string cmNinjaTargetGenerator::BuildCommandLine(const std::vector<std::string> &cmdLines) {
  // TODO: This will work only on Unix platforms. I don't
  // want to use a link.txt file because I will loose the benefit of the
  // $in variables. A discussion about dealing with multiple commands in
  // a rule is started here:
  // http://groups.google.com/group/ninja-build/browse_thread/thread/d515f23a78986008
  std::ostringstream cmd;
  for (std::vector<std::string>::const_iterator li = cmdLines.begin();
       li != cmdLines.end(); ++li) {
    if (li != cmdLines.begin())
      cmd << " && ";
    cmd << *li;
  }
  return cmd.str();
}

void cmNinjaTargetGenerator::AppendCustomCommandLines(const cmCustomCommand *cc, std::vector<std::string> &cmdLines) {
  cmCustomCommandGenerator ccg(*cc, this->GetConfigName(), this->Makefile);
  if (ccg.GetNumberOfCommands() > 0) {
    std::ostringstream cdCmd;
    cdCmd << "cd ";
    if (const char* wd = cc->GetWorkingDirectory())
      cdCmd << wd;
    else
      cdCmd << this->GetMakefile()->GetStartOutputDirectory();
    cmdLines.push_back(cdCmd.str());
  }
  for (unsigned i = 0; i != ccg.GetNumberOfCommands(); ++i) {
    cmdLines.push_back(ccg.GetCommand(i));
    std::string& cmd = cmdLines.back();
    ccg.AppendArguments(i, cmd);
  }
}

void cmNinjaTargetGenerator::WriteCustomCommandRule() {
  this->GetGlobalGenerator()->AddRule("CUSTOM_COMMAND",
                                      "$COMMAND",
                                      "Rule for running custom commands.",
                                      "$DESC",
                                      /*depfile*/ "",
                                      cmNinjaVars());
}

void
cmNinjaTargetGenerator::WriteCustomCommandBuildStatement(cmCustomCommand *cc) {
  if (this->GetGlobalGenerator()->SeenCustomCommand(cc))
    return;

  const std::vector<std::string> &outputs = cc->GetOutputs();
  cmNinjaDeps ninjaOutputs(outputs.size()), ninjaDeps;

  std::transform(outputs.begin(), outputs.end(),
                 ninjaOutputs.begin(), MapToNinjaPath());
  this->AppendCustomCommandDeps(cc, ninjaDeps);

  for (cmNinjaDeps::iterator i = ninjaOutputs.begin(); i != ninjaOutputs.end();
       ++i)
    this->GetGlobalGenerator()->SeenCustomCommandOutput(*i);

  std::vector<std::string> cmdLines;
  this->AppendCustomCommandLines(cc, cmdLines);

  if (cmdLines.empty()) {
    cmGlobalNinjaGenerator::WritePhonyBuild(this->GetBuildFileStream(),
                                            "Phony custom command for " +
                                              ninjaOutputs[0],
                                            ninjaOutputs,
                                            ninjaDeps,
                                            cmNinjaDeps(),
                                            cmNinjaDeps(),
                                            cmNinjaVars());
  } else {
    this->WriteCustomCommandRule();

    cmNinjaVars vars;
    vars["COMMAND"] = this->BuildCommandLine(cmdLines);
    vars["DESC"] = this->LocalGenerator->ConstructComment(*cc);

    cmGlobalNinjaGenerator::WriteBuild(this->GetBuildFileStream(),
                                       "Custom command for " + ninjaOutputs[0],
                                       "CUSTOM_COMMAND",
                                       ninjaOutputs,
                                       ninjaDeps,
                                       cmNinjaDeps(),
                                       cmNinjaDeps(),
                                       vars);
  }
}
