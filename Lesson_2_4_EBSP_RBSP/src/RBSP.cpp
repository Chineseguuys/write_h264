#include "RBSP.hpp"
#include <stdlib.h>

RBSP::RBSP() {}

RBSP::~RBSP() {
    if (this->mBuf != nullptr) {
        free(this->mBuf);
        this->mBuf = nullptr;
    }
}