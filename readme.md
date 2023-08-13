# Image Signal Process For HDR CMOS Image Sensor

**[CHINESE README](docs/readme_ch.md)**

-----

**Zhihu：[EYES OF ADAS](https://www.zhihu.com/people/fen-shi-qing-nian-29) | Blog：[EYES OF ADAS](https://jokereyeadas.github.io/) | WeChat：**

**![WeChat](docs/wechat.png)**

# Default Pipeline

![Pipeline](docs/pipeline.png)
# Support ISP Module Lists

- Raw Domain
- [x] MipiUnPack: mipi raw data unpack to raw16
- [x] DePwl: decode the pwl curve
- [ ] Dpc: dynamic pixel correct
- [ ] Lsc: lens shding correct
- [x] Blc: black level correct
- [ ] Rns: raw noise filter
- [x] WbGain: white balance gain
- [x] Demoasic: change raw to rgb
- RGB Domain
- [x] Ltm: local tone mapping
- [x] RgbGamma: rgb gamma curve
- [x] Ccm: color correct matrix
- [x] Rgb2Yuv: rgb domain to yuv domain
- YUV Domain
- [x] YGamma: gray gamma curve
- [x] Contrast: gray contrast adjust
- [x] Sharpen: gray usm sharpen
- [ ] Cns: chrome noise filter
- [x] Saturation: chrome saturation
- [x] Yuv2Rgb: yuv domain to rgb domain

# How To Build and Run

## Linux
**Develop Env :** 
```bash
#dependencies install(cmake and opencv)
sudo apt update
sudo apt install libopencv-dev python3-opencv cmake
```

**Build**

```bash
git clone https://github.com/JokerEyeAdas/HDRImageSignalProcess
cd HDRImageSignalProcess/
mkdir build
cmake ..
make -j12
```
## Windows

**Develop Env (x64):** 
- vs code
- cmake
- vs2019 c++ gen tool

![build tool](docs/compile.png) 

**Build**

```bash
git clone https://github.com/JokerEyeAdas/HDRImageSignalProcess
code HDRImageSignalProcess
#cmake choose Debug or Release
#compiler choose xxx-amd64
#build all
```
## How To Run

```bash
cd build
#cp cfgs and data
cp -r ../data/ ./
cp -r ../cfgs/ ./
#run isp
./HDR_ISP ./cfgs/isp_config_cannon.json
```

# Result

## ISP Result

|Notes|Image|
|-------|-------|
|Raw|![raw](/docs/ISP/connan_raw14.png)|
|ISP Result(Ours)|![ISP](docs/ISP/isp_result.png)|
|FastOpenISP Result|![Fast ISP](docs/ISP/color_checker.png)|

------

## Detail Compare

|ISP|Image|Notes|
|-------|-------|-------|
|Ours|![Our Detail](docs/ISP/our_detail.png)|Detail and boundaries is clear|
|Fast Open Isp|![Open Isp](docs/ISP/fast_detail.png)|Color banding and detail lost|
|Ours|![Our Detail](docs/ISP/our_sharpen.png)|Detail and boundaries is clear|
|Fast Open Isp|![Open Isp](docs/ISP/others_sharpen.png)|Color banding and detail lost|
# Project Support

- Thanks for your support, we hope that our project can help your work

![Appreciation Code](docs/AppreciationCode.png)

# Reference Repo

Thanks for the following code repository!

|Index|Repo|Language|Notes|
|-----|-----|------|-----|
|1|[fast-openIsp](https://github.com/QiuJueqin/fast-openISP)|Python|open isp speed up verison in python|
|2|[ISP Lab](https://github.com/yuqing-liu-dut/ISPLab)|C++|Isp realized by c++|
|3|[xk-ISP](https://github.com/openasic-org/xkISP)|C++|C++ ISP For HLS on FPGA|

-----

**Eyes of Adas Copyright Reserved @2023**