#ifndef HDR_ISP_DEVICE_H
#define HDR_ISP_DEVICE_H

#include "types.h"
#include <cuda_runtime.h>

#define CUDA_CHECK_ERR(err, str) do { \
    if (err != cudaSuccess) \
	{ \
		fprintf(stderr, "[%s][%s][%d]cuda check failed (error code %s)!\n", __FUNCTION__, str, __LINE__, cudaGetErrorString(err)); \
		exit(EXIT_FAILURE); \
	} \
} while(0)

static __constant__ PixelCfaTypes kGpuPixelCfaLut[kCfaNums][2][2] = {
    {
        {PixelCfaTypes::R, PixelCfaTypes::GR},
        {PixelCfaTypes::GB, PixelCfaTypes::B},
    },
    {
        {PixelCfaTypes::GR, PixelCfaTypes::R},
        {PixelCfaTypes::B, PixelCfaTypes::GB},
    },
    {
        {PixelCfaTypes::B, PixelCfaTypes::GB},
        {PixelCfaTypes::GR, PixelCfaTypes::R},
    },
    {
        {PixelCfaTypes::GB, PixelCfaTypes::B},
        {PixelCfaTypes::R, PixelCfaTypes::GR},
    }
};


static Memory* GpuMemMalloc(int size)
{
    Memory* mem = nullptr;
    mem = new Memory;
    size = (size + 255) & (~0x0000ff);
    mem->size= size;
    auto err = cudaMalloc((void **)(&(mem->addr)), size);
    //LOG(INFO) << "cuda malloc " << size;
	CUDA_CHECK_ERR(err, "cudaMalloc");
	err = cudaMemset(mem->addr, 0, size);
	CUDA_CHECK_ERR(err, "cudaMemset");

    return mem;
}

static void GpuMemFree(Memory* mem)
{
    if (!mem) return;

    if (mem->addr) {
        auto err = cudaFree(mem->addr);
	    CUDA_CHECK_ERR(err, "cudaFree");
    }
    free(mem);
    mem = nullptr;
}


static Memory* CpuMemAlloc(int size)
{
    Memory* mem = nullptr;
    mem = new Memory;
    mem->size= size;
    mem->addr = new char[size];
    return mem;
}

static void CpuMemFree(Memory* mem)
{
    if (!mem) return;

    if (mem->addr) {
        free(mem->addr);
    }
    free(mem);
    mem = nullptr;
}


#endif