#include "AnnexBReader.hpp"

#ifdef __cplusplus
extern "C" {
    #include "logs.h"
}
#endif /* __cplusplus */

static const std::string h264FILENAME = "./resources/demo_video_176x144_baseline.h264";

int main(int argc, char* argv[]) {
    #ifdef LOGS_LEVEL_DEBUG
    log_set_level(LOG_DEBUG);
    #endif

    #ifdef LOGS_LEVEL_INFO
    log_set_level(LOG_INFO);
    #endif


    AnnexBReader h264Reader{h264FILENAME};
    int ret = h264Reader.Open();

    if (ret) {
        log_error("Read file %s error", h264FILENAME.c_str());
        return -1;
    }

    while (true) {
        Nalu nalu;
        ret = h264Reader.ReadNalu(nalu);
        if (ret) {
            break;
        }

        log_debug("=========");
        log_info("Buffer Len: %d", nalu.mLen);
        log_info("Start Code Len: %d", nalu.mStartCodeLen);
        log_info("%d %d %d %d", nalu.mBuf[0], nalu.mBuf[1], nalu.mBuf[2], nalu.mBuf[3]);

        // get nalu header
        // 一个 nalu 的起始符号后面的第一个字节是 nalu 的 head
        uint8_t* buf = nalu.mBuf;
        uint8_t naluHead = *(buf + nalu.mStartCodeLen);
        log_info("Nalu Header: 0x%x", naluHead);

        int forbidden_bit = (naluHead >> 7) & 0x01;
        int nal_ref_idc = (naluHead >> 5) & 0x03;
        int nal_unit_type = (naluHead >> 0) & 0x1f; 

        log_info("Nalu forbidden bit: %d", forbidden_bit);
        log_info("Nalu nal_ref_idc: %d", nal_ref_idc);
        log_info("Nalu nal_unit_type: %d", nal_unit_type);
    }

    h264Reader.Close();
    return 0;
}