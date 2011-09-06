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

void cmGlobalNinjaGenerator::WriteDefault(std::ostream& os,
                                          const cmNinjaDeps& targets,
                                          const std::string& comment)
{
  cmGlobalNinjaGenerator::WriteComment(os, comment);
  os << "default";
  for(cmNinjaDeps::const_iterator i = targets.begin(); i != targets.end(); ++i)
    os << " " << *i;
  os << "\n";
}


cmGlobalNinjaGenerator::cmGlobalNinjaGenerator()
  : cmGlobalGenerator()
  , BuildFileStream(0)
  , RulesFileStream(0)
  , Rules()
  , AllDependencies()
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
  entry.Brief = "Generates build.ninja files (experimental).";
  entry.Full =
    "A build.ninja file is generated into the build tree. Any"
    "version of the ninja program can build the project through the "
    "\"all\" target.  An \"install\" target is also provided.";
}

// Implemented in all cmGlobaleGenerator sub-classes.
// Used in:
//   Source/cmLocalGenerator.cxx
//   Source/cmake.cxx
void cmGlobalNinjaGenerator::Generate()
{
  this->OpenBuildFileStream();
  this->OpenRulesFileStream();

  this->cmGlobalGenerator::Generate();

  this->WriteBuiltinTargets(*this->BuildFileStream);

  this->CloseRulesFileStream();
  this->CloseBuildFileStream();
}

// Implemented in all cmGlobaleGenerator sub-classes.
// Used in:
//   Source/cmMakefile.cxx:
void cmGlobalNinjaGenerator
::EnableLanguage(std::vector<std::string>const& languages,
                 cmMakefile *mf,
                 bool optional)
{
  this->cmGlobalGenerator::EnableLanguage(languages, mf, optional);
  std::string path;
  for(std::vector<std::string>::const_iterator l = languages.begin();
      l != languages.end(); ++l)
    {
    if(*l == "NONE")
      {
      continue;
      }
    if(*l != "C" && *l != "CXX")
      {
      std::string message = "The \"";
      message += this->GetName();
      message += "\" generator does not support the language \"";
      message += *l;
      message += "\" yet.";
      cmSystemTools::Error(message.c_str());
      }
    }
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
  if (this->HasRule(name))
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

bool cmGlobalNinjaGenerator::HasRule(const std::string &name) {
  RulesSetType::const_iterator rule = this->Rules.find(name);
  return (rule != this->Rules.end());
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

void cmGlobalNinjaGenerator::AddDependencyToAll(const std::string& dependency)
{
  this->AllDependencies.push_back(dependency);
}

void cmGlobalNinjaGenerator::WriteBuiltinTargets(std::ostream& os)
{
  // Write headers.
  cmGlobalNinjaGenerator::WriteDivider(os);
  os << "# Built-in targets\n\n";

  this->WriteTargetAll(os);
  this->WriteTargetRebuildManifest(os);
}

void cmGlobalNinjaGenerator::WriteTargetAll(std::ostream& os)
{
  cmNinjaDeps emptyDeps;
  cmNinjaVars emptyVars;

  cmNinjaDeps outputs;
  outputs.push_back("all");

  cmGlobalNinjaGenerator::WritePhonyBuild(os,
                                          "The main all target.",
                                          outputs,
                                          this->AllDependencies,
                                          emptyDeps,
                                          emptyDeps,
                                          emptyVars);

  cmGlobalNinjaGenerator::WriteDefault(os,
                                       outputs,
                                       "Make the all target the default.");
}

void cmGlobalNinjaGenerator::WriteTargetRebuildManifest(std::ostream& os)
{
  cmMakefile* mfRoot = this->LocalGenerators[0]->GetMakefile();

  std::ostringstream cmd;
  cmd << mfRoot->GetRequiredDefinition("CMAKE_COMMAND")
      << " -H" << mfRoot->GetHomeDirectory()
      << " -B" << mfRoot->GetHomeOutputDirectory();
  WriteRule(*this->RulesFileStream,
            "RERUN_CMAKE",
            cmd.str(),
            "Rule for re-running cmake.",
            "Re-running CMake...",
            "",
            cmNinjaVars());

  cmNinjaDeps implicitDeps;
  for (std::vector<cmLocalGenerator *>::const_iterator i =
       this->LocalGenerators.begin(); i != this->LocalGenerators.end(); ++i) {
    const std::vector<std::string>& lf = (*i)->GetMakefile()->GetListFiles();
    implicitDeps.insert(implicitDeps.end(), lf.begin(), lf.end());
  }
  std::sort(implicitDeps.begin(), implicitDeps.end());
  implicitDeps.erase(std::unique(implicitDeps.begin(), implicitDeps.end()),
                     implicitDeps.end());
  implicitDeps.push_back("CMakeCache.txt");

  WriteBuild(os,
             "Re-run CMake if any of its inputs changed.",
             "RERUN_CMAKE",
             /*outputs=*/ cmNinjaDeps(1, "build.ninja"),
             /*explicitDeps=*/ cmNinjaDeps(),
             implicitDeps,
             /*orderOnlyDeps=*/ cmNinjaDeps(),
             /*variables=*/ cmNinjaVars());

  WritePhonyBuild(os,
                  "A missing CMake input file is not an error.",
                  implicitDeps,
                  cmNinjaDeps(),
                  cmNinjaDeps(),
                  cmNinjaDeps(),
                  cmNinjaVars());
}
