/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2011 Nicolas Despres <nicolas.despres@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmGlobalNinjaGenerator.h"
#include "cmLocalNinjaGenerator.h"
#include "cmMakefile.h"
#include "cmGeneratedFileStream.h"
#include "cmVersion.h"

const char* cmGlobalNinjaGenerator::NINJA_BUILD_FILE = "build.ninja";
const char* cmGlobalNinjaGenerator::NINJA_RULES_FILE = "rules.ninja";
const char* cmGlobalNinjaGenerator::INDENT = "  ";

void cmGlobalNinjaGenerator::Indent(std::ostream& os, int count)
{
  for(int i = 0; i < count; ++i)
    os << cmGlobalNinjaGenerator::INDENT;
}

void cmGlobalNinjaGenerator::WriteDivider(std::ostream& os)
{
  os
    << "# ======================================"
    << "=======================================\n";
}

void cmGlobalNinjaGenerator::WriteComment(std::ostream& os,
                                          const std::string& comment)
{
  if (comment.empty())
    return;

  std::string replace = comment;
  std::string::size_type lpos = 0;
  std::string::size_type rpos;
  while((rpos = replace.find('\n', lpos)) != std::string::npos)
    {
    os << "# " << replace.substr(lpos, rpos - lpos) << "\n";
    lpos = rpos + 1;
    }
  os << "# " << replace.substr(lpos) << "\n";
}

void cmGlobalNinjaGenerator::WriteBuild(std::ostream& os,
                                        const std::string& comment,
                                        const std::string& rule,
                                        const cmNinjaDeps& outputs,
                                        const cmNinjaDeps& explicitDeps,
                                        const cmNinjaDeps& implicitDeps,
                                        const cmNinjaDeps& orderOnlyDeps,
                                        const cmNinjaVars& variables)
{
  // Make sure there is a rule.
  if(rule.empty())
    {
    cmSystemTools::Error("No rule for WriteBuildStatement! called "
                         "with comment: ",
                         comment.c_str());
    return;
    }

  // Make sure there is at least one output file.
  if(outputs.empty())
    {
    cmSystemTools::Error("No output files for WriteBuildStatement! called "
                         "with comment: ",
                         comment.c_str());
    return;
    }

  // Make sure there is at least one input file.
  if(explicitDeps.empty() && implicitDeps.empty() && orderOnlyDeps.empty())
    {
    cmSystemTools::Error("No input files for WriteBuildStatement! called "
                         "with comment: ",
                         comment.c_str());
    return;
    }

  cmGlobalNinjaGenerator::WriteComment(os, comment);

  // TODO(Nicolas Despres): Write one file per line when there is multiple
  // input/output files.

  // Write outputs files.
  os << "build";
  for(cmNinjaDeps::const_iterator i = outputs.begin();
      i != outputs.end();
      ++i)
    os << " " << *i;
  os << ":";

  // Write the rule.
  os << " " << rule;

  // Write explicit dependencies.
  for(cmNinjaDeps::const_iterator i = explicitDeps.begin();
      i != explicitDeps.end();
      ++i)
    os  << " " << *i;

  // Write implicit dependencies.
  if(!implicitDeps.empty())
    {
    os << " |";
    for(cmNinjaDeps::const_iterator i = implicitDeps.begin();
        i != implicitDeps.end();
        ++i)
      os  << " " << *i;
    }

  // Write order-only dependencies.
  if(!orderOnlyDeps.empty())
    {
    os << " ||";
    for(cmNinjaDeps::const_iterator i = orderOnlyDeps.begin();
        i != orderOnlyDeps.end();
        ++i)
      os  << " " << *i;
    }

  os << "\n";

  // Write the variables bound to this build statement.
  if(!variables.empty())
    {
    for(cmNinjaVars::const_iterator i = variables.begin();
        i != variables.end();
        ++i)
      cmGlobalNinjaGenerator::WriteVariable(os, i->first, i->second, "", 1);
    }
}

void cmGlobalNinjaGenerator::WritePhonyBuild(std::ostream& os,
                                             const std::string& comment,
                                             const cmNinjaDeps& outputs,
                                             const cmNinjaDeps& explicitDeps,
                                             const cmNinjaDeps& implicitDeps,
                                             const cmNinjaDeps& orderOnlyDeps,
                                             const cmNinjaVars& variables)
{
  cmGlobalNinjaGenerator::WriteBuild(os,
                                     comment,
                                     "phony",
                                     outputs,
                                     explicitDeps,
                                     implicitDeps,
                                     orderOnlyDeps,
                                     variables);
}

