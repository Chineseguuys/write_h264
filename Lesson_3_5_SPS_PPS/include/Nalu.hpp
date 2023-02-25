#ifndef EYERH264DEOCDER_NALU_HPP
#define EYERH264DEOCDER_NALU_HPP

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include "EBSP.hpp"
#include "RBSP.hpp"

// H.264 编码中的宏块的大小为 16
#define H264_MACROBLOCK_SIZE 16

class Nalu
{
public:
    uint8_t*    mBuf = nullptr;
    int         mLen = 0;
    int         mStartCodeLen = 0;

    int mForbidden_bit = 0;
    int mNal_ref_idc = 0;
    int mNal_unit_type = 0;

    RBSP mRbsp;

public:
    Nalu();
    ~Nalu();

    Nalu& operator = (const Nalu& nalu);
    Nalu(const Nalu& nalu);

    int setBuf(uint8_t* _buf, int _len);
    /**
     * @brief EBSP 就是 Nalu 除去起始字节后剩余的部分， 
     * 对于 Annex-B 格式的数据，开头需要除去的数据为 0x001 或者 0x0001
     * 即需要除去 3个或者 4个字节
     * @param ebsp 
     * @return int 成功执行，返回 0
     */
    int getEBSP(EBSP& ebsp);

    /**
     * @brief RBSP 就是 EBSP 撤销竞争字节序操作后的原始视频流数据
     * 
     * @return int 成功解析 RBSP 返回 0
     */
    int parseRBSP();
    /**
     * @brief RBSP 数据的第一个字节（8个bits）存储的是 Nalu Head 数据。它记录了当前的 Nalu 的禁止位值，Nalu 的类型，以及 Nalu 的重要
     * 程度
     * 
     * @return int 总是返回 0
     */
    int parseHeader();
    int getNaluType();

    /**
     * @brief 用作子类的扩展
     * 
     * @return int 
     */
    virtual int Parse();
};

#endif /* EYERH264DEOCDER_NALU_HPP */