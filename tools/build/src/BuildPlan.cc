/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "BuildPlan.h"
#include "BuildMap.h"
#include "DependencyCache.h"
#include "ConfigureShell.h"
#include "GnuToolChain.h"
#include "JobScheduler.h"
#include "RecipeProtocol.h"
#include <cc/meta/yason>
#include <cc/meta/JsonWriter>
#include <cc/glob/Glob>
#include <cc/Process>
#include <cc/Crc32Sink>
#include <cc/File>
#include <cc/Dir>
#include <cc/DirWalker>
#include <cc/ResourceGuard>
#include <cc/Arguments>
#include <cc/stdio>

namespace ccbuild {

Ref<BuildPlan> BuildPlan::create(int argc, char **argv)
{
    return new BuildPlan{argc, argv};
}

Ref<BuildPlan> BuildPlan::create(const String &projectPath)
{
    Ref<BuildPlan> plan;
    if (BuildMap::instance()->lookupPlan(projectPath->absolutePathRelativeTo(Process::getWorkingDirectory()), &plan)) return plan;
    return new BuildPlan{projectPath, this};
}

#define CCBUILD_BUILDPLAN_COMPONENTS_INIT \
    shell_{BuildShell::create(this)}, \
    preparationStage_{this}, \
    configureStage_{this}, \
    analyseStage_{this}, \
    preprocessStage_{this}, \
    compileLinkStage_{this}, \
    testRunStage_{this}, \
    installStage_{this}, \
    uninstallStage_{this}, \
    cleanStage_{this}

BuildPlan::BuildPlan(int argc, char **argv):
    projectPath_{"."},
    concurrency_{-1},
    disabled_{false},
    CCBUILD_BUILDPLAN_COMPONENTS_INIT
{
    Arguments arguments{argc, argv};
    StringList items = arguments->items();

    if (items->count() > 0) {
        if (items->count() > 1)
            throw UsageError{"Handling multiple source directories at once is not supported"};
        projectPath_ = items->at(0);
    }

    projectPath_ = projectPath_->absolutePathRelativeTo(Process::getWorkingDirectory())->canonicalPath();
    recipePath_ = recipePath(projectPath_);

    ResourceGuard context(recipePath_);
    recipe_ = yason::parse(File{recipePath_}->map(), RecipeProtocol::instance());
    arguments->validate(recipe_);
    arguments->override(recipe_);

    readRecipe();
    Process::setEnv("PREFIX", installPrefix_);

    toolChain_ = GnuToolChain::create(this);
    if (optimize_ == "") optimize_ = toolChain_->defaultOptimization(this);

    BuildMap::instance()->insertPlan(projectPath_, this);
    scope_ = projectPath_;
}

BuildPlan::BuildPlan(const String &projectPath, BuildPlan *parentPlan):
    toolChain_{parentPlan->toolChain_},
    projectPath_{projectPath},
    recipePath_{recipePath(projectPath)},
    scope_{parentPlan->scope_},
    concurrency_{parentPlan->concurrency_},
    disabled_{false},
    CCBUILD_BUILDPLAN_COMPONENTS_INIT
{
    ResourceGuard context(recipePath_);
    recipe_ = yason::parse(File{recipePath_}->map(), RecipeProtocol::instance());
    readRecipe(parentPlan);
    BuildMap::instance()->insertPlan(projectPath_, this);
}

bool BuildPlan::goForBuild() const
{
    return (options_ & Package) || configureStage_.success();
}

void BuildPlan::readRecipe(BuildPlan *parentPlan)
{
    options_ = 0;
    if (recipe_->className() == "Application")  options_ |= Application;
    else if (recipe_->className() == "Library") options_ |= Library;
    else if (recipe_->className() == "Plugin")  options_ |= Library | Plugin;
    else if (recipe_->className() == "Test")    options_ |= Application | Test;
    else if (recipe_->className() == "Tools")   options_ |= Tools;
    else if (recipe_->className() == "Tests")   options_ |= Tools | Test;
    else if (recipe_->className() == "Package") options_ |= Package;

    name_ = recipe_->value("name");
    alias_ = recipe_->value("alias");
    version_ = recipe_->value("version");
    installRoot_ = recipe_->value("root");
    installPrefix_ = recipe_->value("prefix");
    if (installPrefix_ == "") {
        installPrefix_ = (installRoot_ == "/") ? "/usr/local" : "/usr";
    }
    testArgs_ = recipe_->value("test-args");

    checkDuplicateTargetNames();

    if (recipe_->value("debug"))       options_ |= Debug;
    if (recipe_->value("release"))     options_ |= Release;
    if (recipe_->value("simulate"))    options_ |= Simulate;
    if (recipe_->value("blindfold"))   options_ |= Blindfold;
    if (recipe_->value("concatenate")) options_ |= Blindfold;
    if (recipe_->value("bootstrap"))   options_ |= Bootstrap | Simulate | Blindfold;
    if (recipe_->value("test"))        options_ |= BuildTests;
    if (recipe_->value("test-run"))    options_ |= BuildTests;
    if (recipe_->value("test-report")) options_ |= BuildTests;
    if (recipe_->value("clean"))       options_ |= BuildTests;
    if (recipe_->value("verbose"))     options_ |= Verbose;
    if (recipe_->value("configure"))   options_ |= Configure;
    else if (recipe_->value("clean"))  options_ |= Clean;

    concurrency_ = recipe_->value("jobs");
    testRunConcurrency_ = recipe_->value("test-run-jobs");

    if (parentPlan) {
        options_ &= ~GlobalOptions;
        options_ |= parentPlan->options() & GlobalOptions;
        concurrency_ = parentPlan->concurrency_;
        if (parentPlan->testRunConcurrency_ > 0 && testRunConcurrency_ < 0)
            testRunConcurrency_ = parentPlan->testRunConcurrency_;
        installRoot_ = parentPlan->installRoot_;
        installPrefix_ = parentPlan->installPrefix_;
        testArgs_ = parentPlan->testArgs_;
    }
    else {
        ConfigureShell::instance()->setVerbose(options_ & Verbose);
    }

    if (concurrency_ > 0 && testRunConcurrency_ < 0)
        testRunConcurrency_ = concurrency_;

    BuildParameters::read(recipe_, this);

    String defaultIncludePath = projectPath_->extendPath("include");
    if (Dir::exists(defaultIncludePath)) {
        if (!includePaths_->contains(defaultIncludePath))
            includePaths_->append(defaultIncludePath);
    }

    StringList dependsList = Variant::cast<StringList>(recipe_->value("depends"));
    if (dependsList->count() > 0) {
        for (String item: dependsList) {
            String name;
            Version versionMin, versionMax;
            if (item->contains(">=")) {
                StringList parts = item->split(">=");
                name = parts->at(0)->trim();
                if (parts->has(1)) versionMin = Version{parts->at(1)->trim()};
            }
            else if (item->contains("<=")) {
                StringList parts = item->split("<=");
                name = parts->at(0)->trim();
                if (parts->has(1)) versionMax = Version{parts->at(1)->trim()};
            }
            else {
                name = item;
            }

            SystemPrerequisitesByName::Iterator it;
            if (!systemPrerequisitesByName_->find(name, &it))
                systemPrerequisitesByName_->insert(name, SystemPrerequisiteList{}, &it);

            Ref<SystemPrerequisite> prerequisite = SystemPrerequisite::create(name);
            prerequisite->setVersionMin(versionMin);
            prerequisite->setVersionMax(versionMax);
            it->value()->append(prerequisite);
        }
    }

    if (recipe_->hasChildren()) {
        for (const MetaObject *object: recipe_->children()) {
            if (object->className() == "Dependency") {
                Ref<SystemPrerequisite> p = SystemPrerequisite::read(object, this);
                SystemPrerequisitesByName::Iterator it;
                if (!systemPrerequisitesByName_->find(p->name(), &it))
                    systemPrerequisitesByName_->insert(p->name(), SystemPrerequisiteList{}, &it);
                else
                    throw UsageError{Format{"%%: Ambiguous system dependency '%%'"} << recipePath_ << p->name()};
                it->value()->append(p);
            }
            else if (object->className() == "Predicate") {
                readPredicate(object);
            }
            else if (object->className() == "PreBuild") {
                compileLinkStage_.preCommands()->append(String(object->value("execute")));
            }
            else if (object->className() == "PostBuild") {
                compileLinkStage_.postCommands()->append(String(object->value("execute")));
            }
            else if (object->className() == "PreClean") {
                cleanStage_.preCommands()->append(String(object->value("execute")));
            }
            else if (object->className() == "PostClean") {
                cleanStage_.postCommands()->append(String(object->value("execute")));
            }
            else if (object->className() == "PreInstall") {
                installStage_.preCommands()->append(String(object->value("execute")));
            }
            else if (object->className() == "PostInstall") {
                installStage_.postCommands()->append(String(object->value("execute")));
            }
            else if (object->className() == "PreUninstall") {
                uninstallStage_.preCommands()->append(String(object->value("execute")));
            }
            else if (object->className() == "PostUninstall") {
                uninstallStage_.postCommands()->append(String(object->value("execute")));
            }
        }
    }

    if (parentPlan) {
        optimize_ = parentPlan->optimize();
        linkStatic_ = parentPlan->linkStatic();
    }
}

void BuildPlan::readPredicate(const MetaObject *object)
{
    predicates_->append(Predicate::read(object));
    for (const MetaObject *child: object->children())
        readPredicate(child);
}

void BuildPlan::checkDuplicateTargetNames()
{
    if (name_ == "") return;

    String otherRecipePath;
    bool ok = true;
    if (options_ & Library)
        ok = BuildMap::instance()->registerLibrary(name_, recipePath_, &otherRecipePath);
    else if (options_ & Application)
        ok = BuildMap::instance()->registerApplication(name_, recipePath_, &otherRecipePath);
    if (!ok) {
        throw UsageError{
            Format{"Duplicate target name '%%' in\n  %%\n  and\n  %%"}
            << name_
            << otherRecipePath
            << recipePath_
        };
    }
}

void BuildPlan::gatherAutoConfigureSystemPrerequisites(Set<String> &names)
{
    if (configureListComplete_) return;
    configureListComplete_ = true;

    if (systemPrerequisitesByName_) {
        for (auto e: systemPrerequisitesByName_) {
            if (e->value()) {
                for (auto p: e->value()) {
                    if (p->autoConfigure() && p->origName() != "")
                        names->insert(p->origName());
                }
            }
        }
    }

    for (auto plan: prerequisites_)
        plan->gatherAutoConfigureSystemPrerequisites(names);
}

int BuildPlan::run()
{
    readPrerequisites();

    if (recipe_->value("configure-list")) {
        Set<String> names;
        gatherAutoConfigureSystemPrerequisites(names);
        for (auto name: names)
            fout() << name << nl;
        return 0;
    }

    Process::setEnv("SOURCE", projectPath_);

    initModules();

    configureStage()->run();
    if (String(recipe_->value("query")) != "") {
        queryVariables(Variant::cast<String>(recipe_->value("query"))->split(','));
        return 0;
    }
    else if (recipe_->value("query-all")) {
        queryVariables(queryableVariableNames());
        return 0;
    }
    if (options_ & Configure)
        return configureStage()->success() ? 0 : 1;

    if (options_ & Bootstrap) {
        fout(
            "#!/bin/sh -ex\n"
            "SOURCE=$1\n"
            "MACHINE=$(%%)\n"
        ) << toolChain_->machineCommand();
    }

    if (configureStage()->success()) {
        if (!preparationStage()->run()) return 1;
    }
    if (recipe_->value("prepare")) return 0;

    String defaultIncludePath = projectPath_->extendPath("include");
    if (Dir::exists(defaultIncludePath)) {
        if (!includePaths_->contains(defaultIncludePath))
            includePaths_->append(defaultIncludePath);
    }

    globSources();

    if (recipe_->value("pkg-config")) {
        if (options_ & Package) {
            for (const BuildPlan *prerequisite: prerequisites_) {
                if (prerequisite->options_ & Library) {
                    fout() << toolChain()->pkgConfig(prerequisite);
                    break;
                }
            }
        }
        else if (options_ & Library) {
            fout() << toolChain()->pkgConfig(this);
        }
        return 0;
    }

    if (!analyseStage()->run()) return 1;

    if (recipe_->value("preprocess")) return preprocessStage()->run();

    if (options_ & Clean) return !cleanStage()->run();
    if (recipe_->value("uninstall")) return !uninstallStage()->run();

    if (!compileLinkStage()->run()) return 1;

    bool testRun = recipe_->value("test-run");
    bool testReport = recipe_->value("test-report");
    if (testRun || testReport) {
        if (testRun) Process::setEnv("TEST_RUN", "1");
        if (testReport) Process::setEnv("TEST_REPORT", "1");
        if (!testRunStage()->run())
            return testRunStage()->status();
    }

    if (recipe_->value("install")) {
        if (!installStage()->run()) return 1;
        if (installStage()->linkerCacheDirty()) {
            if (!toolChain()->refreshLinkerCache(this))
                return 1;
        }
    }

    return 0;
}

String BuildPlan::description() const
{
    return recipe_->value("description");
}

String BuildPlan::sourcePath(const String &source) const
{
    if (projectPath_ == ".") return source;
    return projectPath_->extendPath(source);
}

String BuildPlan::prestagePath(const String &precursor) const
{
    return prestagePath_->extendPath(precursor);
}

String BuildPlan::modulePath(const String &object) const
{
    return modulePath_->extendPath(object);
}

String BuildPlan::installPath(const String &relativeInstallPath) const
{
    return installRoot_->extendPath(installPrefix_)->extendPath(relativeInstallPath);
}

String BuildPlan::pluginPath(const String &targetLibName) const
{
    String path = targetLibName->extendPath("plugins");
    String group = recipe_->value("group");
    if (group != "") path = path->extendPath(group);
    return path;
}

String BuildPlan::pluginReversePath() const
{
    String group = recipe_->value("group");
    return group != "" ? "../../.." : "../..";
}

String BuildPlan::previousLinkCommandPath() const
{
    return modulePath("LinkCommand");
}

StringList BuildPlan::globSources(const StringList &patternList) const
{
    StringList sources;
    for (const String &pattern: patternList) {
        Glob glob{sourcePath(pattern)};
        for (String path; glob->read(&path);) {
            try {
                DirWalker walker{path};
                bool isDir = false;
                for (String path; walker->read(&path, &isDir);) {
                    if (!isDir) sources->append(path);
                }
            }
            catch (...) {
                sources->append(path);
            }
        }
    }
    for (const String &pattern: patternList) {
        Glob glob{prestagePath(pattern)};
        for (String path; glob->read(&path);) {
            try {
                DirWalker walker{path};
                bool isDir = false;
                for (String path; walker->read(&path, &isDir);) {
                    if (!isDir)
                        sources->append(path);
                }
            }
            catch (...) {
                if (path->startsWith("./")) // \todo FIXME, why?!
                    path = path->copy(2, path->count());
                sources->append(path);
            }
        }
    }

    return sources;
}

void BuildPlan::registerLinkDerivative(Job *linkJob)
{
    for (BuildPlan *prerequisite: prerequisites()) {
        if (prerequisite->options() & Package) {
            for (BuildPlan *child: prerequisite->prerequisites()) {
                if (child->options() & Library) {
                    if (child->libraryLinkJob_)
                        child->libraryLinkJob_->registerDerivative(linkJob);
                }
            }
        }
        else if (prerequisite->options() & Library) {
            if (prerequisite->libraryLinkJob_)
                prerequisite->libraryLinkJob_->registerDerivative(linkJob);
        }
    }
}

String BuildPlan::findPrerequisite(const String &prerequisitePath) const
{
    if (prerequisitePath->isAbsolutePath()) {
        if (File::exists(recipePath(prerequisitePath))) return prerequisitePath;
        return String{};
    }
    {
        String candidatePath = systemSourcePath()->extendPath(prerequisitePath);
        if (File::exists(recipePath(candidatePath))) return candidatePath;
    }
    for (String path = projectPath_; path != "/"; path = path->reducePath()) {
        String candidatePath = path->extendPath(prerequisitePath);
        if (File::exists(recipePath(candidatePath))) {
            candidatePath = candidatePath->canonicalPath();
            if (candidatePath == projectPath_) continue;
            return candidatePath;
        }
    }
    return String{};
}

void BuildPlan::readPrerequisites()
{
    if (prerequisitesRead_) return;
    prerequisitesRead_ = true;

    if ((options_ & Test) && !(options_ & BuildTests)) return;

    StringList prerequisitePaths = Variant::cast<StringList>(recipe_->value("use"));

    if (options_ & Package) {
        StringList packageItems = Variant::cast<StringList>(recipe_->value("include"));
        if (packageItems->count() > 0) prerequisitePaths = packageItems;
    }

    for (const String &prerequisitePath: prerequisitePaths) {
        String path = findPrerequisite(prerequisitePath);
        if (path == "")
            throw UsageError{Format{} << recipePath() << ": Failed to locate prerequisite '" << prerequisitePath << "'"};
        Ref<BuildPlan> plan = BuildPlan::create(path);
        plan->readPrerequisites();
        prerequisites_->append(plan);
    }

    if (options_ & Plugin) {
        String extensionTargetPath = recipe_->value("extend");
        if (extensionTargetPath == "")
            throw UsageError{Format{} << recipePath() << ": Please provide the path of a library to extend in Plugin.extend"};
        String path = findPrerequisite(extensionTargetPath);
        if (path == "")
            throw UsageError{Format{} << recipePath() << ": Failed to locate library '" << extensionTargetPath << "'"};
        extensionTarget_ = BuildPlan::create(path);
        if (extensionTarget_->options_ & Package) {
            extensionTarget_->readPrerequisites();
            for (BuildPlan *prerequisite: extensionTarget_->prerequisites_) {
                if (prerequisite->options_ & Library)
                    extensionTarget_ = prerequisite;
            }
        }
        if (!(extensionTarget_->options_ & Library))
            throw UsageError{Format{} << recipePath() << ": '" << extensionTargetPath << "' (Plugin.extend) does not point to a library"};
        extensionTarget_->readPrerequisites();
        prerequisites_->appendList(extensionTarget_->prerequisites_);
        prerequisites_->append(extensionTarget_);
    }

    findVersion();
}

void BuildPlan::findVersion()
{
    if (!version_->isValid()) {
        String path = findPrerequisite("..");
        if (path != "") {
            Ref<BuildPlan> plan = BuildPlan::create(path);
            if (!plan->version_->isValid()) plan->findVersion();
            if (plan->version_->isValid()) version_ = plan->version_;
        }
    }
    if (!version_->isValid())
        version_ = Version(0, 1, 0);
}

void BuildPlan::globSources()
{
    if (sources_->count() > 0) return;

    if ((options_ & Test) && !(options_ & BuildTests)) return;

    if (recipe_->contains("source"))
        sources_ = globSources(Variant::cast<StringList>(recipe_->value("source")));

    sourcePrefix_ = BuildMap::instance()->commonPrefix();
    if (sourcePrefix_ == "") sourcePrefix_ = projectPath_;
    else sourcePrefix_ = sourcePrefix_->canonicalPath();

    containsCPlusPlus_ = false;
    for (String source: sources_) {
        String suffix = source->fileSuffix();
        if (suffix == "cc" || suffix == "cpp" || suffix == "cxx" || suffix == "mm") {
            containsCPlusPlus_ = true;
            break;
        }
    }

    if (recipe_->contains("bundle"))
        bundle_ = globSources(Variant::cast<StringList>(recipe_->value("bundle")));

    for (BuildPlan *plan: prerequisites_)
        plan->globSources();
}

void BuildPlan::initModules()
{
    if (modulesInitialized_) return;

    if ((options_ & Test) && !(options_ & BuildTests)) return;

    modulesInitialized_ = true;

    String suffix;
    {
        Format f;
        String absoulteProjectPath = projectPath_->absolutePathRelativeTo(Process::getWorkingDirectory());
        {
            Format h;
            String topLevel = sourcePrefix_->absolutePathRelativeTo(Process::getWorkingDirectory());
            for (
                String path = absoulteProjectPath;
                path != topLevel && path != "/" && path != toolChain_->systemRoot();
                path = path->reducePath()
            )
                h << path->fileName();
            h << topLevel->fileName();
            f << hex(crc32(h->reverse()->join("/")), 8);
        }
        if (options_ & Bootstrap)
            f << "$MACHINE";
        else
            f << toolChain_->machine();
        f << absoulteProjectPath->reducePath()->fileName() + "_" + absoulteProjectPath->fileName();
        suffix = f->join("-");
    }
    modulePath_ = ".modules-" + suffix;
    configPath_ = ".config-" + suffix;
    prestagePath_ = ".prestage-" + suffix;

    for (BuildPlan *prerequisite: prerequisites_)
        prerequisite->initModules();
}

StringList BuildPlan::queryableVariableNames()
{
    return StringList {
        "name",
        "version",
        "source",
        "include-paths",
        "library-paths",
        "libraries",
        "custom-compile-flags",
        "custom-link-flags"
    };
}

void BuildPlan::queryVariables(const StringList &names) const
{
    Ref<MetaObject> variables = MetaObject::create();
    variables->insert("name", name_);
    variables->insert("version", version_);
    variables->insert("source", recipe_->value("source"));
    variables->insert("include-paths", includePaths_);
    variables->insert("library-paths", libraryPaths_);
    variables->insert("libraries", libraries_);
    variables->insert("custom-compile-flags", customCompileFlags_);
    variables->insert("custom-link-flags", customLinkFlags_);

    JsonWriter::create(stdOut())->writeObject(variables, names);
    stdOut()->write("\n");
}

} // namespace ccbuild
