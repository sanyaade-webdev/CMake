/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmLocalNinjaGenerator.h"
#include "cmMakefile.h"

cmLocalNinjaGenerator::cmLocalNinjaGenerator()
  : cmLocalGenerator()
{
  // TODO(Nicolas Despres): Maybe I should set this one to true??
  this->IsMakefileGenerator = false;
}

//----------------------------------------------------------------------------
// Virtual public methods.

cmLocalNinjaGenerator::~cmLocalNinjaGenerator()
{
}

void cmLocalNinjaGenerator::Generate()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::Generate();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented in:
//   cmLocalUnixMakefileGenerator3.
// Used in:
//   Source/cmMakefile.cxx
//   Source/cmGlobalGenerator.cxx
void cmLocalNinjaGenerator::Configure()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::Configure();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
void cmLocalNinjaGenerator::TraceDependencies()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::TraceDependencies();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented only in some VisualStudio local generator.
void cmLocalNinjaGenerator::AddHelperCommands()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::AddHelperCommands();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented only in some VisualStudio local generator.
void cmLocalNinjaGenerator::ConfigureFinalPass()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::ConfigureFinalPass();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
void cmLocalNinjaGenerator::GenerateInstallRules()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::GenerateInstallRules();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
void cmLocalNinjaGenerator::GenerateTestFiles()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::GenerateTestFiles();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
void cmLocalNinjaGenerator::GenerateTargetManifest()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::GenerateTargetManifest();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented only in UnixMakefile local generator.
// Used in:
//   Source/cmMakefileExecutableTargetGenerator.cxx
//   Source/cmMakefileLibraryTargetGenerator.cxx
//   Source/cmMakefileTargetGenerator.cxx
void cmLocalNinjaGenerator::AppendFlags(std::string& flags,
                                        const char* newFlags)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: flags: '" << flags << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: newFlags: '" << newFlags << "'" << std::endl;

  cmLocalGenerator::AppendFlags(flags, newFlags);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented only in UnixMakefile local generator.
// Used in:
//   Source/cmake.cxx
void cmLocalNinjaGenerator::ClearDependencies(cmMakefile* mf,
                                              bool verbose)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: cmMakefile: " << mf
            << " project: '" << mf->GetProjectName() << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: verbose: " << verbose << std::endl;

  cmLocalGenerator::ClearDependencies(mf, verbose);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented only in UnixMakefile local generator.
// Used in:
//   Source/cmake.cxx
bool cmLocalNinjaGenerator::UpdateDependencies(const char* tgtInfo,
                                               bool verbose,
                                               bool color)
{
  std::cout << "DEBUG NINJA: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: tgtInfo: '" << tgtInfo << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: verbose: " << verbose << std::endl;
  std::cout << "DEBUG NINJA: ARG: color: " << color << std::endl;

  bool ret = cmLocalGenerator::UpdateDependencies(tgtInfo, verbose, color);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

// Implemented in almost all cmLocalGenerator sub-classes.
// Used in:
//   Source/cmTarget.cxx:
void cmLocalNinjaGenerator
::GetTargetObjectFileDirectories(cmTarget* target,
                                 std::vector<std::string>& dirs)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: target: " << target
            << " name: '" << target->GetName() << "'"
            << " type: '" << cmTarget::TargetTypeNames(target->GetType()) << "'"
            << std::endl;
  std::cout << "DEBUG NINJA: ARG: dirs: ";
  for (std::vector<std::string>::const_iterator d = dirs.begin();
       d != dirs.end();
       ++d)
    std::cout << "'" << *d << "', ";
  std::cout << std::endl;

  cmLocalGenerator::GetTargetObjectFileDirectories(target, dirs);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented and used in almost all cmLocalGenerator sub-classes.
std::string cmLocalNinjaGenerator
::GetTargetDirectory(cmTarget const& target) const
{
  std::cout << "DEBUG NINJA: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: target: "
            << " name: '" << target.GetName() << "'"
            << " type: '" << cmTarget::TargetTypeNames(target.GetType()) << "'"
            << std::endl;

  std::string ret = cmLocalGenerator::GetTargetDirectory(target);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

//----------------------------------------------------------------------------
// Virtual protected methods.

// Not implemented in none of cmLocalGenerator sub-classes.
// Used in:
//   Source/cmLocalGenerator.cxx
//   Source/cmMakefileExecutableTargetGenerator.cxx
//   Source/cmMakefileLibraryTargetGenerator.cxx
void cmLocalNinjaGenerator::OutputLinkLibraries(std::ostream& fout,
                                                cmTarget& tgt,
                                                bool relink)
{
  std::cout << "DEBUG NINJA: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: tgt: "
            << " name: '" << tgt.GetName() << "'"
            << " type: '" << cmTarget::TargetTypeNames(tgt.GetType()) << "'"
            << std::endl;
  std::cout << "DEBUG NINJA: ARG: relink: '" << relink << "'" << std::endl;

  cmLocalGenerator::OutputLinkLibraries(fout, tgt, relink);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented only in cmLocalVisualStudio6Generator.
// Used in:
//   Source/cmLocalGenerator.cxx
//   Source/cmLocalVisualStudio6Generator.cxx
bool cmLocalNinjaGenerator::CheckDefinition(std::string const& define) const
{
  std::cout << "DEBUG NINJA: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: define: '" << define << "'" << std::endl;

  bool ret = cmLocalGenerator::CheckDefinition(define);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}
