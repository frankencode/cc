#include <fkit/stdio.h>
#include <fkit/Sha1.h>

using namespace fkit;

int main()
{
	String test[] = {
		"abc",
		"abcdbcdecdefdefgefghfghighijhi" "jkijkljklmklmnlmnomnopnopq",
		"a",
		"01234567012345670123456701234567" "01234567012345670123456701234567"
	};
	const int testCount = sizeof(test) / sizeof(test[0]);
	const int repeatCount[] = {
		1, 1, 1000000, 10
	};
	String results[] = {
		"A9 99 3E 36 47 06 81 6A BA 3E 25 71 78 50 C2 6C 9C D0 D8 9D",
		"84 98 3E 44 1C 3B D2 6E BA AE 4A A1 F9 51 29 E5 E5 46 70 F1",
		"34 AA 97 3C D4 C4 DA A4 F6 1E EB 2B DB AD 27 31 65 34 01 6F",
    	"DE A3 56 A2 CD DD 90 C7 A7 EC ED C5 EB B5 63 93 4F 46 04 52"
    };

	for (int i = 0; i < testCount; ++i) {
		Ref<Sha1> hash = Sha1::create();
		for (int j = 0; j < repeatCount[i]; ++j)
			hash->feed(test[i]);
		hash->finish();
		String requiredSum = hash->sum()->hex();
		String sum = results[i]->replace(" ", "")->downcase();
		fout("SHA-1 of \"%%\" repeated %% time(s):") << test[i] << repeatCount[i] << nl;
		fout() << "  " << requiredSum << " (required)" << nl;
		fout() << "  " << sum << " (delivered)" << nl;
		if (sum != requiredSum) return i + 1;
	}

	return 0;
}