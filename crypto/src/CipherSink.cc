/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/crypto/CipherSink>
#include <cc/NullStream>

namespace cc {
namespace crypto {

CipherSink::Instance::Instance(BlockCipher *cipher, const Stream &sink, const Stream &pad):
    cipher_{cipher},
    sink_{sink},
    pad_{pad},
    block_{String::allocate(cipher->blockSize())}
{
    if (!pad_) pad_ = NullStream{};
}

CipherSink::Instance::~Instance()
{
    if (pending_)
        write(pad_->readSpan(cipher_->blockSize() - pending_->count()));
}

void CipherSink::Instance::write(const CharArray *data)
{
    String feed;

    if (pending_) {
        feed = String::cat(pending_, data);
        data = feed;
        pending_ = nullptr;
    }

    const int n_b = cipher_->blockSize();
    int i = 0;

    for (; i + n_b <= data->count(); i += n_b) {
        cipher_->encode(data->select(i, i + n_b), mutate(block_));
        sink_->write(block_);
    }

    if (i < data->count())
        pending_ = data->copy(i, data->count());
}

}} // namespace cc::crypto
