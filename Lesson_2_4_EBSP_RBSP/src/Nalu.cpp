#include "Nalu.hpp"


Nalu::Nalu() {}

Nalu::~Nalu() {
    if (this->mBuf != nullptr) {
        free(this->mBuf);
        this->mBuf = nullptr;
    }
}


int Nalu::setBuf(uint8_t* _buf, int _len) {
    if (this->mBuf != nullptr) {
        free(this->mBuf);
        this->mBuf = nullptr;
    }

    this->mLen = _len;
    this->mBuf = static_cast<uint8_t*>(malloc(this->mLen));
    memcpy(this->mBuf, _buf, this->mLen);

    return 0;
}

int Nalu::getEBSP(EBSP& ebsp) {
    ebsp.mLen = this->mLen - this->mStartCodeLen;
    ebsp.mBuf = static_cast<uint8_t*>( malloc(ebsp.mLen) );

    memcpy(ebsp.mBuf, this->mBuf + this->mStartCodeLen, ebsp.mLen);

    return 0;
}