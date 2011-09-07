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
#include "cmComputeLinkInformation.h"
#include "cmake.h"

cmLocalNinjaGenerator::cmLocalNinjaGenerator()
  : cmLocalGenerator()
  , ConfigName("")
  , HomeRelativeOutputPath("")
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


  this->SetConfigName();

  this->WriteProcessedMakefile(this->GetBuildFileStream());
  this->WriteProcessedMakefile(this->GetRulesFileStream());

  this->WriteBuildFileTop();

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

}

// Implemented in:
//   cmLocalUnixMakefileGenerator3.
// Used in:
//   Source/cmMakefile.cxx
//   Source/cmGlobalGenerator.cxx
void cmLocalNinjaGenerator::Configure()
{
  // Compute the path to use when referencing the current output
  // directory from the top output directory.
  this->HomeRelativeOutputPath =
    this->Convert(this->Makefile->GetStartOutputDirectory(), HOME_OUTPUT);
  if(this->HomeRelativeOutputPath == ".")
    {
    this->HomeRelativeOutputPath = "";
    }
  this->cmLocalGenerator::Configure();

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

// Implemented only in cmLocalVisualStudio6Generator.
// Used in:
//   Source/cmLocalGenerator.cxx
//   Source/cmLocalVisualStudio6Generator.cxx
bool cmLocalNinjaGenerator::CheckDefinition(std::string const& define) const
{

  bool ret = cmLocalGenerator::CheckDefinition(define);

  return ret;
}

std::string cmLocalNinjaGenerator::ConvertToLinkReference(std::string const& lib)
{
  return this->Convert(lib.c_str(), HOME_OUTPUT, SHELL);
}

std::string
cmLocalNinjaGenerator::ConvertToIncludeReference(std::string const& path)
{
  return this->Convert(path.c_str(), HOME_OUTPUT, SHELL);
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

cmake* cmLocalNinjaGenerator::GetCMakeInstance() const
{
  return this->GetGlobalGenerator()->GetCMakeInstance();
}

bool cmLocalNinjaGenerator::isRootMakefile() const
{
  return (strcmp(this->Makefile->GetCurrentDirectory(),
                 this->GetCMakeInstance()->GetHomeDirectory()) == 0);
}

void cmLocalNinjaGenerator::WriteBuildFileTop()
{
  // We do that only once for the top CMakeLists.txt file.
  if(!this->isRootMakefile())
    return;

  // For the build file.
  this->WriteProjectHeader(this->GetBuildFileStream());
  this->WriteNinjaFilesInclusion(this->GetBuildFileStream());

  // For the rule file.
  this->WriteProjectHeader(this->GetRulesFileStream());
}

void cmLocalNinjaGenerator::WriteProjectHeader(std::ostream& os)
{
  cmGlobalNinjaGenerator::WriteDivider(os);
  os
    << "# Project: " << this->GetMakefile()->GetProjectName() << std::endl
    << "# Configuration: " << this->ConfigName << std::endl
    ;
  cmGlobalNinjaGenerator::WriteDivider(os);
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

void cmLocalNinjaGenerator::AddDependencyToAll(const std::string& dependency)
{
  this->GetGlobalNinjaGenerator()->AddDependencyToAll(dependency);
}

void cmLocalNinjaGenerator::WriteProcessedMakefile(std::ostream& os)
{
  cmGlobalNinjaGenerator::WriteDivider(os);
  os
    << "# Write statements declared in CMakeLists.txt:" << std::endl
    << "# " << this->Makefile->GetCurrentListFile() << std::endl
    ;
  if(this->isRootMakefile())
    os << "# Which is the root file." << std::endl;
  cmGlobalNinjaGenerator::WriteDivider(os);
  os << std::endl;
}
