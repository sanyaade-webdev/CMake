/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef cmNinjaSharedLibraryTargetGenerator_h
#  define cmNinjaSharedLibraryTargetGenerator_h

#  include "cmNinjaTargetGenerator.h"
#  include "cmNinjaTypes.h"

class cmSourceFile;

class cmNinjaSharedLibraryTargetGenerator : public cmNinjaTargetGenerator
{
public:
  /// Build a NinjaTargetGenerator.
  cmNinjaSharedLibraryTargetGenerator(cmTarget* target);
  ~cmNinjaSharedLibraryTargetGenerator();

private:
  virtual void WriteLinkRule(const std::string& language);
  virtual void WriteLinkStatement();

private:
  // Target name info.
  std::string TargetNameOut;
  std::string TargetNameSO;
  std::string TargetNameReal;
  std::string TargetNameImport;
  std::string TargetNamePDB;
};

#endif // ! cmNinjaSharedLibraryTargetGenerator_h
