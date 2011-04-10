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
#include "cmGlobalNinjaGenerator.h"
#include "cmNinjaTargetGenerator.h"
#include "cmGeneratedFileStream.h"
#include "cmSourceFile.h"

cmLocalNinjaGenerator::cmLocalNinjaGenerator()
  : cmLocalGenerator()
  , ConfigName("")
  , AllDependencies()
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

  this->SetConfigName();
  this->WriteProjectHeader(this->GetBuildFileStream());
  this->WriteNinjaFilesInclusion(this->GetBuildFileStream());

  cmTargets& targets = this->GetMakefile()->GetTargets();
  for(cmTargets::iterator t = targets.begin(); t != targets.end(); ++t)
    {
    cmNinjaTargetGenerator* tg = cmNinjaTargetGenerator::New(&t->second);
    if(tg)
      {
      tg->Generate();
      delete tg;
      }
    }

  this->WriteBuiltinTargets(this->GetBuildFileStream());

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

  // NOTE: cmLocalUnixMakefileGenerator3::Configure() compute
  // a path used to write rules in Makefiles later. We will see if
  // we need such a thing later.
  this->cmLocalGenerator::Configure();

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
  for(std::vector<std::string>::const_iterator d = dirs.begin();
      d != dirs.end();
      ++d)
    std::cout << "'" << *d << "', ";
  std::cout << std::endl;

  cmLocalGenerator::GetTargetObjectFileDirectories(target, dirs);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// TODO(Nicolas Despres): Picked up from cmLocalUnixMakefileGenerator3.
// Refactor it.
std::string cmLocalNinjaGenerator
::GetTargetDirectory(cmTarget const& target) const
{
  std::string dir = cmake::GetCMakeFilesDirectoryPostSlash();
  dir += target.GetName();
#if defined(__VMS)
  dir += "_dir";
#else
  dir += ".dir";
#endif
  return dir;
}

//----------------------------------------------------------------------------
// Non-virtual public methods.

cmGlobalNinjaGenerator* cmLocalNinjaGenerator::GetGlobalNinjaGenerator() const
{
  return static_cast<cmGlobalNinjaGenerator*>(this->GetGlobalGenerator());
}

// TODO(Nicolas Despres): Picked up from cmLocalUnixMakefileGenerator3.
// Refactor it.
std::string
cmLocalNinjaGenerator
::GetObjectFileName(const cmTarget& target,
                    const cmSourceFile& source)
{
  // Make sure we never hit this old case.
  if(source.GetProperty("MACOSX_PACKAGE_LOCATION"))
    {
    std::string msg = "MACOSX_PACKAGE_LOCATION set on source file: ";
    msg += source.GetFullPath();
    this->GetMakefile()->IssueMessage(cmake::INTERNAL_ERROR,
                                      msg.c_str());
    }

  // Start with the target directory.
  std::string obj = this->GetTargetDirectory(target);
  obj += "/";

  // Get the object file name without the target directory.
  std::string dir_max;
  dir_max += this->Makefile->GetCurrentOutputDirectory();
  dir_max += "/";
  dir_max += obj;
  std::string objectName =
    this->GetObjectFileNameWithoutTarget(source, dir_max, 0);
  // Append the object name to the target directory.
  obj += objectName;
  return obj;
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

//----------------------------------------------------------------------------
// Private methods.

cmGeneratedFileStream& cmLocalNinjaGenerator::GetBuildFileStream() const
{
  return *this->GetGlobalNinjaGenerator()->GetBuildFileStream();
}

cmGeneratedFileStream& cmLocalNinjaGenerator::GetRulesFileStream() const
{
  return *this->GetGlobalNinjaGenerator()->GetRulesFileStream();
}

void cmLocalNinjaGenerator::WriteProjectHeader(std::ostream& os)
{
  cmGlobalNinjaGenerator::WriteDivider(os);
  os
    << "# Project: " << this->GetMakefile()->GetProjectName() << "\n"
    << "# Configuration: " << this->ConfigName << "\n"
    << "\n"
    ;
}

void cmLocalNinjaGenerator::WriteNinjaFilesInclusion(std::ostream& os)
{
  cmGlobalNinjaGenerator::WriteDivider(os);
  os
    << "# Include auxiliary files.\n"
    << "\n"
    ;
  cmGlobalNinjaGenerator::WriteInclude(os,
                                       cmGlobalNinjaGenerator::NINJA_RULES_FILE,
                                       "Include rules file.");
  os << "\n";
}

void cmLocalNinjaGenerator::SetConfigName()
{
  // Store the configuration name that will be generated.
  if(const char* config = this->GetMakefile()->GetDefinition("CMAKE_BUILD_TYPE"))
    {
    // Use the build type given by the user.
    this->ConfigName = config;
    }
  else
    {
    // No configuration type given.
    this->ConfigName = "";
    }
}

void cmLocalNinjaGenerator::WriteBuiltinTargets(std::ostream& os)
{
  // Write headers.
  cmGlobalNinjaGenerator::WriteDivider(os);
  os << "# Built-in targets\n\n";

  this->WriteTargetAll(os);
}


void cmLocalNinjaGenerator::AddDependencyToAll(const std::string& dependency)
{
  this->AllDependencies.insert(dependency);
}

void cmLocalNinjaGenerator::WriteTargetAll(std::ostream& os)
{
  cmNinjaDeps emptyDeps;
  cmNinjaVars emptyVars;

  cmNinjaDeps outputs;
  outputs.insert("all");

  cmGlobalNinjaGenerator::WritePhonyBuild(os,
                                          "The main all target.",
                                          outputs,
                                          emptyDeps,
                                          emptyDeps,
                                          this->AllDependencies,
                                          emptyVars);
}
