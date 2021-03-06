/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include "RecipeProtocol.h"

namespace ccbuild {

class BuildParametersPrototype: public MetaPrototype
{
public:
    static Ref<BuildParametersPrototype> create(const String &className) {
        return new BuildParametersPrototype{className};
    }

protected:
    BuildParametersPrototype(const String &className, const MetaProtocol *protocol = nullptr):
        MetaPrototype{className, protocol}
    {
        insert("compiler", "");
        insert("optimize", "");
        insert("static", false);

        insert("include-paths", StringList{});
        insert("library-paths", StringList{});
        insert("link", StringList{});

        insert("compile-flags", StringList{});
        insert("link-flags", StringList{});
    }
};

class SpecificBuildParametersPrototype: public BuildParametersPrototype
{
public:
    static Ref<SpecificBuildParametersPrototype> create(const String &className) {
        return new SpecificBuildParametersPrototype{className};
    }

protected:
    static Ref<MetaProtocol> createProtocol(MetaProtocol *protocol) {
        Ref<MetaProtocol> newProtocol;
        if (!protocol) protocol = newProtocol = MetaProtocol::create();
        const char *paramClasses[] = {
            "Debug", "Release",
            "Linux", "OpenBSD", "FreeBSD", "NetBSD", "DragonFly",
            "MacOSX", "Solaris", "Mach", "HPUX"
        };
        for (int i = 0, n = sizeof(paramClasses) / sizeof(paramClasses[0]); i < n; ++i)
            protocol->define(BuildParametersPrototype::create(paramClasses[i]));
        return protocol;
    }

    SpecificBuildParametersPrototype(const String &className, MetaProtocol *protocol = nullptr):
        BuildParametersPrototype{className, createProtocol(protocol)}
    {}
};

class UserStop: public MetaPrototype
{
public:
    static Ref<UserStop> create(const String &className) {
        return new UserStop{className};
    }

protected:
    UserStop(const String &className):
        MetaPrototype{className}
    {
        insert("execute", "");
    }
};

class BuildOptionsPrototype: public SpecificBuildParametersPrototype
{
protected:
    static Ref<MetaProtocol> createProtocol(MetaProtocol *protocol) {
        Ref<MetaProtocol> newProtocol;
        if (!protocol) protocol = newProtocol = MetaProtocol::create();
        const char *stopClasses[] = {
            "PreBuild",     "PostBuild",
            "PreClean",     "PostClean",
            "PreInstall",   "PostInstall",
            "PreUninstall", "PostUninstall",
        };
        for (int i = 0, n = sizeof(stopClasses) / sizeof(stopClasses[0]); i < n; ++i)
            protocol->define(UserStop::create(stopClasses[i]));
        return protocol;
    }

    BuildOptionsPrototype(const String &className, MetaProtocol *protocol = nullptr):
        SpecificBuildParametersPrototype{className, createProtocol(protocol)}
    {
        insert("use", StringList{});
        insert("root", "/");
        insert("prefix", "");

        insert("debug", false);
        insert("release", false);

        insert("configure", false);
        insert("configure-list", false);
        insert("prepare", false);
        insert("preprocess", false);
        insert("clean", false);
        insert("install", false);
        insert("uninstall", false);
        insert("test", false);
        insert("test-run", false);
        insert("test-report", false);
        insert("test-args", "");

        insert("verbose", false);
        insert("jobs", -1);
        insert("test-run-jobs", -1);

        insert("simulate", false);
        insert("blindfold", false);
        insert("bootstrap", false);

        insert("query", "");
        insert("query-all", false);

        insert("pkg-config", false);
    }
};

class SystemPrerequisitePrototype: public SpecificBuildParametersPrototype
{
public:
    static Ref<SystemPrerequisitePrototype> create() {
        return new SystemPrerequisitePrototype{"Dependency"};
    }

protected:
    SystemPrerequisitePrototype(const String &className):
        SpecificBuildParametersPrototype{className}
    {
        insert("name", "");
        insert("optional", false);
        insert("cascade", false);

        insert("compile-flags-configure", "");
        insert("link-flags-configure", "");

        insert("version-configure", "");
        insert("version-min", Version());
        insert("version-max", Version());

        insert("probe", "");
        insert("configure", "");
    }
};

class PredicatePrototype;

class PredicateMetaProtocol: public MetaProtocol
{
public:
    static Ref<PredicateMetaProtocol> create(PredicatePrototype *prototype) {
        return new PredicateMetaProtocol{prototype};
    }

