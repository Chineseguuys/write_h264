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

        log_debug("=======");
        log_info("forbidden bit: %d", forbidden_bit);
        log_info("nal_ref_idc: %d", nal_ref_idc);
        log_info("nal_unit_type: %d", nal_unit_type);
    }

    h264Reader.Close();

    return 0;
}