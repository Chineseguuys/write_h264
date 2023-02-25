#include "BitStream.hpp"

BitStream::BitStream(uint8_t* buf, int size) {
    this->mStart = buf;
    this->mCurrentP = buf;
    this->mSize = size;
    this->mBits_left = 8;
}

BitStream::~BitStream() {}

/**
 * @brief 读取一个 bit 位的数据
 */
int BitStream::readU1() {
    int r = 0;
    this->mBits_left --;
    r = ((*(this->mCurrentP)) >> this->mBits_left) & 0x01;
    if (this->mBits_left == 0) {
        this->mCurrentP++;
        this->mBits_left = 8;
    }

    return r;
}

/**
 * @brief 读取 n 个比特的数据
 * 
 * @param n  需要读取的 bit 个数
 * @return int 返回读取的值
 */
int BitStream::readU(int n) {
    int r = 0;
    int i = 0;
    for (i = 0; i < n; ++i) {
        r |= (this->readU1() << (n - i - 1));
    }

    return r;
}

/**
 * @brief 读取一个无符号指数哥伦布编码的数据，并解码为原始的数据
 * 
 * @return int 返回编码之前的值
 */
int BitStream::readUE() {
    int r = 0;
    int i = 0;

    // 首先读取前导0， 记录前导0 有多少个，直到读取到一个 1 的时候为止
    while ((this->readU1() == 0) && (i < 32)) {
        ++i;
    }
    // 注意，有 i 个前导 0， 说明我们实际编码的数据有 i + 1 位，我们要连续读取0 之后i + 1 位的数据。
    // 但是注意，在 while 循环结束之后，我们已经读取出来了一个1， 下面只需要读取 i 位数据就可以了

    r = readU(i);
    // 我们首先把最前面的一个1 通过移位的方式补上， 实际的原始值是编码后的值 -1。
    r += ((1 << i) - 1);
    return r;
} 

/**
 * @brief 读取一个有符号的指数哥伦布编码的数据，并解码为原始的数据
 * 
 * @return int 返回编码之前的值
 */
int BitStream::readSE() {
    int r = readUE();
    if (r & 0x01) {
        r = (r + 1) / 2;
    }
    else {
        r = -(r / 2);
    }
    return r;
}