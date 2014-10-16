/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/check>
#include <flux/yason>

using namespace flux;

void testData(MetaObject *object)
{
	check(object->className() == "Person");
	check(
		object->contains("name") &&
		object->contains("age") &&
		object->contains("hobbies") &&
		object->contains("picture") &&
		object->contains("home") &&
		object->contains("favouriteNumbers") &&
		object->contains("Is a super hero?") &&
		object->contains("motto")
	);

	check(String(object->value("name")) == "Hans Mustermann");
	check(int(object->value("age")) == 17);

	VariantList *hobbies = cast<VariantList>(object->value("hobbies"));
	check(String(hobbies->at(0)) == "Sky Diving");
	check(String(hobbies->at(1)) == "Mountain Biking");
	check(String(hobbies->at(2)) == "Poetry");

	MetaObject *picture = cast<MetaObject>(object->value("picture"));
	check(String(picture->value("uri")) == "http://www.hans-mustermann.de/photo.jpg");
	check(int(picture->value("width")) == 400);
	check(int(picture->value("height")) == 300);

	MetaObject *home = cast<MetaObject>(object->value("home"));
	check(float(home->value("latitude")) == float(12.34));
	check(float(home->value("longitude")) == float(123.4));

	VariantList *numbers = cast<VariantList>(object->value("favouriteNumbers"));
	check(int(numbers->at(0)) == 2);
	check(int(numbers->at(1)) == 5);
	check(int(numbers->at(2)) == 7);

	check(object->value("Is a super hero?"));
	check(
		String(object->value("motto")) ==
		String(
			"What I cannot create,\n"
			"I do not understand."
		)
	);
	check(
		String(object->value("smiley")) ==
		String(
			"******\n"
			"* ** *\n"
			"******\n"
			"*    *\n"
			"******\n"
		)
	);
	check(object->children()->count() == 2);
	check(String(object->children()->at(0)->value("name")) == "Jane");
	check(String(object->children()->at(1)->value("name")) == "John");
}

int main() {
	String text =
		"Person {\n"
		"  name: Hans Mustermann\n"
		"  age: 17\n"
		"  hobbies: [ Sky Diving, Mountain Biking, Poetry ]\n"
		"  picture: {\n"
		"    uri: \"http://www.hans-mustermann.de/photo.jpg\"\n"
		"    width: 400; height: 300\n"
		"  }\n"
		"  home: {\n"
		"    latitude: 12.34\n"
		"    longitude: 123.4\n"
		"  }\n"
		"  favouriteNumbers: [ 2, 5, 7 ]\n"
		"  \"Is a super hero?\": true\n"
		"  motto:\n"
		"    \"What I cannot create,\\n\"\n"
		"    \"I do not understand.\"\n"
		"  smiley:<<EOI\n"
		"******\n"
		"* ** *\n"
		"******\n"
		"*    *\n"
		"******\n"
		"EOI\n"
		"  Person {\n"
		"    name: Jane\n"
		"  }\n"
		"  Person {\n"
		"    name: John\n"
		"  }\n"
		"}";

	try {
		Ref<MetaObject> object = yason::parse(text);
		testData(object);

		String text2 = object->toString();
		fout() << text2;
		Ref<MetaObject> object2 = yason::parse(text2);
		testData(object2);
	}
	catch (TextError &ex) {
		ferr() << ex.message() << nl;
		check(false);
	}

	return 0;
}
