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

  /// Overloaded methods. @see cmLocalGenerator::TraceDependencies()
  virtual void TraceDependencies();

  /// Overloaded methods. @see cmLocalGenerator::AddHelperCommands()
  virtual void AddHelperCommands();

  /// Overloaded methods. @see cmLocalGenerator::ConfigureFinalPass()
  virtual void ConfigureFinalPass();

  /// Overloaded methods. @see cmLocalGenerator::GenerateInstallRules()
  virtual void GenerateInstallRules();

  /// Overloaded methods. @see cmLocalGenerator::GenerateTestFiles()
  virtual void GenerateTestFiles();

  /// Overloaded methods. @see cmLocalGenerator::GenerateTargetManifest()
  virtual void GenerateTargetManifest();

  /// Overloaded methods. @see cmLocalGenerator::ClearDependencies()
  virtual void ClearDependencies(cmMakefile* mf, bool verbose);

  /// Overloaded methods. @see cmLocalGenerator::UpdateDependencies()
  virtual bool UpdateDependencies(const char* tgtInfo,
                                  bool verbose,
                                  bool color);

  /// Overloaded methods.
  /// @see cmLocalGenerator::GetTargetObjectFileDirectories()
  /// Get the directories into which the .o files will go for this target.
  virtual void
    GetTargetObjectFileDirectories(cmTarget* target,
                                   std::vector<std::string>& dirs);

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

protected:

  /// Overloaded methods. @see cmLocalGenerator::OutputLinkLibraries()
  virtual void OutputLinkLibraries(std::ostream& fout,
                                   cmTarget& tgt,
                                   bool relink);

  /// Overloaded methods. @see cmLocalGenerator::CheckDefinition()
  virtual bool CheckDefinition(std::string const& define) const;

private:
  // In order to access to protected member of the local generator.
  friend class cmNinjaTargetGenerator;
  friend class cmNinjaExecutableTargetGenerator;
  friend class cmNinjaLibraryTargetGenerator;

private:
  cmGeneratedFileStream& GetBuildFileStream() const;
  cmGeneratedFileStream& GetRulesFileStream() const;

  void WriteBuildFileTop();
  void WriteProjectHeader(std::ostream& os);
  void WriteNinjaFilesInclusion(std::ostream& os);
  void AddDependencyToAll(const std::string& dependency);

  void SetConfigName();

private:
  std::string ConfigName;
};

#endif // ! cmLocalNinjaGenerator_h
