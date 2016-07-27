/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <new>
#include <cc/Color>
#include <cc/Version>
#include <cc/String>
#include <cc/Map>

namespace cc {

class Variant;

typedef List<Variant> VariantList;
typedef Map<String, Variant> VariantMap;

/** \class Variant Variant.h cc/Variant
  * \brief Duck-typed value
  *
  * A Variant can represent different types.
  * The type of a Variant is defined implicitly at construction or assignment.
  * Variants automatically cast to bool, int, float if the Variant type is compatible
  * with the target type requested by an expression.
  * In debug mode a DebugException will be thrown on illegal type casts.
  * Variants can be savely passed by value.
  */
class Variant
{
public:
    typedef float real;

    enum Type {
        UndefType   = 0,
        IntType     = 1,
        BoolType    = 2 | IntType,
        FloatType   = 4,
        ColorType   = 8,
        VersionType = 16,
        ObjectType  = 32,
        StringType  = 64 | ObjectType,
        ListType    = 128 | ObjectType,
        MapType     = 256 | ObjectType,
        AnyType     = 511
    };

    static const char *typeName(int type, int itemType = UndefType);
    static Variant read(String s);

    Variant():              type_(UndefType),   itemType_(UndefType)                     {}
    Variant(int value):     type_(IntType),     itemType_(UndefType), int_(value)        {}
    Variant(bool value):    type_(BoolType),    itemType_(UndefType), int_(value)        {}
    Variant(float value):   type_(FloatType),   itemType_(UndefType), float_(value)      {}
    Variant(double value):  type_(FloatType),   itemType_(UndefType), float_(value)      {}
    Variant(Color value):   type_(ColorType),   itemType_(UndefType), word_(value.word_) {}
    Variant(Version value): type_(VersionType), itemType_(UndefType), word_(value.n())   {}

    Variant(const char *value):   type_(StringType), itemType_(UndefType) { initRef(String(value).get()); }
    Variant(const String &value): type_(StringType), itemType_(UndefType) { initRef(value.get()); }
    template<class T>
    Variant(const Ref<T> &value): type_(ObjectType), itemType_(UndefType) { initRef(value.get()); }

    Variant(const Ref< List<int> > &value):     type_(ListType), itemType_(IntType)     { initRef(value); }
    Variant(const Ref< List<bool> > &value):    type_(ListType), itemType_(BoolType)    { initRef(value); }
    Variant(const Ref< List<float> > &value):   type_(ListType), itemType_(FloatType)   { initRef(value); }
    Variant(const Ref< List<Color> > &value):   type_(ListType), itemType_(ColorType)   { initRef(value); }
    Variant(const Ref< List<Version> > &value): type_(ListType), itemType_(VersionType) { initRef(value); }
    Variant(const Ref<StringList> &value):      type_(ListType), itemType_(StringType)  { initRef(value); }
    Variant(const Ref<VariantList> &value):     type_(ListType), itemType_(AnyType)     { initRef(value); }
    Variant(const Ref<VariantMap> &value):      type_(ListType), itemType_(AnyType)     { initRef(value); }

    Variant(const Variant &b): type_(UndefType), itemType_(UndefType) { *this = b; }

    ~Variant() { if (type_ & ObjectType) killRef(); }

    inline const Variant &operator=(bool value)          { type_ = BoolType; int_ = value; return *this; }
    inline const Variant &operator=(int value)           { type_ = IntType;  int_ = value; return *this; }
    inline const Variant &operator=(float value)         { type_ = FloatType; float_ = value; return *this; }
    inline const Variant &operator=(double value)        { type_ = FloatType; float_ = value; return *this; }
    inline const Variant &operator=(Color value)         { type_ = ColorType; word_ = value.word_; return *this; }
    inline const Variant &operator=(Version value)       { type_ = VersionType; word_ = value.n(); return *this; }
    inline const Variant &operator=(const char *value)   { return *this = Variant(value); }
    inline const Variant &operator=(const String &value) { return *this = Variant(value); }
    template<class T>
    inline const Variant &operator=(const Ref<T> &value) { return *this = Variant(value); }

    inline const Variant &operator=(const Variant &b) {
        if (type_ & ObjectType) killRef();
        type_ = b.type_;
        itemType_ = b.itemType_;
        if (b.type_ & ObjectType)
            initRef(b.ref().get());
        else if (b.type_)
            int_ = b.int_;
        return *this;
    }

    inline operator bool() const {
        if (!type_) return bool();
        CC_ASSERT2(type_ & IntType, illegalConversion());
        return int_;
    }
    inline operator int() const {
        if (!type_) return int();
        CC_ASSERT2(type_ & IntType, illegalConversion());
        return int_;
    }
    inline operator float() const {
        if (!type_) return float();
        if (type_ & IntType) return int_;
        CC_ASSERT2(type_ & FloatType, illegalConversion());
        return float_;
    }
    inline operator Color() const {
        if (!type_) return Color();
        CC_ASSERT2(type_ & ColorType, illegalConversion());
        return Color::cast(word_);
    }
    inline operator Version() const {
        return toVersion(*this);
    }
    inline operator String() const {
        return toString(*this);
    }

    inline static Version toVersion(const Variant &value) {
        if (!value.type_) return Version();
        CC_ASSERT2(value.type_ & VersionType, illegalConversion());
        return Version::cast(value.word_);
    }

    inline static String toString(const Variant &value) {
        if (!value.type_) return String();
        CC_ASSERT2(value.type_ & StringType, illegalConversion());
        return cast<ByteArray>(value.ref().get());
    }


    template<class T>
    inline operator Ref<T>() const {
        if (!type_) return Ref<T>();
        CC_ASSERT2(type_ & ObjectType, illegalConversion());
        return cast<T>(ref().get());
    }

    bool operator==(const Variant &b) const;
    bool operator<(const Variant &b) const;

    inline bool operator>(const Variant &b) const  { return b < *this; }
    inline bool operator!=(const Variant &b) const { return !(*this == b); }
    inline bool operator<=(const Variant &b) const { return (*this < b) || (*this == b); }
    inline bool operator>=(const Variant &b) const { return (b < *this) || (*this == b); }

private:
    friend String str(const Variant &x);
    friend int type(const Variant &value);
    friend int itemType(const Variant &value);

    template<class U>
    friend U *cast(const Variant &value);

    inline static const char *illegalConversion() { return "Illegal Variant conversion"; }

    inline void initRef(Object *object = 0) {
        new(dummy_)Ref<Object>(object);
    }
    inline void killRef() {
        ref().~Ref<Object>();
    }
    inline void setRef(Object *object) const {
        ref() = object;
    }
    inline Ref<Object> &ref() const {
        return *union_cast< Ref<Object> *>(dummy_);
    }

    unsigned short type_;
    unsigned short itemType_;
    union {
        int32_t int_;
        uint32_t word_;
        float32_t float_;
        mutable char dummy_[sizeof(Ref<Object>)];
    };
};

String str(const Variant &x);

inline int type(const Variant &value) { return value.type_; }
inline int itemType(const Variant &value) { return value.itemType_; }

template<class U>
inline U *cast(const Variant &value) { return type(value) & Variant::ObjectType ? cast<U>(value.ref()) : null<U>(); }

} // namespace cc
