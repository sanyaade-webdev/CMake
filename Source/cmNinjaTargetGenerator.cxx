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
#include "cmNinjaLibraryTargetGenerator.h"
#include "cmSystemTools.h"
#include "cmMakefile.h"
#include "cmComputeLinkInformation.h"

cmNinjaTargetGenerator *
cmNinjaTargetGenerator::New(cmTarget* target)
{
  switch (target->GetType())
    {
      case cmTarget::EXECUTABLE:
        return new cmNinjaExecutableTargetGenerator(target);

      case cmTarget::SHARED_LIBRARY:
        return new cmNinjaLibraryTargetGenerator(target);

      default:
        return 0;
    }
}

cmNinjaTargetGenerator::cmNinjaTargetGenerator(cmTarget* target)
  : Target(target)
  , Makefile(target->GetMakefile())
  , LocalGenerator(static_cast<cmLocalNinjaGenerator*>(Makefile->GetLocalGenerator()))
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

// TODO(Nicolas Despres): Most of the code is picked up from
// void cmMakefileExecutableTargetGenerator::WriteExecutableRule(bool relink),
// void cmMakefileTargetGenerator::WriteTargetLanguageFlags()
// Refactor it.
std::string
cmNinjaTargetGenerator::ComputeFlagsForLink(const std::string& language)
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
  cmNinjaDeps linkDeps;

  cmComputeLinkInformation* cli =
    this->Target->GetLinkInformation(this->GetConfigName());
  if(!cli)
    return linkDeps;


  // Append the link items.
  typedef cmComputeLinkInformation::ItemVector ItemVector;
  ItemVector const& items = cli->GetItems();
  for(ItemVector::const_iterator li = items.begin(); li != items.end(); ++li)
    {
    if(li->IsPath)
      {
      linkDeps.push_back(this->LocalGenerator->ConvertToLinkReference(li->Value));
      }
    else
      {
      linkDeps.push_back(li->Value);
      }
    }

  return linkDeps;
}
