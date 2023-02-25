#ifndef EYERLIB_EYER_AVC_BLOG_ANNEXBREADER_HPP
#define EYERLIB_EYER_AVC_BLOG_ANNEXBREADER_HPP

#include <stdint.h>
#include <string>

#include "Nalu.hpp"

class AnnexBReader{
private:
    std::string     mFilePath;
    FILE*           mF = nullptr;
    bool            mIsEnd = false;
    uint8_t*        mBuffer = nullptr;
    int             mBufferLen = 0;

private:
    bool CheckStartCode(int& startCodeLen, uint8_t* buffPtr, int bufLen);
    int ReadFromFile();

public:
    AnnexBReader(const std::string& _fileName);
    ~AnnexBReader();

    // 打开文件
    int Open();
    // 关闭文件
    int Close();
    // 读取一个 nalu
    int ReadNalu(Nalu& nalu);
};

#endif /* EYERLIB_EYER_AVC_BLOG_ANNEXBREADER_HPP */