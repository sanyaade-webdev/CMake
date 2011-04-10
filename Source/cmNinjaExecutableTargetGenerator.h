/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef cmNinjaExecutableTargetGenerator_h
#  define cmNinjaExecutableTargetGenerator_h

#  include "cmNinjaTargetGenerator.h"
#  include "cmNinjaTypes.h"

class cmSourceFile;

class cmNinjaExecutableTargetGenerator : public cmNinjaTargetGenerator
{
public:
  /// Build a NinjaTargetGenerator.
  cmNinjaExecutableTargetGenerator(cmTarget* target);
  ~cmNinjaExecutableTargetGenerator();

  virtual void Generate();

private:
  void WriteLanguagesRules();
  void WriteLanguageRules(const std::string& language);
  void WriteCompileRule(const std::string& language);
  void WriteLinkRule(const std::string& language);
  void WriteObjectBuildStatements();
  void WriteObjectBuildStatement(cmSourceFile* source);
  void WriteLinkStatement();
  std::string ComputeLinkFlags(const std::string& linkLanguage);

private:
  // Target name info.
  std::string TargetNameOut;
  std::string TargetNameReal;
  std::string TargetNameImport;
  std::string TargetNamePDB;
  /// List of object files for this target.
  cmNinjaDeps Objects;
};

#endif // ! cmNinjaExecutableTargetGenerator_h
