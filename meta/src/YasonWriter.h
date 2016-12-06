/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Format>
#include <cc/meta/MetaObject>

namespace cc {
namespace meta {

/** \class YasonWriter YasonWriter.h cc/meta/YasonWriter
  * \brief Generate YSON representation of a meta object tree
  * \see JsonWriter
  */
class YasonWriter: public Object
{
public:
    /** Create a new YSON writer
      * \param sink data stream to write the formatted output to
      * \param indent whitespace to use for each indentation step
      * \return new object instance
      */
    static Ref<YasonWriter> create(Stream *sink = 0, String indent = "  ");

    /** Convert a meta value into YSON notation
      * \param value meta-value
      */
    void write(Variant value);

private:
    YasonWriter(Stream *sink, String indent);
    void writeValue(Variant value, int depth);
    void writeList(Variant value, int depth);
    bool isIdentifier(String name) const;
    void writeObject(Variant value, int depth);
    void writeIndent(int depth);

    template<class T>
    void writeTypedList(Variant value, int depth);

    Format format_;
    String indent_;
};

}} // namespace cc::meta
