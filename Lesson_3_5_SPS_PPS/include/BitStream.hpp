#ifndef EYERH264DEOCDER_BITSTREAM_HPP
#define EYERH264DEOCDER_BITSTREAM_HPP

#include <stdio.h>
#include <stdint.h>


/**
 * @brief 按照 bit 作为单位来读取一个数据流
 * 目前该类没有做特殊情况的处理
 */
class BitStream {
public:
    /**
     * @brief Construct a new Bit Stream object
     * 
     * @param buf 传入的bit流， 一般是一段内存数据
     * @param size 传入的 bit 流所包含的数据的大小
     */
    BitStream(uint8_t* buf, int size);
    ~BitStream();

    // 从数据流中读取一个 bit
    int readU1();
    /**
     * @brief 从数据流当中读取 n 个 Bit
     * 
     * @param n  需要读取的 bit 位数
     * @return int 
     */
    int readU(int n);
    int readUE();
    int readSE();

private:
    // 指向 buffer 开始的位置
    uint8_t* mStart = nullptr;
    // buffer 的长度 (Bytes)
    int mSize = 0;
    // 当前读取到的位置
    uint8_t* mCurrentP = nullptr;
    // 当前读取到了字节中的第几位
    int mBits_left = 0;
};

#endif /* EYERH264DEOCDER_BITSTREAM_HPP */