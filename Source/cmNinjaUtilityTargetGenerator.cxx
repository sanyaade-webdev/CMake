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

static void find_replace(std::string &str, const std::string &find,
                         const std::string &replace) {
  std::string::size_type pos = 0;
  while ((pos = str.find(find, pos)) != std::string::npos) {
    str.replace(pos, find.size(), replace);
    pos += replace.size();
  }
}

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
    this->GetLocalGenerator()->WriteCustomCommandRule();

    cmNinjaVars vars;
    vars["COMMAND"] = this->GetLocalGenerator()->BuildCommandLine(commands);
    const char *desc = this->GetTarget()->GetProperty("EchoString");
    if (desc)
      vars["DESC"] = desc;
    else
      vars["DESC"] = "Running utility command for " + this->GetTargetName();

    // TODO: fix problematic global targets.  For now, search and replace the
    // makefile vars.
    find_replace(vars["COMMAND"], "$(CMAKE_SOURCE_DIR)",
                 this->GetTarget()->GetMakefile()->GetHomeDirectory());
    find_replace(vars["COMMAND"], "$(CMAKE_BINARY_DIR)",
                 this->GetTarget()->GetMakefile()->GetHomeOutputDirectory());
    find_replace(vars["COMMAND"], "$(ARGS)", "");

    if (vars["COMMAND"].find('$') != std::string::npos)
      return;

    std::string utilCommandName = cmake::GetCMakeFilesDirectoryPostSlash();
    utilCommandName += this->GetTargetName() + ".util";

    cmGlobalNinjaGenerator::WriteBuild(this->GetBuildFileStream(),
                                       "Utility command for " + this->GetTargetName(),
                                       "CUSTOM_COMMAND",
                                       cmNinjaDeps(1, utilCommandName),
                                       deps,
                                       cmNinjaDeps(),
                                       cmNinjaDeps(),
                                       vars);

    cmGlobalNinjaGenerator::WritePhonyBuild(this->GetBuildFileStream(),
                                            "",
                                            cmNinjaDeps(1, this->GetTargetName()),
                                            cmNinjaDeps(1, utilCommandName),
                                            cmNinjaDeps(),
                                            cmNinjaDeps(),
                                            cmNinjaVars());
  }
}
