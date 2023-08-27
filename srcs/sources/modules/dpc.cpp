/**
 * @file dpc.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-27
 * 
 * Copyright (c) of ADAS_EYES 2023
 * 
 */


#include "modules/modules.h"

#define MOD_NAME "dpc"

static int Dpc(Frame *frame, const IspPrms *isp_prm)
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

    int thres = isp_prm->dpc_prms.thres;
    auto mode = isp_prm->dpc_prms.mode;

    FOR_ITER(ih, frame->info.height)
    {
        FOR_ITER(iw, frame->info.width)
        {

            if ((iw < 2) || iw > (frame->info.width - 1) || (ih < 2) || (ih > (frame->info.height - 2))) {
                continue;
            }

            int pixel_idx = GET_PIXEL_INDEX(iw, ih, frame->info.width);

            /*
             p1 x  p2 x  p3
             x  x  x  x  x
             p4 x  p0 x  p5
             x  x  x  x  x
             p6 x  p7 x  p8
            */

           int p0 = raw32_in[GET_PIXEL_INDEX(iw      , ih      , frame->info.width)];
           int p1 = raw32_in[GET_PIXEL_INDEX((iw - 2), (ih - 2), frame->info.width)];
           int p2 = raw32_in[GET_PIXEL_INDEX(iw      , (ih - 2), frame->info.width)];
           int p3 = raw32_in[GET_PIXEL_INDEX((iw + 2), (ih - 2), frame->info.width)];
           int p4 = raw32_in[GET_PIXEL_INDEX((iw - 2), ih      , frame->info.width)];
           int p5 = raw32_in[GET_PIXEL_INDEX((iw + 2), ih      , frame->info.width)];
           int p6 = raw32_in[GET_PIXEL_INDEX((iw - 2), (ih + 2), frame->info.width)];
           int p7 = raw32_in[GET_PIXEL_INDEX(iw      , (ih + 2), frame->info.width)];
           int p8 = raw32_in[GET_PIXEL_INDEX((iw + 2), (ih + 2), frame->info.width)];
            
            if ((abs(p1 - p0) > thres) && (abs(p2 - p0) > thres) && (abs(p3 - p0) > thres) && \
                (abs(p4 - p0) > thres) && (abs(p5 - p0) > thres) && (abs(p6 - p0) > thres) && \
                (abs(p7 - p0) > thres) && (abs(p8 - p0) > thres)) {
                if (mode == DpcMode::MEAN) {
                    raw32_out[pixel_idx] = (p2 + p4 + p5 + p7) >> 2;
                } else { 
                    //use gradient
                    int dv = abs(2 * p0 - p2 - p7);
                    int dh = abs(2 * p0 - p4 - p5);
                    int ddl = abs(2 * p0 - p1 - p8);
                    int ddr = abs(2 * p0 - p3 - p6);

                    int dvh_min = min(dv, dh);
                    int dlr_min = min(ddl, ddr);

                    int min_val = min(dvh_min, dlr_min);
                    //choose the fat border pixel and get mean of them
                    if (min_val == dv) {
                        raw32_out[pixel_idx] = (p2 + p7 + 1) >> 1;
                    } else if (min_val == dh) {
                        raw32_out[pixel_idx] = (p4 + p5 + 1) >> 1;
                    } else if (min_val == ddl) {
                        raw32_out[pixel_idx] = (p1 + p8 + 1) >> 1;
                    } else {
                        raw32_out[pixel_idx] = (p3 + p6 + 1)  >> 1;
                    }
                }         
            } else {
                raw32_out[pixel_idx] = static_cast<int>(raw32_in[pixel_idx]);
            }
        }
    }

    SwapMem<void>(frame->data.raw_s32_i, frame->data.raw_s32_o);
    
    return 0;
}

void RegisterDpcMod()
{
    IspModule mod;

    mod.in_type = DataPtrTypes::TYPE_INT32;
    mod.out_type = DataPtrTypes::TYPE_INT32;

    mod.in_domain = ColorDomains::RAW;
    mod.out_domain = ColorDomains::RAW;

    mod.name = MOD_NAME;
    mod.run_function = Dpc;

    RegisterIspModule(mod);
}