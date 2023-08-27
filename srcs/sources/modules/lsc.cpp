/**
 * @file lsc.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-27
 * 
 * Copyright (c) of ADAS_EYES 2023
 * 
 */

#include "modules/modules.h"

#define MOD_NAME "lsc"
/*
*gain0         *gain1

      . x_coff,y_coff


*gain2         *gain3

x_coff 0~1
y_coff 0~1
*/
static inline float BilinearInterpolation(float gain0, float gain1, float gain2, float gain3, float x_coff, float y_coff)
{
    if (x_coff > 1 || y_coff > 1) {
        return 1;
    }

    float gain01 = gain0 * x_coff + gain1 * (1 - x_coff);
    float gain23 = gain2 * x_coff + gain3 * (1 - x_coff);

    return gain01 * y_coff + gain23 * (1 - y_coff);
}


static int Lsc(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;
    int pwl_idx = 0;

    int32_t* raw32_in = reinterpret_cast<int32_t *>(frame->data.raw_s32_i);
    int32_t* raw32_out = reinterpret_cast<int32_t *>(frame->data.raw_s32_o);

    const LscPrms *lsc_prm = &(isp_prm->lsc_prms);

    const int mesh_box_width =  static_cast<int>(ceil((float)frame->info.width / kLscMeshBoxHNums)) ;
    const int mesh_box_height =  static_cast<int>(ceil((float)frame->info.height / kLscMeshBoxVNums));

    //std::cout << mesh_box_width << "|" << mesh_box_height;

    FOR_ITER(ih, frame->info.height)
    {
        FOR_ITER(iw, frame->info.width)
        {
            int pixel_idx = GET_PIXEL_INDEX(iw, ih, frame->info.width);
            int box_idx =  iw / mesh_box_width;
            int box_idy =  ih / mesh_box_height;
            float x_coff = (float)(iw % mesh_box_width) / mesh_box_width;
            float y_coff = (float)(ih % mesh_box_height) / mesh_box_height;
            int cfa_id = static_cast<int>(frame->info.cfa);
            float gain = 1;

            switch (kPixelCfaLut[cfa_id][iw % 2][ih % 2])
            {
            case PixelCfaTypes::R:
                gain = BilinearInterpolation(lsc_prm->mesh_r[box_idy][box_idx], lsc_prm->mesh_r[box_idy][box_idx + 1],
                                             lsc_prm->mesh_r[box_idy + 1][box_idx], lsc_prm->mesh_r[box_idy + 1][box_idx + 1],
                                             x_coff, y_coff);
                break;
            case PixelCfaTypes::GR:
                gain = BilinearInterpolation(lsc_prm->mesh_gr[box_idy][box_idx], lsc_prm->mesh_gr[box_idy][box_idx + 1],
                                             lsc_prm->mesh_gr[box_idy + 1][box_idx], lsc_prm->mesh_gr[box_idy + 1][box_idx + 1],
                                             x_coff, y_coff);
                break;
            case PixelCfaTypes::GB:
                gain = BilinearInterpolation(lsc_prm->mesh_gb[box_idy][box_idx], lsc_prm->mesh_gb[box_idy][box_idx + 1],
                                             lsc_prm->mesh_gb[box_idy + 1][box_idx], lsc_prm->mesh_gb[box_idy + 1][box_idx + 1],
                                             x_coff, y_coff);
                break;
            case PixelCfaTypes::B:
                gain = BilinearInterpolation(lsc_prm->mesh_b[box_idy][box_idx], lsc_prm->mesh_b[box_idy][box_idx + 1],
                                             lsc_prm->mesh_b[box_idy + 1][box_idx], lsc_prm->mesh_b[box_idy + 1][box_idx + 1],
                                             x_coff, y_coff);
                break;
            default:
                break;
            }

            raw32_out[pixel_idx] = static_cast<int>(raw32_in[pixel_idx] * gain);
                     
            ClipMinMax<int32_t>(raw32_out[pixel_idx], (int32_t)isp_prm->info.max_val, 0);
        }
    }

    SwapMem<void>(frame->data.raw_s32_i, frame->data.raw_s32_o);
    
    return 0;
}

void RegisterLscMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::RAW;
    mod.out_domain = ColorDomains::RAW;

    mod.name = MOD_NAME;
    mod.run_function = Lsc;

    RegisterIspModule(mod);
}