    const MetaPrototype *lookup(const String &className) const override;

private:
    PredicateMetaProtocol(PredicatePrototype *prototype):
        prototype_{prototype}
    {}

    PredicatePrototype *prototype_;
};

class PredicatePrototype: public MetaPrototype
{
public:
    static Ref<PredicatePrototype> create() {
        return new PredicatePrototype{"Predicate"};
    }

protected:
    PredicatePrototype(const String &className):
        MetaPrototype{className, PredicateMetaProtocol::create(this)}
    {
        insert("source", StringList{});
        insert("target", "");
        insert("create", "");
        insert("update", "");
        insert("remove", "");
        insert("clean", "");
    }
};

const MetaPrototype *PredicateMetaProtocol::lookup(const String &className) const
{
    if (className == prototype_->className()) return prototype_;
    return MetaProtocol::lookup(className);
}

class ApplicationPrototype: public BuildOptionsPrototype
{
public:
    static Ref<ApplicationPrototype> create() {
        return new ApplicationPrototype{"Application"};
    }

protected:
    static Ref<MetaProtocol> createProtocol(MetaProtocol *protocol) {
        Ref<MetaProtocol> newProtocol;
        if (!protocol) protocol = newProtocol = MetaProtocol::create();
        protocol->define(SystemPrerequisitePrototype::create());
        protocol->define(PredicatePrototype::create());
        return protocol;
    }

    ApplicationPrototype(const String &className, MetaProtocol *protocol = nullptr):
        BuildOptionsPrototype{className, createProtocol(protocol)}
    {
        insert("name", "");
        insert("description", "");
        insert("alias", StringList{});
        insert("depends", StringList{});
        insert("source", StringList{"*.(cc|cpp|cxx|c)"});
        insert("bundle", StringList{});
        insert("version", Version{});
    }
};

class TestPrototype: public ApplicationPrototype
{
public:
    static Ref<TestPrototype> create() {
        return new TestPrototype{"Test"};
    }

protected:
    TestPrototype(const String &className):
        ApplicationPrototype{className}
    {
        remove("prefix");
        remove("install");
    }
};

class LibraryPrototype: public ApplicationPrototype
{
public:
    static Ref<LibraryPrototype> create() {
        return new LibraryPrototype{"Library"};
    }

protected:
    LibraryPrototype(const String &className):
        ApplicationPrototype{className}
    {}
};

class PluginPrototype: public ApplicationPrototype
{
public:
    static Ref<PluginPrototype> create(const String &className = "Plugin") {
        return new PluginPrototype{className};
    }

protected:
    PluginPrototype(const String &className):
        ApplicationPrototype{className}
    {
        insert("extend", "");
        insert("group", "");
    }
};

class ToolsPrototype: public ApplicationPrototype
{
public:
    static Ref<ToolsPrototype> create(const String &className = "Tools") {
        return new ToolsPrototype{className};
    }

protected:
    ToolsPrototype(const String &className):
        ApplicationPrototype{className}
    {
        remove("name");
    }
};

class TestsPrototype: public TestPrototype
{
public:
    static Ref<TestsPrototype> create(const String &className = "Tests") {
        return new TestsPrototype{className};
    }

protected:
    TestsPrototype(const String &className):
        TestPrototype{className}
    {
        remove("name");
    }
};

class PackagePrototype: public BuildOptionsPrototype
{
public:
    static Ref<PackagePrototype> create(const String &className = "Package") {
        return new PackagePrototype{className};
    }

protected:
    PackagePrototype(const String &className):
        BuildOptionsPrototype{className}
    {
        insert("version", Version{});
        insert("include", StringList{});
    }
};

RecipeProtocol::RecipeProtocol()
{
    define(ApplicationPrototype::create());
    define(TestPrototype::create());
    define(LibraryPrototype::create());
    define(PluginPrototype::create());
    define(ToolsPrototype::create());
    define(TestsPrototype::create());
    define(PackagePrototype::create());
}

const RecipeProtocol *RecipeProtocol::instance()
{
    return Singleton<RecipeProtocol>::instance();
}

} // namespace ccbuild
