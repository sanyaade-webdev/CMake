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
#include "cmSystemTools.h"
#include "cmMakefile.h"

cmNinjaTargetGenerator *
cmNinjaTargetGenerator::New(cmTarget* target)
{
  switch (target->GetType())
    {
      case cmTarget::EXECUTABLE:
        return new cmNinjaExecutableTargetGenerator(target);

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
::LanguageFlagsVarName(const std::string& language) const
{
  std::ostringstream varname;
  varname << "$" << language << "FLAGS";
  return varname.str();
}