void cmGlobalNinjaGenerator::WriteRule(std::ostream& os,
                                       const std::string& name,
                                       const std::string& command,
                                       const std::string& comment,
                                       const std::string& description,
                                       const std::string& depfile,
                                       const cmNinjaVars& variables)
{
  // Make sure the rule has a name.
  if(name.empty())
    {
    cmSystemTools::Error("No name given for WriteRuleStatement! called "
                         "with comment: ",
                         comment.c_str());
    return;
    }

  // Make sure a command is given.
  if(command.empty())
    {
    cmSystemTools::Error("No command given for WriteRuleStatement! called "
                         "with comment: ",
                         comment.c_str());
    return;
    }

  cmGlobalNinjaGenerator::WriteComment(os, comment);

  // Write the rule.
  os << "rule " << name << "\n";

  // Write the depfile if any.
  if(!depfile.empty())
    {
    cmGlobalNinjaGenerator::Indent(os, 1);
    os << "depfile = " << depfile << "\n";
    }

  // Write the command.
  cmGlobalNinjaGenerator::Indent(os, 1);
  os << "command = " << command << "\n";

  // Write the description if any.
  if(!description.empty())
    {
    cmGlobalNinjaGenerator::Indent(os, 1);
    os << "description = " << description << "\n";
    }

  // Write the variables bound to this build statement.
  if(!variables.empty())
    {
    for(cmNinjaVars::const_iterator i = variables.begin();
        i != variables.end();
        ++i)
      cmGlobalNinjaGenerator::WriteVariable(os, i->first, i->second, "", 1);
    }
}

void cmGlobalNinjaGenerator::WriteVariable(std::ostream& os,
                                           const std::string& name,
                                           const std::string& value,
                                           const std::string& comment,
                                           int indent)
{
  // Make sure we have a name.
  if(name.empty())
    {
    cmSystemTools::Error("No name given for WriteVariable! called "
                         "with comment: ",
                         comment.c_str());
    return;
    }

  // Do not add a variable if the value is empty.
  std::string val = cmSystemTools::Trimmed(value);
  if(val.empty())
    {
    return;
    }

  cmGlobalNinjaGenerator::WriteComment(os, comment);
  cmGlobalNinjaGenerator::Indent(os, indent);
  os << name << " = " << val << "\n";
}

void cmGlobalNinjaGenerator::WriteInclude(std::ostream& os,
                                          const std::string& filename,
                                          const std::string& comment)
{
  cmGlobalNinjaGenerator::WriteComment(os, comment);
  os << "include " << filename << "\n";
}


cmGlobalNinjaGenerator::cmGlobalNinjaGenerator()
  : cmGlobalGenerator()
  , BuildFileStream(0)
  , RulesFileStream(0)
  , Rules()
{
  // // Ninja is not ported to non-Unix OS yet.
  // this->ForceUnixPaths = true;
  this->FindMakeProgramFile = "CMakeNinjaFindMake.cmake";
}

//----------------------------------------------------------------------------
// Virtual public methods.

cmLocalGenerator* cmGlobalNinjaGenerator::CreateLocalGenerator()
{
  cmLocalGenerator* lg = new cmLocalNinjaGenerator;
  lg->SetGlobalGenerator(this);
  return lg;
}

void cmGlobalNinjaGenerator
::GetDocumentation(cmDocumentationEntry& entry) const
{
  entry.Name = this->GetName();
  entry.Brief = "Generates build.ninja files.";
  entry.Full =
    "A hierarchy of build.ninja files is generated into the build tree. Any"
    "version of the ninja program can build the project through the "
    "\"all\" target.  An \"install\" target is also provided.";
}

