#include "RBSP.hpp"
#include <stdlib.h>
#include <string.h>

RBSP::RBSP() {}

RBSP::RBSP(const RBSP& rbsp) {
    *this = rbsp;
}

RBSP::~RBSP() {
    if (this->mBuf != nullptr) {
        free(this->mBuf);
        this->mBuf = nullptr;
    }
}

RBSP& RBSP::operator=(const RBSP& rbsp) {
    if (this->mBuf != nullptr) {
        free(this->mBuf);
        this->mBuf = nullptr;
    }

    this->mLen = rbsp.mLen;
    this->mBuf = static_cast<uint8_t*>( malloc(this->mLen) );
    memcpy(this->mBuf, rbsp.mBuf, this->mLen);

    return *this;
}