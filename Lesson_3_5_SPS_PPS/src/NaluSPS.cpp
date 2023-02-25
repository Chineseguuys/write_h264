#include "NaluSPS.hpp"


NaluSPS::NaluSPS(const Nalu& nalu) : Nalu(nalu) {}

int NaluSPS::Parse() {
    BitStream bs(mRbsp.mBuf, mRbsp.mLen);

    bs.readU(8);
    this->profile_idc = bs.readU(8);
    this->constraint_set0_flag = bs.readU1();
    this->constraint_set1_flag = bs.readU1();
    this->constraint_set2_flag = bs.readU1();
    this->constraint_set3_flag = bs.readU1();
    this->constraint_set4_flag = bs.readU1();
    this->constraint_set5_flag = bs.readU1();

    bs.readU(2);
    this->level_idc = bs.readU(8);
    this->seq_parameter_set_id = bs.readUE();

    bool isHigh = ( this->profile_idc == 100        ||      this->profile_idc == 110        ||
                    this->profile_idc == 122        ||      this->profile_idc == 244        ||  this->profile_idc == 44 ||
                    this->profile_idc == 83         ||      this->profile_idc == 86         ||  this->profile_idc == 118 ||
                    this->profile_idc == 128        ||      this->profile_idc == 138        ||  this->profile_idc == 139   ||
                    this->profile_idc == 134        ||      this->profile_idc == 135
    );

    if (isHigh) {
        this->chroma_format_idc = bs.readUE();
        if (this->chroma_format_idc == 3) {
            this->separate_colour_plance_flag = bs.readU1();
        }
        // 色度和亮度的采样深度
        this->bit_depth_luma_minus8 = bs.readUE();
        this->bit_depth_chroma_minus8 = bs.readUE();
        this->qpprime_y_zero_transform_bypass_flag = bs.readU1();
        this->seq_scaling_matrix_present_flag = bs.readU1();

        /*
        seq_scaling_matrix_present_flag 是一个标志位，用于指示 H264 编码中的序列参数集（SPS）或图像参数集（PPS）是否包含量化矩阵¹²³。
        量化矩阵是一组用于控制图像质量和压缩率的系数²。如果 seq_scaling_matrix_present_flag 等于 0，表示没有量化矩阵，使用默认的全为 16 的系数¹²³。
        源: 与必应的对话， 2023/2/25(1) 编解码技术：H264 - SPS | 何照江的博客. https://hezhaojiang.github.io/post/2020/4b4fd3b8/ 访问时间 2023/2/25.
        (2) H264/AVC-SPS（序列参数集）解析_qq62的博客-CSDN博客_h264 .... https://blog.csdn.net/qq_42139383/article/details/116788882 访问时间 2023/2/25.
        (3) H264——H264码流分析实例（SPS、PPS）_Spark！的博客-CSDN .... https://blog.csdn.net/qq_28258885/article/details/119449089 访问时间 2023/2/25.
        (4) VkVideoEncodeH264CapabilityFlagBitsEXT(3) - Khronos Group. https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkVideoEncodeH264CapabilityFlagBitsEXT.html 访问时间 2023/2/25.
        */
        if (this->seq_scaling_matrix_present_flag) {
            for (int i = 0; i < ((this->chroma_format_idc != 3) ? 8 : 12); ++i) {
                this->seq_scaling_list_present_flag[i] = bs.readU1();
                if (this->seq_scaling_list_present_flag[i]) {
                    if (i < 6) {
                        this->ReadScalingList(bs, this->ScalingList4x4[i], 16, &(this->UseDefaultScalingMatrix4x4Flag[i]));
                    } else {
                        this->ReadScalingList(bs, this->ScalingList8x8[i - 6], 64, &(this->UseDefaultScalingMatrix8x8Flag[i - 6]));
                    }
                }
            }
        }
    }   // end if (isHigh)

    if (this->separate_colour_plance_flag == 0) {
        this->ChromeArrayType = chroma_format_idc;
    } else {
        this->ChromeArrayType = 0;
    }
    // SubWidthC 和 SubHeightC 是两个变量，用于表示 YUV 分量中，Y 分量和 UV 分量在水平和竖直方向上的比值
    if (this->chroma_format_idc == 1) {
        this->SubWidthC = 2;
        this->SubHeightC = 2;
    }

    if (this->chroma_format_idc == 2) { // yuv 4:2:2
        // 每一行每采样四个 Y, 值采样两个 CrCb。每一列都会采样 CrCb
        this->SubWidthC = 2;
        this->SubHeightC = 1;
    }

    if (this->chroma_format_idc == 3) {
        this->SubWidthC = 1;
        this->SubHeightC = 1;
    }

    this->log2_max_frame_num_minus4 = bs.readUE();
    this->pic_order_cnt_type = bs.readUE();

    if (this->pic_order_cnt_type == 0) {
        this->log2_max_pic_order_cnt_lsb_minus4 = bs.readUE();
    } else if (this->pic_order_cnt_type == 1) {
        this->delta_pic_order_always_zero_flag      = bs.readU1();
        this->offset_for_non_ref_pic                = bs.readSE();
        this->offset_for_top_to_bottom_field        = bs.readSE();
        this->num_ref_frames_in_pic_order_cnt_cycle = bs.readUE();

        for (int i = 0; i < this->num_ref_frames_in_pic_order_cnt_cycle; ++i) {
            this->offset_for_ref_frame[i] = bs.readSE();
        }
    }

    this->num_ref_frames = bs.readUE();
    this->gaps_in_frame_num_value_allowed_flag = bs.readU1();
    this->pic_width_in_mbs_minus1 = bs.readUE();
    this->pic_height_in_map_units_minus1 = bs.readUE();
    this->frame_mbs_only_flag = bs.readU1();

    if (!this->frame_mbs_only_flag) {
        this->mb_adaptive_frame_field_flag = bs.readU1();
    }

    this->direct_8x8_inference_flag = bs.readU1();
    this->frame_cropping_flag = bs.readU1();
    // 图像在上下左右的偏移， 我们知道宏块的大小是 16x16, 但是很多的视频的长和宽都不是 16 的倍数，所以在进行编码的时候，会对视频的上下左右添加
    // 一些额外的像素
    if (this->frame_cropping_flag) {
        this->frame_crop_left_offset = bs.readUE();
        this->frame_crop_right_offset = bs.readUE();
        this->frame_crop_top_offset = bs.readUE();
        this->frame_crop_bottom_offset = bs.readUE();
    }

    // vui (video usability information)， 它是可选的参数集合，用于描述视频的显示特性和编码约束，它可以包括下面的这些信息
    // 1 宽高比
    // 2 视频格式
    // 3 颜色空间
    // 4 时间信息
    // 5 HRD 信息
    this->vui_parameters_present_flag = bs.readU1();
    if (this->vui_parameters_present_flag) {
        this->ReadVuiParameters(bs);
    }

    return 0;
}


