/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug> // DEBUG
#include <cc/format>
#include <cc/Arguments>

namespace cc {

Ref<Arguments> Arguments::parse(int argc, char **argv, VariantMap *options)
{
    return new Arguments{argc, argv, options};
}

Arguments::Arguments(int argc, char **argv, VariantMap *options):
    options_{VariantMap::create()},
    items_{StringList::create()}
{
    execPath_ = argv[0];

    for (int i = 1; i < argc; ++i)
    {
        string s = argv[i];
        bool isKeyValueOption = s->contains('=');
        bool isFlag = s->startsWith('-') && s->count() >= 2 && (s->at(1) < '0' || '9' < s->at(1)) && s->at(1) != '.';
        if (!isFlag && !isKeyValueOption) {
            items_->append(s);
            continue;
        }

        if (isFlag) mutate(s)->trimInsitu("-");

        variant value = true;
        if (isKeyValueOption) {
            Ref<StringList> parts = s->split('=');
            string name = parts->front();
            parts->popFront();
            string valueText = parts->join("=");
            options_->establish(name, variant::read(valueText));
        }
        else {
            bool value = true;
            if (s->startsWith("no-")) {
                value = false;
                s = s->copy(3, s->count());
            }
            options_->establish(s, value);
        }
    }

    if (options_->contains("h") || options_->contains("help") || options_->contains("?"))
        throw HelpRequest{};

    if (options_->contains("v") || options_->contains("version"))
        throw VersionRequest{};

    if (options) {
        validate(options);
        override(options);
        options_ = options;
    }
}

void Arguments::validate(const VariantMap *prototype)
{
    for (int i = 0; i < options_->count(); ++i)
    {
        string name = options_->at(i)->key();
        variant value = options_->at(i)->value();

        variant defaultValue;
        if (!prototype->lookup(name, &defaultValue))
            throw UsageError{format{"No such option: \"%%\""} << name};
        if (defaultValue == variant{}) continue;
        if (value->type() != defaultValue->type()) {
            if (value->type() == VariantType::Int && defaultValue->type() == VariantType::Bool) {
                int intValue = value;
                if (intValue == 0 || intValue == 1)
                    options_->establish(name, intValue == 1);
            }
            else if (value->type() == VariantType::Int && defaultValue->type() == VariantType::Float) {
                options_->establish(name, float(int(value)));
            }
            else if (value->type() != VariantType::Object && defaultValue->type() == VariantType::String) {
                options_->establish(name, str(value));
            }
            else if (value->type() == VariantType::String && defaultValue->type() == VariantType::List && defaultValue->itemType() == VariantType::String) {
                options_->establish(name, StringList::create() << str(value));
            }
            else if (value->type() == VariantType::Int && defaultValue->type() == VariantType::List && defaultValue->itemType() == VariantType::Int) {
                options_->establish(name, List<int>::create() << int(value));
            }
            else {
                throw UsageError{
                    format{"Option \"%%\" expects type %% (got %%: %%)"}
                        << name
                        << defaultValue->typeName()
                        << value->typeName()
                        << value
                };
            }
        }
    }
}

void Arguments::override(VariantMap *config) const
{
    for (int i = 0; i < options_->count(); ++i)
    {
        string name = options_->at(i)->key();
        variant value = options_->at(i)->value();
        config->establish(name, value);
    }
}

} // namespace cc
