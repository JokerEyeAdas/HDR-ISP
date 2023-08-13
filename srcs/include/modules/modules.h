#ifndef ISP_MODS_H
#define ISP_MODS_H

#include <list>
#include <functional>
#include "common/frame.h"
#include "common/common.h"
#include <cmath>

struct IspPrms
{
    std::string raw_file;
    std::string out_file_path;

    std::string sensor_name;
    int blc = 0;
    bool data_packed;
    std::list<std::string> pipe;
    ImageInfo info;
    DePwlPrms depwl_prm;
    CcmPrms ccm_prms;
    WbGain wb_gains;
    GammmaCurve y_gamma;
    GammmaCurve rgb_gamma;
    LtmPrms ltm_prms;
    SaturationPrms sat_prms;
    ContrastPrms contrast_prms;
    SharpenPrms sharpen_prms;
};

struct IspModule
{
    std::string name;

    DataPtrTypes in_type;
    DataPtrTypes out_type;
    ColorDomains in_domain;
    ColorDomains out_domain;

    std::function<int(Frame *, const IspPrms *)> run_function;
};

int RegisterIspModule(IspModule mod);
int GetIspModuleFromName(std::string, IspModule &mod);
int ShowAllIspModules();

/**
 * @brief register isp module instance
 */

void RegisterUnpackMod();
void RegisterDePwlMod();
void RegisterBlcMod();
void RegisterDemoasicMod();
void RegisterCcmMod();
void RegisterYGammaMod();
void RegisterWbGaincMod();
void RegisterLtmMod();
void RegisterRgbGammaMod();
void RegisterYuv2RgbMod();
void RegisterRgb2YuvMod();
void RegisterSaturationMod();
void RegisterContrastMod();
void RegisterSharpenMod();

#endif