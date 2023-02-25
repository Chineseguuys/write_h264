#include "AnnexBReader.hpp"
#include "BitStream.hpp"

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
        log_error("Open file %s failed", h264FILENAME.c_str());
        return -1;
    }

    while (1)
    {
        Nalu nalu;
        ret = h264Reader.ReadNalu(nalu);
        if (ret) {
            break;
        }

        EBSP ebsp;
        ret = nalu.getEBSP(ebsp);
        if (ret) {
            break;
        }

        RBSP rbsp;
        ret = ebsp.GetRBSP(rbsp);
        if (ret) {
            break;
        }

        uint8_t naluHead = rbsp.mBuf[0];

        int forbidden_bit   = (naluHead >> 7) & 0x01;
        int nal_ref_idc     = (naluHead >> 5) & 0x03;
        int nal_unit_type   = (naluHead) & 0x1f;

        BitStream bitStream {rbsp.mBuf, rbsp.mLen};

        // Nalu 数据的第一个字节是 naluHead 数据
        bitStream.readU(8);

        if (nal_unit_type == 7) {
            // profile_idc 是一个字节的无符号数，没有使用任何编码方式
            unsigned int profile_idc = bitStream.readU(8);
            unsigned int constraint_flags = bitStream.readU(8);
            unsigned int level_idc = bitStream.readU(8);
            int seq_parameter_set_id = bitStream.readUE();

            log_info("profile_idc is %u", profile_idc);
            log_info("level_idc is %u", level_idc);
            log_info("seq_parameter_set_id is %d", seq_parameter_set_id);
            break;
        }
    }

    h264Reader.Close();

    return 0;
}