int NaluSPS::ReadScalingList(BitStream& bs, int* scalingList, int sizeOfScalingList, int* useDefaultScalingMatrixFlag) {
    int lastScale = 8;
    int nextScale = 8;
    int delta_scale;

    for (int j = 0; j < sizeOfScalingList; ++j) {
        if (nextScale != 0) {
            delta_scale = bs.readSE();
            nextScale = (lastScale + delta_scale + 256) % 256;
            useDefaultScalingMatrixFlag[0] = (j == 0 && nextScale == 0); 
        }

        scalingList[j] = (nextScale == 0) ? lastScale : nextScale;
        lastScale = scalingList[j];
    }

    return 0;
}

int NaluSPS::ReadHrdParameters(HRD & hrd, BitStream & bs)
{
    hrd.cpb_cnt_minus1 = bs.readUE();
    hrd.bit_rate_scale = bs.readU(4);
    hrd.cpb_size_scale = bs.readU(4);

    for(int SchedSelIdx = 0; SchedSelIdx <= hrd.cpb_cnt_minus1; SchedSelIdx++) {
        hrd.bit_rate_value_minus1[SchedSelIdx]        = bs.readUE();
        hrd.cpb_size_value_minus1[SchedSelIdx]        = bs.readUE();
        hrd.cbr_flag[SchedSelIdx]                     = bs.readU1();
    }
    hrd.initial_cpb_removal_delay_length_minus1     = bs.readU(5);
    hrd.cpb_removal_delay_length_minus1             = bs.readU(5);
    hrd.dpb_output_delay_length_minus1              = bs.readU(5);
    hrd.time_offset_length                          = bs.readU(5);

    return 0;
}

