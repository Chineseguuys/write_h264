#include "AnnexBReader.hpp"

#ifdef __cplusplus
extern "C" {
    #include "logs.h"
}
#endif /* __cplusplus */

AnnexBReader::AnnexBReader(const std::string& _fileName) {
    this->mFilePath = _fileName;
}

AnnexBReader::~AnnexBReader() {
    this->Close();
}

int AnnexBReader::Open() {
    if (this->mF != nullptr) {
        fclose(this->mF);
        this->mF == nullptr;
    }

    this->mF = fopen(this->mFilePath.c_str(), "rb");
    if (this->mF == nullptr) {
        log_error("open file %s error", this->mFilePath.c_str());
        return -1;
    }

    return 0;
}


int AnnexBReader::Close() {
    if (this->mF != nullptr) {
        log_info("AnnexBReader close file %s[%p]", this->mFilePath.c_str(), this->mF);
        fclose(this->mF);
        this->mF = nullptr;
    }

    if (this->mBuffer != nullptr) {
        free(this->mBuffer);
        this->mBuffer = nullptr;
        this->mBufferLen = 0;
    }

    return 0;
}


int AnnexBReader::ReadFromFile() {
    static int tempBufferLen = 1024;
    uint8_t* buf = static_cast<uint8_t*>( malloc(tempBufferLen) );
    int readLen = fread(buf, 1, tempBufferLen, this->mF);

    if (readLen > 0) {
        uint8_t* _buffer = static_cast<uint8_t*>( malloc(this->mBufferLen + readLen) );
        memcpy(_buffer, this->mBuffer, this->mBufferLen);
        memcpy(_buffer + this->mBufferLen, buf, readLen);
        this->mBufferLen = this->mBufferLen + readLen;

        if (this->mBuffer != nullptr) {
            free(this->mBuffer);
            this->mBuffer = nullptr;
        }

        this->mBuffer = _buffer;
    }

    free(buf);
    return readLen;
}

bool AnnexBReader::CheckStartCode(int& startCodecLen, uint8_t* bufPtr, int bufLen) {
    static unsigned int STARTCODE_3 = (0x00 << 16) | (0x00 << 8) | (0x01);
    static unsigned int STARTCODE_4 = (0x00 << 24) | (0x00 << 16) | (0x00 << 8) | (0x01);

    if (bufLen <= 2) {
        // 当前的 buffer 当中可能无法完全容纳下一个完整的 Nalu 的数据，可能需要读取新的数据来继续继续进行判断
        startCodecLen = 0;
        log_trace("buf is too short to read start code!");
        return false;
    }
    unsigned int startCode = 0x00;

    startCode = (bufPtr[0] << 16)| (bufPtr[1] << 8) | (bufPtr[2]);
    if (startCode == STARTCODE_3) {
        startCodecLen = 3;
        return true;
    }
    if (bufLen >= 4) {
        startCode = (startCode << 8) | (bufPtr[3]);
        if (startCode == STARTCODE_4) {
            startCodecLen = 4;
            return true;
        }
    }

    startCodecLen = 0;
    return false;
}

int AnnexBReader::ReadNalu(Nalu& nalu) {
    while(1) {
        if (this->mBufferLen <= 0) {
            int readLen = this->ReadFromFile();
            if (readLen <= 0) {
                this->mIsEnd = true;
            }
        }

        uint8_t* buf = this->mBuffer;

        int startCodecLen = 0;
        bool isStartCode = this->CheckStartCode(startCodecLen, buf, this->mBufferLen);
        if (!isStartCode) {
            break;
        }

        nalu.mStartCodeLen = startCodecLen;
        int endPos = -1;

        for (int i = 2; i < this->mBufferLen; ++i) {
            startCodecLen = 0;
            isStartCode = this->CheckStartCode(startCodecLen, buf + i, this->mBufferLen - i);
            if (isStartCode) {
                endPos = i;
                break;
            }
        }

        if (endPos > 0) {
            nalu.setBuf(this->mBuffer, endPos);
            uint8_t* _buffer = static_cast<uint8_t*>(malloc(this->mBufferLen - endPos));

            memcpy(_buffer, this->mBuffer + endPos, this->mBufferLen - endPos);

            if (this->mBuffer) {
                free(this->mBuffer);
                this->mBuffer = nullptr;
            }

            this->mBuffer = _buffer;
            this->mBufferLen = this->mBufferLen - endPos;

            return 0;
        }
        else {
            if (mIsEnd == true) {
                nalu.setBuf(this->mBuffer, this->mBufferLen);
                if (this->mBuffer != nullptr) {
                    free(this->mBuffer);
                    this->mBuffer = nullptr;
                }

                this->mBuffer = nullptr;
                this->mBufferLen = 0;

                return 0;
            }

            int readedLen = this->ReadFromFile();
            if (readedLen <= 0) {
                this->mIsEnd = true;
            }
        }
    }

    return -1;
}