#ifndef EYERH264DEOCDER_EBSP_HPP
#define EYERH264DEOCDER_EBSP_HPP

#include <stdint.h>
#include "RBSP.hpp"

class EBSP {
public:
    EBSP();
    EBSP(const EBSP& ebsp);
    EBSP& operator = (const EBSP& ebsp);
    ~EBSP();

    /**
     * @brief 防竞争字节序的处理，在进行解码之前，必须恢复原始数据
     * @param rbsp 存储恢复之后的数据
     * @return int 
     */
    int GetRBSP(RBSP& rbsp);
    int GetRBSP_1(RBSP& rbsp);

public:
    uint8_t* mBuf = nullptr;
    int mLen = 0;
};

#endif /* EYERH264DEOCDER_EBSP_HPP */