// Implemented by:
//   cmGlobalVisualStudio7Generator
//   cmGlobalVisualStudio8Generator
// Used in:
//   Source/cmake.cxx
void cmGlobalNinjaGenerator::Configure()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  cmGlobalGenerator::Configure();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented in all cmGlobaleGenerator sub-classes.
// Used in:
//   Source/cmLocalGenerator.cxx
//   Source/cmake.cxx
void cmGlobalNinjaGenerator::Generate()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;

  this->OpenBuildFileStream();
  this->OpenRulesFileStream();

  this->cmGlobalGenerator::Generate();

  this->CloseRulesFileStream();
  this->CloseBuildFileStream();

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Implemented in all cmGlobaleGenerator sub-classes.
// Used in:
//   Source/cmMakefile.cxx:
void cmGlobalNinjaGenerator
::EnableLanguage(std::vector<std::string>const& languages,
                 cmMakefile *mf,
                 bool optional)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: languages: ";
  for (std::vector<std::string>::const_iterator l = languages.begin();
       l != languages.end();
       ++l)
    std::cout << "'" << *l << "', ";
  std::cout << std::endl;
  std::cout << "DEBUG NINJA: ARG: cmMakefile: " << mf
            << " project: '" << mf->GetProjectName() << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: optional: " << optional << std::endl;

  this->cmGlobalGenerator::EnableLanguage(languages, mf, optional);
  // NOTE: cmGlobalUnixMakefileGenerator3::EnableLanguage seems to
  // do some checking and cleaning about the CMAKE_<lang>_COMPILER
  // variables. Maybe we should add it too??

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
// Used nowhere.
void cmGlobalNinjaGenerator
::EnableLanguagesFromGenerator(cmGlobalGenerator* gen,
                               cmMakefile* mf)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: cmGlobaleGenerator: " << gen
            << " name: '" << gen->GetName() << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: mf: " << mf
            << " project: '" << mf->GetProjectName() << "'" << std::endl;

  cmGlobalGenerator::EnableLanguagesFromGenerator(gen, mf);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in none of cmLocalGenerator sub-classes.
// Used in:
//   Source/cmMakefile.cxx
int cmGlobalNinjaGenerator::TryCompile(const char* srcdir,
                                       const char* bindir,
                                       const char* projectName,
                                       const char* targetName,
                                       bool fast,
                                       std::string* output,
                                       cmMakefile* mf)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: srcdir: '" << srcdir << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: bindir: '" << bindir << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: projectName: '" << projectName << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: fast: '" << fast << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: output: '" << *output << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: mf: " << mf
            << " project: '" << mf->GetProjectName() << "'" << std::endl;

  int ret = cmGlobalGenerator::TryCompile(srcdir,
                                          bindir,
                                          projectName,
                                          targetName,
                                          fast,
                                          output,
                                          mf);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

// Implemented by:
//   cmGlobalUnixMakefileGenerator3
//   cmGlobalVisualStudio10Generator
//   cmGlobalVisualStudio6Generator
//   cmGlobalVisualStudio7Generator
//   cmGlobalXCodeGenerator
// Called by:
//   cmGlobalGenerator::Build()
std::string cmGlobalNinjaGenerator
::GenerateBuildCommand(const char* makeProgram,
                       const char* projectName,
                       const char* additionalOptions,
                       const char* targetName,
                       const char* config,
                       bool ignoreErrors,
                       bool fast)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: makeProgram: '" << makeProgram << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: projectName: '" << projectName << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: additionalOptions: '" << (additionalOptions ? additionalOptions : "NULL") << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: targetName: '" << targetName << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: ignoreErrors: '" << ignoreErrors << "'" << std::endl;
  std::cout << "DEBUG NINJA: ARG: fast: '" << fast << "'" << std::endl;

  // Project name and config are not used yet.
  (void)projectName;
  (void)config;
  // Ninja does not have -i equivalent option yet.
  (void)ignoreErrors;
  // We do not handle fast build yet.
  (void)fast;

  std::string makeCommand =
    cmSystemTools::ConvertToUnixOutputPath(makeProgram);

  if(additionalOptions)
    {
    makeCommand += " ";
    makeCommand += additionalOptions;
    }
  if(targetName)
    {
    makeCommand += " ";
    makeCommand += targetName;
    }

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__
            << " RESULT='" << makeCommand << "'" << std::endl;
  return makeCommand;
}

//----------------------------------------------------------------------------
// Non-virtual public methods.

void cmGlobalNinjaGenerator::AddRule(const std::string& name,
                                     const std::string& command,
                                     const std::string& comment,
                                     const std::string& description,
                                     const std::string& depfile,
                                     const cmNinjaVars& variables)
{
  // Do not add twice the same rule.
  RulesSetType::const_iterator rule = this->Rules.find(name);
  if (rule != this->Rules.end())
    return;

  this->Rules.insert(name);
  cmGlobalNinjaGenerator::WriteRule(*this->RulesFileStream,
                                    name,
                                    command,
                                    comment,
                                    description,
                                    depfile,
                                    variables);
}


//----------------------------------------------------------------------------
// Virtual protected methods.

// Not implemented in UnixMakefile generator.
void cmGlobalNinjaGenerator::GetTargetSets(TargetDependSet& projectTargets,
                                           TargetDependSet& originalTargets,
                                           cmLocalGenerator* root,
                                           GeneratorVector const& generators)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;

  cmGlobalGenerator::GetTargetSets(projectTargets,
                                   originalTargets,
                                   root,
                                   generators);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
}

