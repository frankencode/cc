/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXMAKE_BUILDPLAN_H
#define FLUXMAKE_BUILDPLAN_H

#include <flux/PrefixTree.h>
#include <flux/Map.h>
#include "Module.h"
#include "BuildParameters.h"
#include "BuildShell.h"
#include "ToolChain.h"
#include "SystemPrerequisite.h"
#include "ConfigureStage.h"
#include "AnalyseStage.h"
#include "CompileLinkStage.h"
#include "TestRunStage.h"
#include "InstallStage.h"
#include "UninstallStage.h"
#include "CleanStage.h"

namespace flux {
class FileStatus;
class Config;
} // namespace flux

namespace fluxmake
{

using namespace flux;

class BuildPlan;
typedef List< Ref<BuildPlan> > BuildPlanList;

class BuildPlan: public BuildParameters
{
public:
	enum Option {
		Application   = 1 << 0,
		Library       = 1 << 1,
		Tools         = 1 << 2,
		Tests         = 1 << 3,
		Package       = 1 << 4,
		Debug         = 1 << 5,
		Release       = 1 << 6,
		Simulate      = 1 << 7,
		Blindfold     = 1 << 8,
		Bootstrap     = 1 << 9,
		BuildTests    = 1 << 10,
		Verbose       = 1 << 11,
		Unspecified   = -1,
		GlobalOptions = Debug|
		                Release|
		                Simulate|
		                Blindfold|
		                Bootstrap|
		                BuildTests|
		                Verbose
	};

	static Ref<BuildPlan> create(int argc, char **argv);

	inline ToolChain *toolChain() const { return toolChain_; }

	inline String projectPath() const { return projectPath_; }
	inline String modulePath() const { return modulePath_; }
	inline Config *recipe() const { return recipe_; }

	inline String name() const { return name_; }
	inline StringList *alias() const { return alias_; }
	inline String version() const { return version_; }
	inline int options() const { return options_; }

	inline StringList *sources() const { return sources_; }
	inline ModuleList *modules() const { return modules_; }
	inline bool containsCPlusPlus() const { return containsCPlusPlus_; }

	inline int concurrency() const { return concurrency_; }

	inline BuildPlanList *prerequisites() const { return prerequisites_; }
	inline SystemPrerequisitesByName *systemPrerequisitesByName() const { return systemPrerequisitesByName_; }

	int run();

	inline String sourcePrefix() const { return sourcePrefix_; }

	String sourcePath(String source) const;
	String modulePath(String object) const;
	String installPath(String relativeInstallPath) const;

	inline BuildShell *shell() { return &shell_; }

	inline ConfigureStage *configureStage() { return &configureStage_; }
	inline AnalyseStage *analyseStage() { return &analyseStage_; }
	inline CompileLinkStage *compileLinkStage() { return &compileLinkStage_; }
	inline TestRunStage *testRunStage() { return &testRunStage_; }
	inline InstallStage *installStage() { return &installStage_; }
	inline UninstallStage *uninstallStage() { return &uninstallStage_; }
	inline CleanStage *cleanStage() { return &cleanStage_; }

private:
	Ref<BuildPlan> create(String projectPath);

	typedef PrefixTree<char, BuildPlan *> BuildMap;

	BuildPlan(int argc, char **argv);
	BuildPlan(String projectPath, BuildPlan *parentPlan);

	void readRecipe(BuildPlan *parentPlan = 0);

	void readPrerequisites();
	void globSources();
	void initModules();

	Ref<ToolChain> toolChain_;

	String projectPath_;
	String modulePath_;
	Ref<Config> recipe_;

	String name_;
	Ref<StringList> alias_;
	String version_;
	int options_;

	Ref<StringList> sources_;
	Ref<ModuleList> modules_;
	bool containsCPlusPlus_;

	int concurrency_;

	Ref<SystemPrerequisitesByName> systemPrerequisitesByName_;
	Ref<BuildPlanList> prerequisites_;

	Ref<BuildMap> buildMap_;
	String sourcePrefix_;
	String installPrefix_;

	BuildShell shell_;

	ConfigureStage configureStage_;
	AnalyseStage analyseStage_;
	CompileLinkStage compileLinkStage_;
	TestRunStage testRunStage_;
	InstallStage installStage_;
	UninstallStage uninstallStage_;
	CleanStage cleanStage_;
};

} // namespace fluxmake

#endif // FLUXMAKE_BUILDPLAN_H
