/**
 * @file cns.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief chrome noise signal filter
 * @version 0.1
 * @date 2023-08-27
 * Copyright (c) of ADAS_EYES 2023
 */

#include "modules/modules.h"

#define MOD_NAME "cns"

static int Cns(Frame *frame, const IspPrms *isp_prm)
{
    if ((frame == nullptr) || (isp_prm == nullptr))
    {
        LOG(ERROR) << "input prms is null";
        return -1;
    }
    int pixel_idx = 0;

    uint8_t *u_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.u);
    uint8_t *v_i = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_i.v);
    uint8_t *u_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.u);
    uint8_t *v_o = reinterpret_cast<uint8_t *>(frame->data.yuv_u8_o.v);

    uint8_t u[25];
    uint8_t v[25];

    FOR_ITER(ih, frame->info.height)
    {
        FOR_ITER(iw, frame->info.width)
        {
            int pixel_idx = ih * frame->info.width + iw;
            if ((iw < 2) || (iw >= (frame->info.width - 2)) || (ih < 2) || (ih >= (frame->info.height - 2))) {
                u_o[pixel_idx] = u_i[pixel_idx];
                v_o[pixel_idx] = v_i[pixel_idx];
                continue;
            }

            int sub_index = 0;
            for (int idy = -2; idy < 2; ++idy) {
                for (int idx = -2; idx < 2; ++idx) {
                    int filer_pixel_idx = GET_PIXEL_INDEX((iw + idx), (ih + idy), frame->info.width);
                    // it is too slow!!!
                    u[sub_index] = u_i[filer_pixel_idx];
                    v[sub_index] = v_i[filer_pixel_idx];
                    ++sub_index;
                }
            }
            std::sort(u, u + 25);
            std::sort(v, v + 25);
                
            u_o[pixel_idx] = u[12];
            v_o[pixel_idx] = v[12];
        }
    }

    SwapMem<void>(frame->data.yuv_u8_i.u, frame->data.yuv_u8_o.u);
    SwapMem<void>(frame->data.yuv_u8_i.v, frame->data.yuv_u8_o.v);

    return 0;
}

void RegisterCnsMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::YUV;
    mod.out_domain = ColorDomains::YUV;

    mod.name = MOD_NAME;
    mod.run_function = Cns;

    RegisterIspModule(mod);
}