// Not implemented in UnixMakefile generator.
bool cmGlobalNinjaGenerator::IsRootOnlyTarget(cmTarget* target)
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "DEBUG NINJA: ARG: target: " << target
            << " name: '" << target->GetName() << "'"
            << " type: '" << cmTarget::TargetTypeNames(target->GetType()) << "'"
            << std::endl;

  bool ret = cmGlobalGenerator::IsRootOnlyTarget(target);

  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

// Not implemented in UnixMakefile generator.
bool cmGlobalNinjaGenerator::ComputeTargetDepends()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  bool ret = cmGlobalGenerator::ComputeTargetDepends();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

// Not implemented in UnixMakefile generator.
const char* cmGlobalNinjaGenerator::GetPredefinedTargetsFolder()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  const char* ret = cmGlobalGenerator::GetPredefinedTargetsFolder();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

// Not implemented in UnixMakefile generator.
bool cmGlobalNinjaGenerator::UseFolderProperty()
{
  std::cout << "DEBUG NINJA: BEGIN: " << __PRETTY_FUNCTION__ << std::endl;
  bool ret = cmGlobalGenerator::UseFolderProperty();
  std::cout << "DEBUG NINJA: END: " << __PRETTY_FUNCTION__ << std::endl;
  return ret;
}

//----------------------------------------------------------------------------
// Private methods

void cmGlobalNinjaGenerator::OpenBuildFileStream()
{
  // Compute Ninja's build file path.
  std::string buildFilePath =
    this->GetCMakeInstance()->GetHomeOutputDirectory();
  buildFilePath += "/";
  buildFilePath += cmGlobalNinjaGenerator::NINJA_BUILD_FILE;

  // Get a stream where to generate things.
  if (!this->BuildFileStream)
    {
    this->BuildFileStream = new cmGeneratedFileStream(buildFilePath.c_str());
    if (!this->BuildFileStream)
      {
      // An error message is generated by the constructor if it cannot
      // open the file.
      return;
      }
    }

  // Write the do not edit header.
  this->WriteDisclaimer(*this->BuildFileStream);

  // Write a comment about this file.
  *this->BuildFileStream
    << "# This file contains all the build statements describing the\n"
    << "# compilation DAG.\n\n"
    ;
}

void cmGlobalNinjaGenerator::CloseBuildFileStream()
{
  if (this->BuildFileStream)
    {
    delete this->BuildFileStream;
    this->BuildFileStream = 0;
    }
  else
    {
    // TODO(Nicolas Despres): Add the the name of the build filestream.
    cmSystemTools::Error("Build file stream was not open.");
   }
}

void cmGlobalNinjaGenerator::OpenRulesFileStream()
{
  // Compute Ninja's build file path.
  std::string rulesFilePath =
    this->GetCMakeInstance()->GetHomeOutputDirectory();
  rulesFilePath += "/";
  rulesFilePath += cmGlobalNinjaGenerator::NINJA_RULES_FILE;

  // Get a stream where to generate things.
  if (!this->RulesFileStream)
    {
    this->RulesFileStream = new cmGeneratedFileStream(rulesFilePath.c_str());
    if (!this->RulesFileStream)
      {
      // An error message is generated by the constructor if it cannot
      // open the file.
      return;
      }
    }

  // Write the do not edit header.
  this->WriteDisclaimer(*this->RulesFileStream);

  // Write comment about this file.
  *this->RulesFileStream
    << "# This file contains all the rules used to get the outputs files\n"
    << "# built from the input files.\n"
    << "# It is included in the main '" << NINJA_BUILD_FILE << "'.\n\n"
    ;
}

void cmGlobalNinjaGenerator::CloseRulesFileStream()
{
  if (this->RulesFileStream)
    {
    delete this->RulesFileStream;
    this->RulesFileStream = 0;
    }
  else
    {
    // TODO(Nicolas Despres): Add the the name of the filestream.
    cmSystemTools::Error("Rules file stream was not open.");
   }
}

void cmGlobalNinjaGenerator::WriteDisclaimer(std::ostream& os)
{
  os
    << "# CMAKE generated file: DO NOT EDIT!\n"
    << "# Generated by \"" << this->GetName() << "\""
    << " Generator, CMake Version "
    << cmVersion::GetMajorVersion() << "."
    << cmVersion::GetMinorVersion() << "\n\n";
}
