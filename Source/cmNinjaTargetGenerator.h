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
#  include "cmNinjaTypes.h"

class cmTarget;
class cmLocalNinjaGenerator;
class cmGlobalNinjaGenerator;
class cmGeneratedFileStream;
class cmMakefile;
class cmSourceFile;

class cmNinjaTargetGenerator
{
public:
  /// Create a cmNinjaTargetGenerator according to the @a target's type.
  static cmNinjaTargetGenerator* New(cmTarget* target);

  /// Build a NinjaTargetGenerator.
  cmNinjaTargetGenerator(cmTarget* target);

  /// Destructor.
  ~cmNinjaTargetGenerator();

  virtual void Generate();

protected:
  cmGeneratedFileStream& GetBuildFileStream() const;
  cmGeneratedFileStream& GetRulesFileStream() const;

  cmTarget* GetTarget() const
  { return this->Target; }

  std::string GetTargetName() const;

  cmLocalNinjaGenerator* GetLocalGenerator() const
  { return this->LocalGenerator; }

  cmGlobalNinjaGenerator* GetGlobalGenerator() const;

  cmMakefile* GetMakefile() const
  { return this->Makefile; }

  const char* GetConfigName() const;

  std::string LanguageCompilerRule(const std::string& lang) const
  { return lang + "_COMPILER"; }

  std::string LanguageLinkerRule(const std::string& lang) const;

  std::string LanguageFlagsVarName(const std::string& language,
                                   bool ref = false) const;
  std::string LanguageDefinesVarName(const std::string& language,
                                     bool ref = false) const;

  const char* GetFeature(const char* feature);
  bool GetFeatureAsBool(const char* feature);
  void AddFeatureFlags(std::string& flags, const char* lang);

  /**
   * Compute the flags for compilation of object files for a given @a language.
   * @note Generally it is the value of the variable whose name is computed
   *       by LanguageFlagsVarName().
   */
  std::string ComputeFlagsForObject(const std::string& language);

  /**
   * Compute the flags for the linking of a target for a given @a language.
   * @note Generally it is the value of the variable whose name is computed
   *       by LanguageFlagsVarName().
   */
  std::string ComputeFlagsForLink(const std::string& language);

  std::string ComputeDefines(const std::string& language);

  /// @return the list of link dependency for the given target @a target.
  cmNinjaDeps ComputeLinkDeps() const;

  /// @return the source file path for the given @a source.
  std::string GetSourceFilePath(cmSourceFile* source) const;

  /// @return the object file path for the given @a source.
  std::string GetObjectFilePath(cmSourceFile* source) const;

  /// @return the file path where the target named @a name is generated.
  std::string GetTargetFilePath(const std::string& name) const;

  /// @return the output path for the target.
  virtual std::string GetTargetOutputDir() const;

  /**
   * Write shortcut build statements for the target name with the target
   * output name and the target output path.
   */
  void WriteTargetBuild(const std::string& outputName,
                        const std::string& outputPath);

  void WriteLanguagesRules();
  void WriteLanguageRules(const std::string& language);
  void WriteCompileRule(const std::string& language);
  virtual void WriteLinkRule(const std::string& language) = 0;
  void WriteObjectBuildStatements();
  void WriteObjectBuildStatement(cmSourceFile* source);
  virtual void WriteLinkStatement() = 0;
  std::string ComputeLinkFlags(const std::string& linkLanguage);

  cmNinjaDeps GetObjects() const
  { return this->Objects; }

private:
  cmTarget* Target;
  cmMakefile* Makefile;
  cmLocalNinjaGenerator* LocalGenerator;
  /// List of object files for this target.
  cmNinjaDeps Objects;
};

#endif // ! cmNinjaTargetGenerator_h
