/**
 * @file pipeline.h
 * @author joker.mao (joker_mao@163.com)
 * @brief
 * @version 0.1
 * @date 2023-07-27
 *
 * Copyright (c) of ADAS_EYES 2023
 *
 */

#ifndef HDR_ISP_PIPE_H
#define HDR_ISP_PIPE_H

#include <list>
#include <functional>

#include "modules/modules.h"
#include <chrono>

class IspPipeline
{
private:
    /* data */
    std::list<IspModule> pipe_;
    bool is_pipe_vaild_ = false;

public:
    IspPipeline();
    IspPipeline(const std::list<std::string>& pipeline);
    virtual ~IspPipeline();

public:
    void SetPipeVaild(bool vaild);
    bool IsPipeVaild();
    int MakePipe(const std::list<std::string> &pipeline_str, DeviceType dev_type);
    int PrintPipe();
    auto GetPipeList() const { return pipe_;} ;

public:
    virtual int RunPipe(Frame *frame, const IspPrms *prms);
    virtual int LoadPrms(const IspPrms *prms) {(void *)prms; return 0;};
};


class IspPipelineCpuImpl : public IspPipeline
{
public:
    IspPipelineCpuImpl(const std::list<std::string> &pipe_list);
    IspPipelineCpuImpl();
    ~IspPipelineCpuImpl();
    int RunPipe(Frame *frame, const IspPrms *prms);
    int LoadPrms(const IspPrms *prms);
};


class IspPipelineGpuImpl : public IspPipeline
{
public:
    IspPipelineGpuImpl(const std::list<std::string> &pipe_list);
    IspPipelineGpuImpl();
    ~IspPipelineGpuImpl();
    int RunPipe(Frame *frame, const IspPrms *prms);
    int LoadPrms(const IspPrms *prms);
};

static std::unique_ptr<IspPipeline> GetIspPipeImplFromDevice(DeviceType dev_type, const std::list<std::string> &pipe_list) {
    switch (dev_type)
    {
    case DeviceType::HOST_CPU:
        return std::make_unique<IspPipelineCpuImpl>(pipe_list);
        break;
    case DeviceType::SLAVE_GPU:
        return std::make_unique<IspPipelineGpuImpl>(pipe_list);
        break;
    default:
        return std::make_unique<IspPipelineCpuImpl>(pipe_list);
        break;
    }
}



#endif // ! ISP_PIPE_H