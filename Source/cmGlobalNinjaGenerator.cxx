/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmGlobalNinjaGenerator.h"
#include "cmLocalNinjaGenerator.h"
#include "cmMakefile.h"

cmGlobalNinjaGenerator::cmGlobalNinjaGenerator()
  : cmGlobalGenerator()
{
  // // Ninja is not ported to non-Unix OS yet.
  // this->ForceUnixPaths = true;
  this->FindMakeProgramFile = "CMakeNinjaFindMake.cmake";
}

//----------------------------------------------------------------------------
// Virtual public methods.

cmLocalGenerator* cmGlobalNinjaGenerator::CreateLocalGenerator()
{
  cmLocalGenerator* lg = new cmLocalNinjaGenerator;
  lg->SetGlobalGenerator(this);
  return lg;
}

void cmGlobalNinjaGenerator
::GetDocumentation(cmDocumentationEntry& entry) const
{
  entry.Name = this->GetName();
  entry.Brief = "Generates build.ninja files.";
  entry.Full =
    "A hierarchy of build.ninja files is generated into the build tree. Any"
    "version of the ninja program can build the project through the "
    "\"all\" target.  An \"install\" target is also provided.";
}

// Implemented by:
//   cmGlobalVisualStudio7Generator
//   cmGlobalVisualStudio8Generator
// Used in:
//   Source/cmake.cxx
void cmGlobalNinjaGenerator::Configure()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmGlobalGenerator::Configure();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented in all cmGlobaleGenerator sub-classes.
// Used in:
//   Source/cmLocalGenerator.cxx
//   Source/cmake.cxx
void cmGlobalNinjaGenerator::Generate()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmGlobalGenerator::Generate();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented in all cmGlobaleGenerator sub-classes.
// Used in:
//   Source/cmMakefile.cxx:
void cmGlobalNinjaGenerator
::EnableLanguage(std::vector<std::string>const& languages,
                 cmMakefile *mf,
                 bool optional)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: languages: ";
  for (std::vector<std::string>::const_iterator l = languages.begin();
       l != languages.end();
       ++l)
    std::cout << "'" << *l << "', ";
  std::cout << std::endl;
  std::cout << "DEBUG NINJA: ARG: cmMakefile: " << mf
            << " project: '" << mf->GetProjectName() << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: optional: " << optional << std::endl;

  cmGlobalGenerator::EnableLanguage(languages, mf, optional);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
// Used nowhere.
void cmGlobalNinjaGenerator
::EnableLanguagesFromGenerator(cmGlobalGenerator* gen,
                               cmMakefile* mf)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: cmGlobaleGenerator: " << gen
            << " name: '" << gen->GetName() << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: mf: " << mf
            << " project: '" << mf->GetProjectName() << "'" << std::endl;

  cmGlobalGenerator::EnableLanguagesFromGenerator(gen, mf);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
// Used in:
//   Source/cmMakefile.cxx
int cmGlobalNinjaGenerator::TryCompile(const char* srcdir,
                                       const char* bindir,
                                       const char* projectName,
                                       const char* targetName,
                                       bool fast,
                                       std::string* output,
                                       cmMakefile* mf)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: srcdir: '" << srcdir << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: bindir: '" << bindir << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: projectName: '" << projectName << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: fast: '" << fast << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: output: '" << *output << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: mf: " << mf
            << " project: '" << mf->GetProjectName() << "'" << std::endl;

  int ret = cmGlobalGenerator::TryCompile(srcdir,
                                          bindir,
                                          projectName,
                                          targetName,
                                          fast,
                                          output,
                                          mf);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

// Implemented by:
//   cmGlobalUnixMakefileGenerator3
//   cmGlobalVisualStudio10Generator
//   cmGlobalVisualStudio6Generator
//   cmGlobalVisualStudio7Generator
//   cmGlobalXCodeGenerator
// Called by:
//   cmGlobalGenerator::Build()
std::string cmGlobalNinjaGenerator
::GenerateBuildCommand(const char* makeProgram,
                       const char* projectName,
                       const char* additionalOptions,
                       const char* targetName,
                       const char* config,
                       bool ignoreErrors,
                       bool fast)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: makeProgram: '" << makeProgram << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: projectName: '" << projectName << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: additionalOptions: '" << (additionalOptions ? additionalOptions : "NULL") << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: targetName: '" << targetName << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: ignoreErrors: '" << ignoreErrors << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: fast: '" << fast << "'" << std::endl;

  std::string ret = cmGlobalGenerator::GenerateBuildCommand(makeProgram,
                                                            projectName,
                                                            additionalOptions,
                                                            targetName,
                                                            config,
                                                            ignoreErrors,
                                                            fast);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

// Not implemented in UnixMakefile generator.
void cmGlobalNinjaGenerator
::AppendDirectoryForConfig(const char* prefix,
                           const char* config,
                           const char* suffix,
                           std::string& dir)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: prefix: '" << prefix << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: config: '" << config << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: suffix: '" << suffix << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: dir: '" << dir << "'" << std::endl;

  cmGlobalGenerator::AppendDirectoryForConfig(prefix,
                                              config,
                                              suffix,
                                              dir);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

//----------------------------------------------------------------------------
// Virtual protected methods.

// Not implemented in UnixMakefile generator.
void cmGlobalNinjaGenerator::GetTargetSets(TargetDependSet& projectTargets,
                                           TargetDependSet& originalTargets,
                                           cmLocalGenerator* root,
                                           GeneratorVector const& generators)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;

  cmGlobalGenerator::GetTargetSets(projectTargets,
                                   originalTargets,
                                   root,
                                   generators);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in UnixMakefile generator.
bool cmGlobalNinjaGenerator::IsRootOnlyTarget(cmTarget* target)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: target: " << target
            << " name: '" << target->GetName() << "'"
            << " type: '" << cmTarget::TargetTypeNames(target->GetType()) << "'"
            << std::endl;

  bool ret = cmGlobalGenerator::IsRootOnlyTarget(target);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

// Not implemented in UnixMakefile generator.
bool cmGlobalNinjaGenerator::ComputeTargetDepends()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  bool ret = cmGlobalGenerator::ComputeTargetDepends();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

// Not implemented in UnixMakefile generator.
const char* cmGlobalNinjaGenerator::GetPredefinedTargetsFolder()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  const char* ret = cmGlobalGenerator::GetPredefinedTargetsFolder();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

// Not implemented in UnixMakefile generator.
bool cmGlobalNinjaGenerator::UseFolderProperty()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  bool ret = cmGlobalGenerator::UseFolderProperty();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}
