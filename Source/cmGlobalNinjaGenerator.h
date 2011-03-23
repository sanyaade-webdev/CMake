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
#  include "cmNinjaTypes.h"

class cmLocalGenerator;
class cmGeneratedFileStream;

/**
 * \class cmGlobalNinjaGenerator
 * \brief Write a build.ninja file.
 *
 * The main differences between this generator and the UnixMakefile
 * generator family are:
 * - We don't care about VERBOSE variable or RULE_MESSAGES property since
 *   it is handle by Ninja's -v option.
 * - We don't care about computing any progress status since ninja manages
 *   it itself.
 */
class cmGlobalNinjaGenerator : public cmGlobalGenerator
{
public:
  /// The default name of Ninja's build file. Typically: build.ninja.
  static const char* NINJA_BUILD_FILE;

  /// The default name of Ninja's rules file. Typically: rules.ninja.
  /// It is included in the main build.ninja file.
  static const char* NINJA_RULES_FILE;

  /// The indentation string used when generating Ninja's build file.
  static const char* INDENT;

  /// Write @a count times INDENT level to output stream @a os.
  static void Indent(std::ostream& os, int count);

  /// Write a divider in the given output stream @a os.
  static void WriteDivider(std::ostream& os);

  /**
   * Write the given @a comment to the output stream @a os. It
   * handles new line character properly.
   */
  static void WriteComment(std::ostream& os, const std::string& comment);

  /**
   * Write a build statement to @a os with the @a comment using
   * the @a rule the list of @a outputs files and inputs.
   * It also writes the variables bound to this build statement.
   * @warning no escaping of any kind is done here.
   */
  static void WriteBuild(std::ostream& os,
                         const std::string& comment,
                         const std::string& rule,
                         const cmNinjaDeps& outputs,
                         const cmNinjaDeps& explicitDeps,
                         const cmNinjaDeps& implicitDeps,
                         const cmNinjaDeps& orderOnlyDeps,
                         const cmNinjaVars& variables);

  /**
   * Helper to write a build statement with the special 'phony' rule.
   */
  static void WritePhonyBuild(std::ostream& os,
                              const std::string& comment,
                              const cmNinjaDeps& outputs,
                              const cmNinjaDeps& explicitDeps,
                              const cmNinjaDeps& implicitDeps,
                              const cmNinjaDeps& orderOnlyDeps,
                              const cmNinjaVars& variables);

  /**
   * Write a rule statement named @a name to @a os with the @a comment,
   * the mandatory @a command, the @a depfile and the @a description.
   * It also writes the variables bound to this rule statement.
   * @warning no escaping of any kind is done here.
   */
  static void WriteRule(std::ostream& os,
                        const std::string& name,
                        const std::string& command,
                        const std::string& comment,
                        const std::string& description,
                        const std::string& depfile,
                        const cmNinjaVars& variables);

  /**
   * Write a variable named @a name to @a os with value @a value and an
   * optional @a comment. An @a indent level can be specified.
   * @warning no escaping of any kind is done here.
   */
  static void WriteVariable(std::ostream& os,
                            const std::string& name,
                            const std::string& value,
                            const std::string& comment = "",
                            int indent = 0);

  /**
   * Write an include statement including @a filename with an optional
   * @a comment to the @a os stream.
   */
  static void WriteInclude(std::ostream& os,
                           const std::string& filename,
                           const std::string& comment = "");

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

public:
  cmGeneratedFileStream* GetBuildFileStream() const
  { return this->BuildFileStream; }

  cmGeneratedFileStream* GetRulesFileStream() const
  { return this->RulesFileStream; }

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

private:
  void OpenBuildFileStream();
  void CloseBuildFileStream();

  void OpenRulesFileStream();
  void CloseRulesFileStream();

  /// Write the common disclaimer text at the top of each build file.
  void WriteDisclaimer(std::ostream& os);

private:
  /// The file containing the build statement. (the relation ship of the
  /// compilation DAG).
  cmGeneratedFileStream* BuildFileStream;
  /// The file containing the rule statements. (The action attached to each
  /// edge of the compilation DAG).
  cmGeneratedFileStream* RulesFileStream;
};

#endif // ! cmGlobalNinjaGenerator_h
