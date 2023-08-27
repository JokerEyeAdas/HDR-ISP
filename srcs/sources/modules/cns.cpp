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


    constexpr int boundary_pixel = 1;
    constexpr int filter_size = (2 * boundary_pixel + 1) * (2 * boundary_pixel + 1);
    constexpr int filter_center = filter_size >> 1;
    uint8_t u[filter_size];
    uint8_t v[filter_size];

    FOR_ITER(ih, frame->info.height)
    {
        FOR_ITER(iw, frame->info.width)
        {
            int pixel_idx = ih * frame->info.width + iw;
            if ((iw < boundary_pixel) || (iw >= (frame->info.width - boundary_pixel)) || (ih < boundary_pixel) || (ih >= (frame->info.height - boundary_pixel))) {
                u_o[pixel_idx] = u_i[pixel_idx];
                v_o[pixel_idx] = v_i[pixel_idx];
                continue;
            }

            int sub_index = 0;
            for (int idy = -boundary_pixel; idy <= boundary_pixel; ++idy) {
                for (int idx = -boundary_pixel; idx <= boundary_pixel; ++idx) {
                    int filer_pixel_idx = GET_PIXEL_INDEX((iw + idx), (ih + idy), frame->info.width);
                    u[sub_index] = u_i[filer_pixel_idx];
                    v[sub_index] = v_i[filer_pixel_idx];
                    ++sub_index;
                }
            }
            //meida filter
            std::sort(u, u + filter_size);
            std::sort(v, v + filter_size);
                
            u_o[pixel_idx] = u[filter_center];
            v_o[pixel_idx] = v[filter_center];
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