int NaluSPS::ReadVuiParameters(BitStream & bs)
{
    int vuilevel = 1;

    int SAR_Extended = 255;
    vui.aspect_ratio_info_present_flag = bs.readU1();
    if(vui.aspect_ratio_info_present_flag) {
        vui.aspect_ratio_idc = bs.readU(8);
        if(vui.aspect_ratio_idc == SAR_Extended){
            vui.sar_width      = bs.readU(16);
            vui.sar_height     = bs.readU(16);
        }
    }

    vui.overscan_info_present_flag = bs.readU1();
    if(vui.overscan_info_present_flag) {
        vui.overscan_appropriate_flag = bs.readU1();
    }

    vui.video_signal_type_present_flag = bs.readU1();
    if(vui.video_signal_type_present_flag) {
        vui.video_format                            = bs.readU(3);
        vui.video_full_range_flag                   = bs.readU1();
        vui.colour_description_present_flag         = bs.readU1();
        if(vui.colour_description_present_flag) {
            vui.colour_primaries                    = bs.readU(8);
            vui.transfer_characteristics            = bs.readU(8);
            vui.matrix_coefficients                 = bs.readU(8);
        }
    }

    vui.chroma_loc_info_present_flag                = bs.readU1();
    if(vui.chroma_loc_info_present_flag) {
        vui.chroma_sample_loc_type_top_field        = bs.readUE();
        vui.chroma_sample_loc_type_bottom_field     = bs.readUE();
    }

    vui.timing_info_present_flag = bs.readU1();
    if(vui.timing_info_present_flag) {
        vui.num_units_in_tick                       = bs.readU(32);
        vui.time_scale                              = bs.readU(32);
        vui.fixed_frame_rate_flag                   = bs.readU1();
    }

    vui.nal_hrd_parameters_present_flag = bs.readU1();
    if(vui.nal_hrd_parameters_present_flag) {
        ReadHrdParameters(vui.hrd_nal, bs);
    }

    vui.vcl_hrd_parameters_present_flag = bs.readU1();
    if(vui.vcl_hrd_parameters_present_flag) {
        ReadHrdParameters(vui.hrd_vcl, bs);
    }

    if(vui.nal_hrd_parameters_present_flag || vui.vcl_hrd_parameters_present_flag) {
        vui.low_delay_hrd_flag = bs.readU1();
    }

    vui.pic_struct_present_flag         = bs.readU1();
    vui.bitstream_restriction_flag      = bs.readU1();

    if(vui.bitstream_restriction_flag) {
        vui.motion_vectors_over_pic_boundaries_flag     = bs.readU1();
        vui.max_bytes_per_pic_denom                     = bs.readUE();
        vui.max_bits_per_mb_denom                       = bs.readUE();
        vui.log2_max_mv_length_horizontal               = bs.readUE();
        vui.log2_max_mv_length_vertical                 = bs.readUE();
        vui.num_reorder_frames                          = bs.readUE();
        vui.max_dec_frame_buffering                     = bs.readUE();
    }

    return 0;
}