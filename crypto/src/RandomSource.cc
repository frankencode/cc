/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/RandomSource>
#include <cc/crypto/PseudoPad>
#include <cc/crypto/AesCipher>
#include <cc/crypto/BlockCascade>
#include <cc/File>

namespace cc {
namespace crypto {

Ref<RandomSource> RandomSource::open(const CharArray *salt)
{
    return new RandomSource{salt};
}

RandomSource::RandomSource(const CharArray *salt)
{
    String key{16};
    String iv{AesCipher::BlockSize};

    if (salt) {
        mutate(key)->write(salt);
        mutate(iv)->fill(0);
    }
    else {
        File file{"/dev/urandom"};
        file->readSpan(mutate(key));
        file->readSpan(mutate(iv));
    }

    source_ =
        PseudoPad::open(
            BlockCascade::create(
                AesCipher::create(key),
                iv
            )
        );
}

int RandomSource::read(CharArray *data)
{
    return source_->read(data);
}

}} // namespace cc::crypto
