/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef cmGlobalNinjaGenerator_h
#  define cmGlobalNinjaGenerator_h

#  include "cmGlobalGenerator.h"

class cmLocalGenerator;

/**
 * \class cmGlobalNinjaGenerator
 * \brief Write a build.ninja file.
 *
 */
class cmGlobalNinjaGenerator : public cmGlobalGenerator
{
public:
  /// Default constructor.
  cmGlobalNinjaGenerator();

  /// Convenience method for creating an instance of this class.
  static cmGlobalGenerator* New() {
    return new cmGlobalNinjaGenerator; }

  /// Destructor.
  virtual ~cmGlobalNinjaGenerator() { }

  /// Overloaded methods. @see cmGlobalGenerator::CreateLocalGenerator()
  virtual cmLocalGenerator* CreateLocalGenerator();

  /// Overloaded methods. @see cmGlobalGenerator::GetName().
  virtual const char* GetName() const {
    return cmGlobalNinjaGenerator::GetActualName(); }

  /// @return the name of this generator.
  static const char* GetActualName() { return "Ninja (Experimental)"; }

  /// Overloaded methods. @see cmGlobalGenerator::GetDocumentation()
  virtual void GetDocumentation(cmDocumentationEntry& entry) const;

  /// Overloaded methods. @see cmGlobalGenerator::Configure()
  virtual void Configure();

  /// Overloaded methods. @see cmGlobalGenerator::Gonfigure()
  virtual void Generate();

  /// Overloaded methods. @see cmGlobalGenerator::EnableLanguage()
  virtual void EnableLanguage(std::vector<std::string>const& languages,
                              cmMakefile* mf,
                              bool optional);

  /// Overloaded methods. @see cmGlobalGenerator::EnableLanguagesFromGenerator()
  virtual void EnableLanguagesFromGenerator(cmGlobalGenerator* gen,
                                            cmMakefile* mf);

  /// Overloaded methods. @see cmGlobalGenerator::TryCompile()
  virtual int TryCompile(const char* srcdir,
                         const char* bindir,
                         const char* projectName,
                         const char* targetName,
                         bool fast, std::string* output,
                         cmMakefile* mf);

  /// Overloaded methods. @see cmGlobalGenerator::GenerateBuildCommand()
  virtual std::string GenerateBuildCommand(const char* makeProgram,
                                           const char* projectName,
                                           const char* additionalOptions,
                                           const char* targetName,
                                           const char* config,
                                           bool ignoreErrors,
                                           bool fast);

  /// Overloaded methods.
  /// @see cmGlobalGenerator::AppendDirectoryForConfig()
  virtual void AppendDirectoryForConfig(const char* prefix,
                                        const char* config,
                                        const char* suffix,
                                        std::string& dir);

  // Setup target names
  virtual const char* GetAllTargetName()           const { return "all"; }
  virtual const char* GetInstallTargetName()       const { return "install"; }
  virtual const char* GetInstallLocalTargetName()  const { return "install/local"; }
  virtual const char* GetInstallStripTargetName()  const { return "install/strip"; }
  virtual const char* GetPreinstallTargetName()    const { return "preinstall"; }
  virtual const char* GetTestTargetName()          const { return "test"; }
  virtual const char* GetPackageTargetName()       const { return "package"; }
  virtual const char* GetPackageSourceTargetName() const { return "package_source"; }
  virtual const char* GetEditCacheTargetName()     const { return "edit_cache"; }
  virtual const char* GetRebuildCacheTargetName()  const { return "rebuild_cache"; }
  virtual const char* GetCleanTargetName()         const { return "clean"; }

protected:

  /// Overloaded methods. @see cmGlobalGenerator::GetTargetSets()
  virtual void GetTargetSets(TargetDependSet& projectTargets,
                             TargetDependSet& originalTargets,
                             cmLocalGenerator* root,
                             GeneratorVector const& generators);
  /// Overloaded methods. @see cmGlobalGenerator::IsRootOnlyTarget()
  virtual bool IsRootOnlyTarget(cmTarget* target);

  /// Overloaded methods. @see cmGlobalGenerator::ComputeTargetDepends()
  virtual bool ComputeTargetDepends();

  /// Overloaded methods.
  /// @see cmGlobalGenerator::CheckALLOW_DUPLICATE_CUSTOM_TARGETS()
  virtual bool CheckALLOW_DUPLICATE_CUSTOM_TARGETS() { return true; }

  /// Overloaded methods. @see cmGlobalGenerator::GetPredefinedTargetsFolder()
  virtual const char* GetPredefinedTargetsFolder();

  /// Overloaded methods. @see cmGlobalGenerator::UseFolderProperty()
  virtual bool UseFolderProperty();
};

#endif // ! cmGlobalNinjaGenerator_h
