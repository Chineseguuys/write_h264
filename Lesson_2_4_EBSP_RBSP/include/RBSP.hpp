#ifndef EYERH264DEOCDER_RBSP_HPP
#define EYERH264DEOCDER_RBSP_HPP

#include <stdint.h>

class RBSP {
public:
    RBSP();
    ~RBSP();

public:
    uint8_t* mBuf = nullptr;
    int mLen = 0;
};

#endif /* EYERH264DEOCDER_RBSP_HPP */