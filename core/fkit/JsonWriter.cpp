/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Format.h"
#include "JsonWriter.h"

namespace fkit
{

Ref<JsonWriter> JsonWriter::create(Format format, String indent) { return new JsonWriter(format, indent); }

JsonWriter::JsonWriter(Format format, String indent)
	: format_(format),
	  indent_(indent)
{}

void JsonWriter::write(Variant value)
{
	writeValue(value, 0);
	format_ << nl;
}

void JsonWriter::writeValue(Variant value, int depth)
{
	if ( type(value) == Variant::IntType ||
	     type(value) == Variant::BoolType ||
	     type(value) == Variant::FloatType )
	{
		format_ << value;
	}
	else if (type(value) == Variant::StringType) {
		String s = value;
		if (s->contains("\""))
			s = s->replace("\"", "\\\"");
		s = s->escape();
		format_ << "\"" << s << "\"";
	}
	else if (type(value) == Variant::ListType) {
		writeList(value, depth);
	}
	else if (type(value) == Variant::ObjectType) {
		writeObject(value, depth);
	}
}

void JsonWriter::writeList(Variant value, int depth)
{
	if (itemType(value) == Variant::IntType)
		writeTypedList<int>(value, depth);
	else if (itemType(value) == Variant::BoolType)
		writeTypedList<bool>(value, depth);
	else if (itemType(value) == Variant::FloatType)
		writeTypedList<float>(value, depth);
	else if (itemType(value) == Variant::StringType)
		writeTypedList<String>(value, depth);
	else
		writeTypedList<Variant>(value, depth);
}

void JsonWriter::writeObject(Variant value, int depth)
{
	Ref<YasonObject> object = cast<YasonObject>(value);
	if (object->className() != "") {
		format_ << object->className();
		format_ << " ";
	}
	if (object->size() == 0) {
		format_ << "{}";
		return;
	}
	format_ << "{\n";
	writeIndent(depth + 1);
	for (int i = 0; i < object->size(); ++i) {
		String memberName = object->keyAt(i);
		Variant memberValue = object->valueAt(i);
		format_ << "\"" << memberName << "\": ";
		writeValue(memberValue, depth + 1);
		if (i < object->size() - 1) {
			format_ << ",\n";
			writeIndent(depth + 1);
		}
		else {
			format_ << "\n";
			writeIndent(depth);
		}
	}
	format_ << "}";
}

void JsonWriter::writeIndent(int depth)
{
	for (int i = 0; i < depth; ++i) format_ << indent_;
}

template<class T>
void JsonWriter::writeTypedList(Variant value, int depth)
{
	List<T> *list = cast< List<T> >(value);
	if (list->size() == 0) {
		format_ << "[]";
		return;
	}
	format_ << "[ ";
	for (int i = 0; i < list->size(); ++i) {
		writeValue(list->at(i), depth);
		if (i < list->size() - 1)
			format_ << ", ";
	}
	format_ << " ]";
}

} // namespace fkit
