#include "EBSP.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


EBSP::EBSP() {}

EBSP::~EBSP() {
    if (this->mBuf != nullptr) {
        free(this->mBuf);
        this->mBuf = nullptr;
    }
}


int EBSP::GetRBSP(RBSP& rbsp) {
    rbsp.mLen = this->mLen;
    rbsp.mBuf = static_cast<uint8_t*>( malloc(rbsp.mLen) );

    int targetIndex = 0;
    for (int i = 0; i < this->mLen; ++i) {
        if (this->mBuf[i] == 0x03) {
            if (i > 2) {
                if (this->mBuf[i - 1] == 0x00 && this->mBuf[i - 2] == 0x00) {
                    if (i < this->mLen - 1) {
                        if (this->mBuf[i + 1] == 0x00 || this->mBuf[i + 1] == 0x01 || this->mBuf[i + 1] == 0x02 || 
                                this->mBuf[i + 1] == 0x03) {
                            rbsp.mLen--;
                            continue;
                        }
                    }
                }
            }
        }
        rbsp.mBuf[targetIndex] = this->mBuf[i];
        targetIndex ++;
    }
    return 0;
}

int EBSP::GetRBSP_1(RBSP& rbsp) {
    rbsp.mLen = this->mLen;
    rbsp.mBuf = static_cast<uint8_t*>( malloc(rbsp.mLen) );

    int targetIndex = 0;
    static uint32_t STARTINGCODE = 0x03;
    uint32_t threeBytes = (0x01 << 8) | 0x01;
    for (int i = 0; i < this->mLen; ++i) {
        threeBytes = (threeBytes << 8) | this->mBuf[i];
        if ( (threeBytes & 0xffffff) == STARTINGCODE && i < this->mLen - 1) {
            if (this->mBuf[i + 1] == 0x00 || this->mBuf[i + 1] == 0x01 || this->mBuf[i + 1] == 0x02 || 
                this->mBuf[i + 1] == 0x03) {
                rbsp.mLen --;
                continue;
            }
        }
        rbsp.mBuf[targetIndex] = this->mBuf[i];
        targetIndex++;
    }

    return 0;
}

