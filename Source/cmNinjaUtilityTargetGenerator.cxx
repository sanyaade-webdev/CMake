#include "cmNinjaUtilityTargetGenerator.h"
#include "cmCustomCommand.h"
#include "cmGeneratedFileStream.h"
#include "cmGlobalNinjaGenerator.h"
#include "cmMakefile.h"
#include "cmSourceFile.h"
#include "cmTarget.h"

cmNinjaUtilityTargetGenerator::cmNinjaUtilityTargetGenerator(cmTarget *target)
  : cmNinjaTargetGenerator(target) {}

cmNinjaUtilityTargetGenerator::~cmNinjaUtilityTargetGenerator() {}

void cmNinjaUtilityTargetGenerator::Generate() {
  std::vector<std::string> commands;
  cmNinjaDeps deps;

  const std::vector<cmCustomCommand> *cmdLists[2] = {
    &this->GetTarget()->GetPreBuildCommands(),
    &this->GetTarget()->GetPostBuildCommands()
  };

  for (unsigned i = 0; i != 2; ++i) {
    for (std::vector<cmCustomCommand>::const_iterator ci = cmdLists[i]->begin();
         ci != cmdLists[i]->end(); ++ci) {
      this->GetLocalGenerator()->AppendCustomCommandDeps(&*ci, deps);
      this->GetLocalGenerator()->AppendCustomCommandLines(&*ci, commands);
    }
  }

  const std::vector<cmSourceFile*>& sources =
    this->GetTarget()->GetSourceFiles();
  for(std::vector<cmSourceFile*>::const_iterator source = sources.begin();
      source != sources.end(); ++source)
    {
    if(cmCustomCommand* cc = (*source)->GetCustomCommand())
      {
      this->GetLocalGenerator()->AddCustomCommandTarget(cc, this->GetTarget());

      // Depend on all custom command outputs.
      const std::vector<std::string>& outputs = cc->GetOutputs();
      std::transform(outputs.begin(), outputs.end(),
                     std::back_inserter(deps), MapToNinjaPath());
      }
    }

  std::set<cmStdString> const& utils = this->GetTarget()->GetUtilities();
  deps.insert(deps.end(), utils.begin(), utils.end());

  if (commands.empty()) {
    cmGlobalNinjaGenerator::WritePhonyBuild(this->GetBuildFileStream(),
                                            "Utility command for " + this->GetTargetName(),
                                            cmNinjaDeps(1, this->GetTargetName()),
                                            deps,
                                            cmNinjaDeps(),
                                            cmNinjaDeps(),
                                            cmNinjaVars());
  } else {
    std::string command = this->GetLocalGenerator()->BuildCommandLine(commands);
    const char *echoStr = this->GetTarget()->GetProperty("EchoString");
    std::string desc;
    if (echoStr)
      desc = echoStr;
    else
      desc = "Running utility command for " + this->GetTargetName();

    // TODO: fix problematic global targets.  For now, search and replace the
    // makefile vars.
    cmSystemTools::ReplaceString(command, "$(CMAKE_SOURCE_DIR)",
                          this->GetTarget()->GetMakefile()->GetHomeDirectory());
    cmSystemTools::ReplaceString(command, "$(CMAKE_BINARY_DIR)",
                    this->GetTarget()->GetMakefile()->GetHomeOutputDirectory());
    cmSystemTools::ReplaceString(command, "$(ARGS)", "");

    if (command.find('$') != std::string::npos)
      return;

    std::string utilCommandName = cmake::GetCMakeFilesDirectoryPostSlash();
    utilCommandName += this->GetTargetName() + ".util";

    this->GetGlobalGenerator()->WriteCustomCommandBuild(
      command,
      desc,
      "Utility command for " + this->GetTargetName(),
      cmNinjaDeps(1, utilCommandName),
      deps);

    cmGlobalNinjaGenerator::WritePhonyBuild(this->GetBuildFileStream(),
                                            "",
                                            cmNinjaDeps(1, this->GetTargetName()),
                                            cmNinjaDeps(1, utilCommandName),
                                            cmNinjaDeps(),
                                            cmNinjaDeps(),
                                            cmNinjaVars());
  }
}
