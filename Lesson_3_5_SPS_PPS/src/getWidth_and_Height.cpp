#include "AnnexBReader.hpp"
#include "BitStream.hpp"
#include "NaluSPS.hpp"

#ifdef __cplusplus
extern "C" {
    #include "logs.h"
}
#endif /* __cplusplus */

// using ffmpeg copy video stream into annex-b format:
// ffmpeg -i sample_640x360_h264.mp4 -vcodec copy -bsf h264_mp4toannexb -an -f h264 sample_640x360_h264.h264
static const std::string h264_176x144_annexb = "./resources/demo_video_176x144_baseline.h264";
static const std::string h264_640x360_annexb = "./resources/sample_640x360_h264.h264";

int main(int argc, char* argv[]) {
    #ifdef LOGS_LEVEL_DEBUG
    log_set_level(LOG_DEBUG);
    #endif

    #ifdef LOGS_LEVEL_INFO
    log_set_level(LOG_INFO);
    #endif

    AnnexBReader h264Reader{h264_640x360_annexb};
    int ret = h264Reader.Open();
    if (ret) {
        log_error("Open file %s failed", h264_640x360_annexb.c_str());
        return -1;
    }

    while (1)
    {
        Nalu nalu;
        ret = h264Reader.ReadNalu(nalu);
        if (ret) {
            break;
        }

        nalu.parseRBSP();
        nalu.parseHeader();

        log_info("nalu Type is %d", nalu.mNal_unit_type);

        if (nalu.mNal_unit_type == 7) { //sps
            NaluSPS sps {nalu};
            sps.Parse();

            log_info("(pic_width_in_mbs_minus1, pic_height_in_mbs_minus1) = (%d, %d)", 
                sps.pic_width_in_mbs_minus1, sps.pic_height_in_map_units_minus1);

            bool frame_crop_offset_flag = sps.frame_cropping_flag;
            if (frame_crop_offset_flag == true) {
                log_info("(left, right, top, bottom) = (%d, %d, %d, %d)", 
                    sps.frame_crop_left_offset, 
                    sps.frame_crop_right_offset, 
                    sps.frame_crop_top_offset, 
                    sps.frame_crop_bottom_offset
                );
            }

            int chroma_format = sps.chroma_format_idc;
            log_info("chroma format idc is %d, (subWidthC, subHeightC) = (%d, %d)", chroma_format, sps.SubWidthC, sps.SubHeightC);
            int videoWidth {0};
            int videoHeight {0};

            videoWidth = (sps.pic_width_in_mbs_minus1 + 1) * H264_MACROBLOCK_SIZE;
            // 注意是否存在场编码情况 等于 0 的时候是场编码，等于 1 的时候是帧编码
            // 场编码用来兼容古老的屏幕刷新方式，几乎无使用
            videoHeight = (2 - sps.frame_mbs_only_flag) * (sps.pic_height_in_map_units_minus1 + 1) * H264_MACROBLOCK_SIZE;

            if (sps.frame_cropping_flag) {  // 视频帧使用了 crop
                int crop_unit_x {0};
                int crop_unit_y {0};
                if (sps.ChromeArrayType == 0) { // 单色，只有亮度信息
                    crop_unit_x = 1;
                    crop_unit_y = 2 - sps.frame_mbs_only_flag;
                }else {
                    crop_unit_x = sps.SubWidthC;
                    // 这里也需要考虑场编码的情况
                    crop_unit_y = sps.SubHeightC * (2 - sps.frame_mbs_only_flag);
                }

                videoWidth -= (sps.frame_crop_left_offset + sps.frame_crop_right_offset) * crop_unit_x;
                videoHeight -= (sps.frame_crop_top_offset + sps.frame_crop_bottom_offset) * crop_unit_y;
            }

            log_info("(width, height) = (%d, %d)", videoWidth, videoHeight);

            break;
        }
    }

    h264Reader.Close();

    return 0;
}