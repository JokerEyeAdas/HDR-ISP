#ifndef HDR_ISP_MODULES_H
#define HDR_ISP_MODULES_H

#include <list>
#include <functional>
#include "common/frame.h"
#include "common/common.h"


struct IspPrms
{
    AppMode    mode;
    DeviceType device;
    std::string topic;
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
    LscPrms lsc_prms;
    DpcPrms dpc_prms;
};

struct IspModule
{
    std::string name;

    DataPtrTypes in_type;
    DataPtrTypes out_type;
    ColorDomains in_domain;
    ColorDomains out_domain;

    std::function<HdrIspErrCode(Frame *, const IspPrms *)> run_function;
    std::function<HdrIspErrCode(Frame *, const IspPrms *, cudaStream_t stream)> gpu_run_function;
    std::function<HdrIspErrCode(const IspPrms *)> gpu_load_prm_function;


};

int RegisterIspModule(IspModule mod, DeviceType dev_type = DeviceType::HOST_CPU);
int GetIspModuleFromName(std::string name, IspModule &mod, DeviceType dev_type  = DeviceType::HOST_CPU);
int ShowAllIspModules(DeviceType dev_type = DeviceType::HOST_CPU);

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
void RegisterLscMod();
void RegisterDpcMod();
void RegisterDpcMod();
void RegisterCnsMod();
void RegisterPwlMod();

#endif