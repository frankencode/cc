/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/meta/YasonWriter>

namespace cc {
namespace meta {

Ref<YasonWriter> YasonWriter::create(const Stream &sink, const String &indent)
{
    return new YasonWriter{sink, indent};
}

YasonWriter::YasonWriter(const Stream &sink, const String &indent):
    format_{sink},
    indent_{indent}
{}

void YasonWriter::write(Variant value)
{
    writeValue(value, 0);
    format_ << nl;
}

void YasonWriter::writeValue(Variant value, int depth)
{
    if (
        value->type() == VariantType::Int ||
        value->type() == VariantType::Bool ||
        value->type() == VariantType::Float ||
        value->type() == VariantType::Color ||
        value->type() == VariantType::Version
    ) {
        format_ << value;
    }
    else if (value->type() == VariantType::String) {
        String s = value;
        if (s->contains("\""))
            s = s->replace("\"", "\\\"");
        s = s->escape();
        format_ << "\"" << s << "\"";
    }
    else if (value->type() == VariantType::List) {
        writeList(value, depth);
    }
    else if (value->type() == VariantType::Object) {
        writeObject(value, depth);
    }
}

void YasonWriter::writeList(Variant value, int depth)
{
    if (value->itemType() == VariantType::Int)
        writeTypedList<int>(value, depth);
    else if (value->itemType() == VariantType::Bool)
        writeTypedList<bool>(value, depth);
    else if (value->itemType() == VariantType::Float)
        writeTypedList<float>(value, depth);
    else if (value->itemType() == VariantType::String)
        writeTypedList<String>(value, depth);
    else
        writeTypedList<Variant>(value, depth);
}

bool YasonWriter::isIdentifier(const String &name) const
{
    for (char ch: name) {
        if (!(
            (('a' <= ch) && (ch <= 'z')) ||
            (('A' <= ch) && (ch <= 'Z')) ||
            (ch == '_') ||
            (ch == '-') ||
            (('0' <= ch) && (ch <= '9'))
        ))
            return false;
    }
    return true;
}

void YasonWriter::writeObject(Variant value, int depth)
{
    const MetaObject *object = Variant::cast<const MetaObject *>(value);
    if (!object) {
        format_ << "null";
        return;
    }
    if (object->className() != "") {
        format_ << object->className();
        format_ << " ";
    }
    if (object->count() == 0 && !object->hasChildren()) {
        format_ << "{}";
        return;
    }
    format_ << "{\n";
    writeIndent(depth + 1);
    for (int i = 0; i < object->count(); ++i) {
        String memberName = object->at(i)->key();
        Variant memberValue = object->at(i)->value();
        if (isIdentifier(memberName))
            format_ << memberName << ": ";
        else
            format_ << "\"" << memberName << "\": ";
        writeValue(memberValue, depth + 1);
        format_ << "\n";
        if (i < object->count() - 1)
            writeIndent(depth + 1);
    }
    if (object->hasChildren()) {
        if (object->count() > 0) writeIndent(depth + 1);
        const MetaObjectList &children = object->children();
        for (int i = 0; i < children->count(); ++i) {
            writeObject(children->at(i), depth + 1);
            format_ << "\n";
            if (i < children->count() -1)
                writeIndent(depth + 1);
        }
    }
    writeIndent(depth);
    format_ << "}";
}

void YasonWriter::writeIndent(int depth)
{
    for (int i = 0; i < depth; ++i) format_ << indent_;
}

template<class T>
void YasonWriter::writeTypedList(Variant value, int depth)
{
    List<T> list = Variant::cast< List<T> >(value);
    if (list->count() == 0) {
        format_ << "[]";
        return;
    }
    format_ << "[ ";
    for (int i = 0; i < list->count(); ++i) {
        writeValue(list->at(i), depth);
        if (i < list->count() - 1)
            format_ << ", ";
    }
    format_ << " ]";
}

}} // namespace cc::meta
