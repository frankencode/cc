/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/CipherSink>
#include <cc/crypto/CipherSource>
#include <cc/crypto/AesCipher>
#include <cc/testing/TestSuite>
#include <cc/MemoryStream>
#include <cc/NullStream>
#include <cc/Random>
#include <cc/stdio>
#include <cc/math>

using namespace cc;
using namespace cc::testing;
using namespace cc::crypto;

class TestSmth: public TestCase
{
protected:
    static String testCycle(BlockCipher *cipher, CharArray *text)
    {
        String buffer = CharArray::allocate(roundUpToNext(cipher->blockSize(), text->count()));
        {
            MemoryStream stream{mutate(buffer)};
            CipherSink sink{cipher, stream, NullStream{}};
            sink->write(text);
        }
        String text2 = String::create(text->count());
        {
            MemoryStream stream{mutate(buffer)};
            CipherSource source{cipher, stream};
            source->read(mutate(text2));
        }
        return text2;
    }

};

class TestBlockBoundary: public TestSmth
{
    void run()
    {
        String key = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c";
        AesCipher cipher{key};
        String text = "0123456789,0123456789,0123456789";
        for (int n = 1; n <= text->count(); ++n) {
            String text1 = text->copy(0, n);
            String text2 = testCycle(cipher, mutate(text1));
            fout() << text1 << " >> " << text2 << nl;
            CC_VERIFY(text1 == text2);
        }
    }
};

class TestBufferBoundary: public TestSmth
{
    void run()
    {
        String key = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c";
        AesCipher cipher{key};
        String text = String::create(0x1001);
        Random random;
        for (int i = 0; i < text->count(); ++i) mutate(text)->byteAt(i) = random->get() & 0xFF;
        {
            String text1 = text->copy(0, text->count() - 1);
            String text2 = testCycle(cipher, mutate(text1));
            CC_VERIFY(text1 == text2);
        }
        {
            String text1 = text;
            String text2 = testCycle(cipher, mutate(text1));
            CC_VERIFY(text1 == text2);
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(TestBlockBoundary);
    CC_TESTSUITE_ADD(TestBufferBoundary);

    return TestSuite::instance()->run(argc, argv);
}
