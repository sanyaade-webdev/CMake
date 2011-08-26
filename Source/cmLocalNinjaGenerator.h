/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef cmLocalNinjaGenerator_h
#  define cmLocalNinjaGenerator_h

#  include "cmLocalGenerator.h"
#  include "cmNinjaTypes.h"

class cmGlobalNinjaGenerator;
class cmGeneratedFileStream;
class cmake;

/**
 * \class cmLocalNinjaGenerator
 * \brief Write a local build.ninja file.
 *
 * cmLocalNinjaGenerator produces a local build.ninja file from its
 * member Makefile.
 */
class cmLocalNinjaGenerator : public cmLocalGenerator
{
public:
  /// Default constructor.
  cmLocalNinjaGenerator();

  /// Destructor.
  virtual ~cmLocalNinjaGenerator();

  /// Overloaded methods. @see cmLocalGenerator::Generate()
  virtual void Generate();

  /// Overloaded methods. @see cmLocalGenerator::Configure()
  virtual void Configure();

  /// Overloaded methods. @see cmLocalGenerator::GetTargetDirectory()
  virtual std::string GetTargetDirectory(cmTarget const& target) const;

public:
  cmGlobalNinjaGenerator* GetGlobalNinjaGenerator() const;

  /**
   * Shortcut to get the cmake instance throw the global generator.
   * @return an instance of the cmake object.
   */
  cmake* GetCMakeInstance() const;

  const char* GetConfigName() const
  { return this->ConfigName.c_str(); }

  std::string GetObjectFileName(const cmTarget& target,
                                const cmSourceFile& source);

  /// @return whether we are processing the top CMakeLists.txt file.
  bool isRootMakefile() const;

  /// @returns the relative path between the HomeOutputDirectory and this
  /// local generators StartOutputDirectory.
  std::string GetHomeRelativeOutputPath() const
  { return this->HomeRelativeOutputPath; }

protected:

  /// Overloaded methods. @see cmLocalGenerator::CheckDefinition()
  virtual bool CheckDefinition(std::string const& define) const;

  virtual std::string ConvertToLinkReference(std::string const& lib);

private:
  // In order to access to protected member of the local generator.
  friend class cmNinjaTargetGenerator;
  friend class cmNinjaNormalTargetGenerator;

private:
  cmGeneratedFileStream& GetBuildFileStream() const;
  cmGeneratedFileStream& GetRulesFileStream() const;

  void WriteBuildFileTop();
  void WriteProjectHeader(std::ostream& os);
  void WriteNinjaFilesInclusion(std::ostream& os);
  void AddDependencyToAll(const std::string& dependency);
  void WriteProcessedMakefile(std::ostream& os);

  void SetConfigName();

private:
  std::string ConfigName;
  std::string HomeRelativeOutputPath;
};

#endif // ! cmLocalNinjaGenerator_h
