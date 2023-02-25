#include "Nalu.hpp"


Nalu::Nalu() {}

Nalu::~Nalu() {
    if (this->mBuf != nullptr) {
        free(this->mBuf);
        this->mBuf = nullptr;
    }
}

// 防止 Nalu 和 NaluSPS 出现 double free
Nalu::Nalu(const Nalu& nalu) {
    *this = nalu;
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


Nalu& Nalu::operator=(const Nalu& nalu) {
    if (this->mBuf != nullptr) {
        free(this->mBuf);
        this->mBuf = nullptr;
    }

    this->mStartCodeLen = nalu.mStartCodeLen;
    this->mLen = nalu.mLen;

    this->mBuf = static_cast<uint8_t*>( malloc( this->mLen) );
    memcpy(this->mBuf, nalu.mBuf, this->mLen);

    this->mForbidden_bit = nalu.mForbidden_bit;
    this->mNal_ref_idc = nalu.mNal_ref_idc;
    this->mNal_unit_type = nalu.mNal_unit_type;

    this->mRbsp = nalu.mRbsp;

    return *this;
}

int Nalu::parseRBSP() {
    EBSP ebsp;
    int ret = this->getEBSP(ebsp);
    if (ret) return -1;

    return ebsp.GetRBSP(this->mRbsp);
}


int Nalu::parseHeader() {
    uint8_t naluHead = this->mRbsp.mBuf[0];
    this->mForbidden_bit = (naluHead >> 7) & 0x01;
    this->mNal_ref_idc = (naluHead >> 5) & 0x03;
    this->mNal_unit_type = (naluHead) & 0x1f;
    return 0;
}

int Nalu::getNaluType() {
    return this->mNal_unit_type;
}

int Nalu::Parse() {
    return 0;
}