/**
 * @file pipeline.cpp
 * @author joker.mao (joker_mao@163.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-27
 * 
 * Copyright (c) of ADAS_EYES 2023
 * 
 */
#include "modules/modules.h"

void IspInit()
{
   RegisterUnpackMod();
   RegisterDePwlMod();
   RegisterBlcMod();
   RegisterDemoasicMod();
   RegisterCcmMod();
   RegisterYGammaMod();
   RegisterWbGaincMod();
   RegisterLtmMod();
   RegisterRgbGammaMod();
   RegisterYuv2RgbMod();
   RegisterRgb2YuvMod();
   RegisterSaturationMod();
   RegisterContrastMod();
   RegisterContrastMod();
   RegisterSharpenMod();
}