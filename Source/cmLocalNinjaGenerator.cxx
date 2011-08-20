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
  std::cerr << "DEBUG NINJA: LOCAL GENERATOR DESTRUCTOR: "
            << this << std::endl;
}

void cmLocalNinjaGenerator::Generate()
{
  std::cerr << "DEBUG NINJA: BEGIN: "
            << __PRETTY_FUNCTION__
            << " (" << this << ")" << std::endl;
  std::cerr << "DEBUG NINJA: CURRENT DIRECTORY: "
            << this->Makefile->GetCurrentDirectory() << std::endl;
  std::cerr << "DEBUG NINJA: CURRENT OUTPUT DIRECTORY: "
            << this->Makefile->GetCurrentOutputDirectory() << std::endl;
  std::cerr << "DEBUG NINJA: CURRENT LIST FILE: "
            << this->Makefile->GetCurrentListFile() << std::endl;


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

  std::cerr << "DEBUG NINJA: END: "
            << __PRETTY_FUNCTION__
            << " (" << this << ")" << std::endl;
}

// Implemented in:
//   cmLocalUnixMakefileGenerator3.
// Used in:
//   Source/cmMakefile.cxx
//   Source/cmGlobalGenerator.cxx
void cmLocalNinjaGenerator::Configure()
{
  std::cerr << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;

  // Compute the path to use when referencing the current output
  // directory from the top output directory.
  this->HomeRelativeOutputPath =
    this->Convert(this->Makefile->GetStartOutputDirectory(), HOME_OUTPUT);
  if(this->HomeRelativeOutputPath == ".")
    {
    this->HomeRelativeOutputPath = "";
    }
  this->cmLocalGenerator::Configure();

  std::cerr << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
void cmLocalNinjaGenerator::TraceDependencies()
{
  std::cerr << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::TraceDependencies();
  std::cerr << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented only in some VisualStudio local generator.
void cmLocalNinjaGenerator::AddHelperCommands()
{
  std::cerr << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::AddHelperCommands();
  std::cerr << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented only in some VisualStudio local generator.
void cmLocalNinjaGenerator::ConfigureFinalPass()
{
  std::cerr << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::ConfigureFinalPass();
  std::cerr << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
void cmLocalNinjaGenerator::GenerateInstallRules()
{
  std::cerr << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::GenerateInstallRules();
  std::cerr << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
void cmLocalNinjaGenerator::GenerateTestFiles()
{
  std::cerr << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::GenerateTestFiles();
  std::cerr << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
void cmLocalNinjaGenerator::GenerateTargetManifest()
{
  std::cerr << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmLocalGenerator::GenerateTargetManifest();
  std::cerr << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented only in UnixMakefile local generator.
// Used in:
//   Source/cmake.cxx
void cmLocalNinjaGenerator::ClearDependencies(cmMakefile* mf,
                                              bool verbose)
{
  std::cerr << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cerr << "DEBUG NINJA: ARG: cmMakefile: " << mf
            << " project: '" << mf->GetProjectName() << "'" << std::endl;
  std::cerr << "DEBUG NINJA: ARG: verbose: " << verbose << std::endl;

  cmLocalGenerator::ClearDependencies(mf, verbose);

  std::cerr << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented only in UnixMakefile local generator.
// Used in:
//   Source/cmake.cxx
bool cmLocalNinjaGenerator::UpdateDependencies(const char* tgtInfo,
                                               bool verbose,
                                               bool color)
{
  std::cerr << "DEBUG NINJA: " << __PRETTY_FUNCTION__ << std::endl;
  std::cerr << "DEBUG NINJA: ARG: tgtInfo: '" << tgtInfo << "'" << std::endl;
  std::cerr << "DEBUG NINJA: ARG: verbose: " << verbose << std::endl;
  std::cerr << "DEBUG NINJA: ARG: color: " << color << std::endl;

  bool ret = cmLocalGenerator::UpdateDependencies(tgtInfo, verbose, color);

  std::cerr << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

// Implemented in almost all cmLocalGenerator sub-classes.
// Used in:
//   Source/cmTarget.cxx:
void cmLocalNinjaGenerator
::GetTargetObjectFileDirectories(cmTarget* target,
                                 std::vector<std::string>& dirs)
{
  std::cerr << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cerr << "DEBUG NINJA: ARG: target: " << target
            << " name: '" << target->GetName() << "'"
            << " type: '" << cmTarget::TargetTypeNames(target->GetType()) << "'"
            << std::endl;
  std::cerr << "DEBUG NINJA: ARG: dirs: ";
  for(std::vector<std::string>::const_iterator d = dirs.begin();
      d != dirs.end();
      ++d)
    std::cerr << "'" << *d << "', ";
  std::cerr << std::endl;

  cmLocalGenerator::GetTargetObjectFileDirectories(target, dirs);

  std::cerr << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
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

// TODO(Nicolas Despres): Refactor with
// void cmLocalNinjaGenerator::OutputLinkLibraries(std::ostream& fout,
//                                                 cmTarget& tgt,
//                                                 bool relink);
//
// Actually do not want the list of .so files to show up here since I want
// to use them as explicit dependencies of the link and thus to be bind to
// the $in variable. So I keep here only the the flags.
//
// The list of dependent targets is compute by
// cmNinjaTargetGenerator::ComputeLinkDeps().
void cmLocalNinjaGenerator::OutputLinkLibraries(std::ostream& fout,
                                                cmTarget& tgt,
                                                bool relink)
{
  const char* config = this->Makefile->GetDefinition("CMAKE_BUILD_TYPE");
  cmComputeLinkInformation* pcli = tgt.GetLinkInformation(config);
  if(!pcli)
    {
    return;
    }
  cmComputeLinkInformation& cli = *pcli;

  // Collect library linking flags command line options.
  std::string linkLibs;

  const char* linkLanguage = cli.GetLinkLanguage();

  std::string libPathFlag =
    this->Makefile->GetRequiredDefinition("CMAKE_LIBRARY_PATH_FLAG");
  std::string libPathTerminator =
    this->Makefile->GetSafeDefinition("CMAKE_LIBRARY_PATH_TERMINATOR");

  // Flags to link an executable to shared libraries.
  std::string linkFlagsVar = "CMAKE_SHARED_LIBRARY_LINK_";
  linkFlagsVar += linkLanguage;
  linkFlagsVar += "_FLAGS";
  if( tgt.GetType() == cmTarget::EXECUTABLE )
    {
    linkLibs = this->Makefile->GetSafeDefinition(linkFlagsVar.c_str());
    linkLibs += " ";
    }

  // Append the framework search path flags.
  std::vector<std::string> const& fwDirs = cli.GetFrameworkPaths();
  for(std::vector<std::string>::const_iterator fdi = fwDirs.begin();
      fdi != fwDirs.end(); ++fdi)
    {
    linkLibs += "-F";
    linkLibs += this->Convert(fdi->c_str(), NONE, SHELL, false);
    linkLibs += " ";
    }

  // Append the library search path flags.
  std::vector<std::string> const& libDirs = cli.GetDirectories();
  for(std::vector<std::string>::const_iterator libDir = libDirs.begin();
      libDir != libDirs.end(); ++libDir)
    {
    std::string libpath = this->ConvertToOutputForExisting(libDir->c_str());
    linkLibs += libPathFlag;
    linkLibs += libpath;
    linkLibs += libPathTerminator;
    linkLibs += " ";
    }

  // Write the library flags to the build rule.
  fout << linkLibs;

  // Get the RPATH entries.
  std::vector<std::string> runtimeDirs;
  cli.GetRPath(runtimeDirs, relink);

  // Check what kind of rpath flags to use.
  if(cli.GetRuntimeSep().empty())
    {
    // Each rpath entry gets its own option ("-R a -R b -R c")
    std::string rpath;
    for(std::vector<std::string>::iterator ri = runtimeDirs.begin();
        ri != runtimeDirs.end(); ++ri)
      {
      rpath += cli.GetRuntimeFlag();
      rpath += this->Convert(ri->c_str(), NONE, SHELL, false);
      rpath += " ";
      }
    fout << rpath;
    }
  else
    {
    // All rpath entries are combined ("-Wl,-rpath,a:b:c").
    std::string rpath = cli.GetRPathString(relink);

    // Store the rpath option in the stream.
    if(!rpath.empty())
      {
      fout << cli.GetRuntimeFlag();
      fout << this->EscapeForShell(rpath.c_str(), true);
      fout << " ";
      }
    }

  // Add the linker runtime search path if any.
  std::string rpath_link = cli.GetRPathLinkString();
  if(!cli.GetRPathLinkFlag().empty() && !rpath_link.empty())
    {
    fout << cli.GetRPathLinkFlag();
    fout << this->EscapeForShell(rpath_link.c_str(), true);
    fout << " ";
    }

  // Add standard libraries for this language.
  std::string standardLibsVar = "CMAKE_";
  standardLibsVar += cli.GetLinkLanguage();
  standardLibsVar += "_STANDARD_LIBRARIES";
  if(const char* stdLibs =
     this->Makefile->GetDefinition(standardLibsVar.c_str()))
    {
    fout << stdLibs << " ";
    }
}

// Implemented only in cmLocalVisualStudio6Generator.
// Used in:
//   Source/cmLocalGenerator.cxx
//   Source/cmLocalVisualStudio6Generator.cxx
bool cmLocalNinjaGenerator::CheckDefinition(std::string const& define) const
{
  std::cerr << "DEBUG NINJA: " << __PRETTY_FUNCTION__ << std::endl;
  std::cerr << "DEBUG NINJA: ARG: define: '" << define << "'" << std::endl;

  bool ret = cmLocalGenerator::CheckDefinition(define);

  std::cerr << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
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
