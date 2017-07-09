/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/crypto/BlockCipher>

namespace cc {
namespace crypto {

/** \class AesCipher AesCipher.h cc/crypto/AesCipher
  * \brief Rijndael Block Cipher according to the AES (FIPS-197)
  */
class AesCipher: public BlockCipher
{
public:
    enum {
        BlockSize = 16 ///< block size of the cipher in bytes
    };

    /** Create a new AES block cipher
      * \param key 128 or 256 bit key (16 or 32 bytes)
      * \return new object instance
      */
    inline static Ref<AesCipher> create(const ByteArray *key) { return new AesCipher(key); }

    void encode(const ByteArray *p, ByteArray *c) override;
    void decode(const ByteArray *c, ByteArray *p) override;

private:
    AesCipher(const ByteArray *key);
    const int Nk_, Nr_;
    Ref<ByteArray> s_;
    Ref<ByteArray> w_;
};

}} // namespace cc::crypto
