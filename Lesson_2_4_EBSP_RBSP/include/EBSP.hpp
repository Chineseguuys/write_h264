#ifndef EYERH264DEOCDER_EBSP_HPP
#define EYERH264DEOCDER_EBSP_HPP

#include <stdint.h>
#include "RBSP.hpp"

class EBSP {
public:
    EBSP();
    ~EBSP();

    // 防竞争字节序的处理，在进行解码之前，必须恢复原始数据
    int GetRBSP(RBSP& rbsp);
    int GetRBSP_1(RBSP& rbsp);

public:
    uint8_t* mBuf = nullptr;
    int mLen = 0;
};

#endif /* EYERH264DEOCDER_EBSP_HPP */