/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef cmNinjaTargetGenerator_h
#  define cmNinjaTargetGenerator_h

#  include "cmStandardIncludes.h"

class cmTarget;
class cmLocalNinjaGenerator;
class cmGlobalNinjaGenerator;
class cmGeneratedFileStream;
class cmMakefile;

class cmNinjaTargetGenerator
{
public:
  /// Create a cmNinjaTargetGenerator according to the @a target's type.
  static cmNinjaTargetGenerator* New(cmTarget* target);

  /// Build a NinjaTargetGenerator.
  cmNinjaTargetGenerator(cmTarget* target);

  /// Destructor.
  ~cmNinjaTargetGenerator();

  virtual void Generate() = 0;

protected:
  cmGeneratedFileStream& GetBuildFileStream() const;
  cmGeneratedFileStream& GetRulesFileStream() const;

  cmTarget* GetTarget() const
  { return this->Target; }

  cmLocalNinjaGenerator* GetLocalGenerator() const
  { return this->LocalGenerator; }

  cmGlobalNinjaGenerator* GetGlobalGenerator() const;

  cmMakefile* GetMakefile() const
  { return this->Makefile; }

  const char* GetConfigName() const;

  std::string LanguageCompilerRule(const std::string& lang) const
  { return lang + "_COMPILER"; }

  std::string LanguageLinkerRule(const std::string& lang) const;

  std::string LanguageFlagsVarName(const std::string& language) const;

private:
  cmTarget* Target;
  cmMakefile* Makefile;
  cmLocalNinjaGenerator* LocalGenerator;
};

#endif // ! cmNinjaTargetGenerator_h
