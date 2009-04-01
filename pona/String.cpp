/*
 * String.cpp -- string processing based on list processing
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "Utf8Source.hpp"
#include "Utf8Sink.hpp"
#include "FormatSyntax.hpp"
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"
#include "String.hpp"

namespace pona
{

String::String()
	: Ref<List<Char>, Owner>(new Media)
{}

String::String(Char ch)
	: Ref<List<Char>, Owner>(new Media)
{
	get()->append(ch);
}

String::String(const char* utf8, int numBytes, int numChars)
	: Ref<List<Char>, Owner>(new Media)
{
	if (numBytes == -1) {
		numBytes = 0;
		if (utf8)
			while (*(utf8 + numBytes)) ++numBytes;
	}
	
	if (numBytes > 0)
	{
		if (numChars == -1) {
			numChars = 0;
			Utf8Source source((uint8_t*)utf8, numBytes);
			while (int(source.numBytesRead()) < numBytes) {
				source.readChar();
				++numChars;
			}
		}
		
		get()->push(0, numChars);
		
		{
			Utf8Source source((uint8_t*)utf8, numBytes);
			for (int i = 0; i < numChars; ++i)
				get()->set(i, source.readChar());
		}
	}
}

String::String(Ref<Media, Owner> media)
	: Ref<List<Char>, Owner>(media)
{
	if (!get()) set(new Media);
}

char* String::strdup() const
{
	CString s = utf8();
	s->liberate();
	return s;
}

CString String::utf8() const
{
	Media* media = get();
	int numChars = media->length();
	int numBytes = 0;
	{
		Utf8Sink nullSink;
		for (int i = 0; i < numChars; ++i)
			nullSink.writeChar(media->get(i));
		numBytes = nullSink.numBytesWritten();
	}
	
	CString buf(numBytes);
	Utf8Sink sink((uint8_t*)buf->at(0), numBytes);
	for (int i = 0; i < numChars; ++i)
		sink.writeChar(media->get(i));
	return buf;
}

String operator*(Char ch, int n)
{
	String s;
	if (n > 0) {
		s->push(0, n);
		for (int i = 0; i < n; ++i)
			s->set(i, ch);
	}
	return s;
}

Ref<StringList, Owner> operator/(String text, String sep)
{
	Ref<StringList, Owner> sl = new StringList();
	
	int i = 0, n = text->length(), m = sep->length();
	while (i < n) {
		int i1 = text->find(i, sep);
		sl->append(text->range(i, i1 - i));
		i = i1 + m;
	}
	if (i == n)
		sl->append(String());
	
	return sl;
}

int toInt(String s, bool* ok)
{
	bool h;
	if (!ok) ok = &h;
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (syntaxFactory()->integerLiteral()->match(s, 0, &i1, &value, &sign)) {
		 if (ok)
		 	*ok = (value <= uint64_t(intMax)) && (i1 == s->length());
	}
	else  {
		if (ok)
			*ok = false;
	}
	return sign * int(value);
}

double toFloat(String s, bool* ok)
{
	return toFloat64(s, ok);
}

/** \brief Converts given input string to a signed integer.
  * If the given integer literal exceeds the range of a signed 64 bit integer or
  * the input string can't be read as an integer *ok will be set to false.
  * The input string is expected to contain exactly one correctly formatted
  * integer literal (see pona::Format).
  */
int64_t toInt64(String s, bool* ok)
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (syntaxFactory()->integerLiteral()->match(s, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = ((value & (uint64_t(1) << 63)) != 0) && (i1 == s->length());
	}
	else {
		if (ok)
			*ok = false;
	}
	return sign * value;
}

/** \brief Converts given input string to a unsigned integer.
  * If the given integer literal exceeds the range of a unsigned 64 bit integer or
  * the input string can't be read as an integer *ok will be set to false.
  */
uint64_t toUInt64(String s, bool* ok)
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (syntaxFactory()->integerLiteral()->match(s, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = (sign == 1) && (i1 == s->length());
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

float64_t toFloat64(String s, bool* ok)
{
	float64_t value = 0.;
	int i1 = 0;
	
	if (syntaxFactory()->floatingPointLiteral()->match(s, 0, &i1, &value)) {
		if (ok)
			*ok = (i1 == s->length());
	}
	else {
		if (ok)
			*ok = false;
	}
	
	return value;
}

String toLower(String s)
{
	String s2;
	s2->push(0, s->length());
	for (int i = 0, n = s->length(); i < n; ++i) {
		Char ch = s->get(i);
		if (('A' <= ch) && (ch <= 'Z'))
			s2->set(i, ch + 'a' - 'A');
	}
	return s;
}

} // namespace pona
