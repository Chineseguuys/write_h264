#ifndef EYERH264DEOCDER_NALU_HPP
#define EYERH264DEOCDER_NALU_HPP

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include "EBSP.hpp"

class Nalu
{
public:
    uint8_t*    mBuf = nullptr;
    int         mLen = 0;
    int         mStartCodeLen = 0;

public:
    Nalu();
    ~Nalu();

    int setBuf(uint8_t* _buf, int _len);
    int getEBSP(EBSP& ebsp);
};

#endif /* EYERH264DEOCDER_NALU_HPP */