#include "Utf8Source.hpp"
#include "Crc32.hpp"
#include "FormatSyntax.hpp"
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"
#include "Variant.hpp"
#include "UString.hpp"

namespace pona
{

UString::UString(const Variant& b)
	: Super(b.toInstance<Media>())
{}

void UString::validate(const char* data, int size)
{
	if (size < 0) size = pona::strlen(data);
	if (size > 0) {
		Utf8Source source(data, size);
		while (source.numBytesRead() < size)
			source.readChar();
	}
}

void UString::assign(Ref<UStringList> parts, const char* glue)
{
	int glueSize = pona::strlen(glue);
	validate(glue, glueSize);
	int ni = parts->length();
	if (ni > 0) {
		int size = 0;
		for (int i = 0; i < ni; ++i)
			size += parts->get(i).size();
		size += (ni - 1) * glueSize;
		set(new Media(size));
		char* p = data();
		for (int i = 0; i < ni; ++i) {
			UString part = parts->get(i);
			pona::memcpy(p, part.data(), part.size());
			p += part.size();
			if (i < ni - 1) {
				pona::memcpy(p, glue, glueSize);
				p += glueSize;
			}
		}
		assert(p == data() + size);
	}
	else {
		set(new Media);
	}
}

UString UString::deepCopy() const
{
	UString b;
	b.Super::set(new Media(data(), size()));
	return b;
}

UString::Index UString::find(const Index& index, const char* pattern) const
{
	if (!index.valid() || empty()) return Index();
	assert(index.data() == data());
	const char* t = index.pos(); // text pos
	const char* m = pattern; // match pos
	while ((*t) && (*m)) {
		if (*t == *m)
			++m;
		else
			m = pattern;
		++t;
	}
	return (*m) ? Index() : Index(data(), t - (m - pattern));
}

Ref<UStringList, Owner> UString::split(const char* pattern) const
{
	Ref<UStringList, Owner> parts = new UStringList;
	Index index0 = first();
	int patternSize = pona::strlen(pattern);
	while (index0.valid()) {
		Index index1 = find(index0, pattern);
		if (!index1.valid()) break;
		parts->append(UString(index0, index1));
		index0 = Index(data(), index1.pos() + patternSize);
	}
	if (index0.valid())
		parts->append(UString(index0, eoi()));
	else
		parts->append(UString());
	return parts;
}

int UString::toInt(bool* ok)
{
	bool h;
	if (!ok) ok = &h;
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		 *ok = (value <= uint64_t(intMax)) && (i1 == size());
	}
	else  {
		*ok = false;
	}
	return sign * int(value);
}

double UString::toFloat(bool* ok)
{
	return toFloat64(ok);
}

int64_t UString::toInt64(bool* ok)
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = ((value & (uint64_t(1) << 63)) != 0) && (i1 == size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return sign * value;
}

uint64_t UString::toUInt64(bool* ok)
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = (sign == 1) && (i1 == size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

float64_t UString::toFloat64(bool* ok)
{
	float64_t value = 0.;
	int i1 = 0;
	if (formatSyntax()->floatingPointLiteral()->match(*this, 0, &i1, &value)) {
		if (ok)
			*ok = (i1 == size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

UString UString::toLower() const
{
	UString s2(size());
	for (int i = 0, n = size(); i < n; ++i)
		s2->set(i, pona::toLower((*this)->at(i)));
	return s2;
}

UString UString::toUpper() const
{
	UString s2(size());
	for (int i = 0, n = size(); i < n; ++i)
		s2->set(i, pona::toUpper((*this)->at(i)));
	return s2;
}

UString UString::stripLeadingSpace() const
{
	int n = size();
	while (n > 0) {
		if (!pona::isSpace((*this)->at(-n))) break;
		--n;
	}
	return UString(*this, size() - n, n);
}

UString UString::stripTrailingSpace() const
{
	int n = size();
	while (n > 0) {
		if (!pona::isSpace((*this)->at(n - 1))) break;
		--n;
	}
	return UString(*this, 0, n);
}

uint32_t UString::crc32() const
{
	Crc32 crc;
	crc.feed(data(), size());
	return crc.sum();
}

Ref<UStringList, Owner> operator+(const UString& a, const UString& b)
{
	Ref<UStringList, Owner> list = new UStringList;
	list->append(a);
	list->append(b);
	return list;
}

Ref<UStringList, Owner> operator+(const UString& a, const char* b)
{
	Ref<UStringList, Owner> list = new UStringList;
	list->append(a);
	list->append(UString(b));
	return list;
}

Ref<UStringList, Owner> operator+(const char* a, const UString& b)
{
	Ref<UStringList, Owner> list = new UStringList;
	list->append(UString(a));
	list->append(b);
	return list;
}

Ref<UStringList, Owner> operator+(Ref<UStringList, Owner> list, const UString& b)
{
	list->append(b);
	return list;
}

Ref<UStringList, Owner> operator+(Ref<UStringList, Owner> list, const char* b)
{
	list->append(UString(b));
	return list;
}

} // namespace pona
