# Image Signal Process For HDR CMOS Image Sensor

**知乎主页：[ADAS之眼](https://www.zhihu.com/people/fen-shi-qing-nian-29) | 个人网站：[ADAS之眼](https://jokereyeadas.github.io/) | 公众号：**

**![公众号](docs/wechat.png)**


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

# How To Build

## Linux
```bash
git clone https://github.com/JokerEyeAdas/HDRImageSignalProcess
cd HDRImageSignalProcess/
mkdir build
cmake ..
make
```
## Windows

**Develop Env (x64):** 
- vs code
- cmake
- vs2019 c++ gen tool

![build tool](docs/compile.png) 

```bash
git clone https://github.com/JokerEyeAdas/HDRImageSignalProcess
code HDRImageSignalProcess
#cmake choose Debug or Release
#compiler choose xxx-amd64
#build all
```

# Result

## Final Result

|RAW Image|ISP Result(Ours)|FastOpenISP Result|
|-------|-------|-------|
|![raw](/docs/ISP/connan_raw14.png)|![ISP](docs/ISP/isp_result.png)|![Fast ISP](docs/ISP/color_checker.png)|

------

|Compare Item|Our ISP|Notes|FastOpenISP|Notes|
|-------|-------|-------|-------|-------|
|Detail|![Our Detail](docs/ISP/our_detail.png)|Detail is Clear|![Open Isp](docs/ISP/fast_detail.png)|Color Banding and Detail lost|


# Project Support(项目支持)

![Appreciation Code](docs/AppreciationCode.png)


# Reference Repo

Thanks for the following code repository!

|Index|Repo|Notes|
|-----|-----|-----|
|1|[fast-openIsp](https://github.com/QiuJueqin/fast-openISP)|open isp 快速版本|
|2|[ISP Lab](https://github.com/yuqing-liu-dut/ISPLab)|Isp realized by c++|

