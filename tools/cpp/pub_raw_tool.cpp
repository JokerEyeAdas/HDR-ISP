//#include <functional>
#include <stdio.h>
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

#if ROS2_SUPPORT

#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "common/types.h"

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

using namespace std::chrono_literals;

class ImgPubNode : public rclcpp::Node
{
public:
  ImgPubNode(const std::string topic, char* mem, int len, int width, int height)
  : Node("ImgPubNode")
  {    
    data_ = mem;
    len_ = len;
    width_ = width;
    height_ = height;
    
    publisher_ = this->create_publisher<sensor_msgs::msg::Image>(topic, 10);
    timer_ = create_wall_timer(200ms, std::bind(&ImgPubNode::timer_callback, this)); 
  }

private:
    std::unique_ptr<sensor_msgs::msg::Image> img_msg_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    char* data_ = nullptr;
    int len_ = 0;
    int width_;
    int height_;

private:
  void timer_callback()
  {
    img_msg_ = std::make_unique<sensor_msgs::msg::Image>();
    img_msg_->is_bigendian = false;
    img_msg_->height = height_;
    img_msg_->width = width_;
    img_msg_->encoding = "mono16";//mat_type2encoding(frame.type());
    img_msg_->step = static_cast<sensor_msgs::msg::Image::_step_type>(img_msg_->width * 2);//msg.step = static_cast<sensor_msgs::msg::Image::_step_type>(frame.step);
    size_t size = img_msg_->width * 2 * img_msg_->height;
    img_msg_->data.resize(size);
    memcpy(&img_msg_->data[0], data_, size);
    img_msg_->header.frame_id = "connan_raw14";
    RCLCPP_INFO(this->get_logger(), "I pub image: width %d, height %d", img_msg_->width, img_msg_->height);
    img_msg_->header.stamp = this->now();
    publisher_->publish(std::move(img_msg_));
  }
};

HdrIspErrCode ParseSensorInfo(const std::string cfg_file_path, IspPrms &isp_prm)
{

    std::ifstream fs(cfg_file_path);

    if (!fs.is_open())
    {
        std::cout << cfg_file_path << " open failed";
        return HdrIspErrCode::ERR_FILE_INVAILD;
    }

    json j_root;
    fs >> j_root;

    isp_prm.topic = j_root["topic"];
    
    // raw path
    isp_prm.raw_file = j_root["raw_file"];
    isp_prm.out_file_path = j_root["out_file_path"];
    // sensor info
    isp_prm.sensor_name = j_root["info"]["sensor_name"];
    std::cout << "Sensor Name: " << isp_prm.sensor_name;

    auto cfa_str = j_root["info"]["cfa"];
    if (cfa_str == "RGGB")
    {
        isp_prm.info.cfa = CfaTypes::RGGB;
    }
    else if (cfa_str == "BGGR")
    {
        isp_prm.info.cfa = CfaTypes::BGGR;
    }
    else if (cfa_str == "GBRG")
    {
        isp_prm.info.cfa = CfaTypes::GBRG;
    }
    else if (cfa_str == "GRBG")
    {
        isp_prm.info.cfa = CfaTypes::GRBG;
    }
    std::cout << "Sensor CFA: " << cfa_str;

    auto raw_type_str = j_root["info"]["data_type"];
    if (raw_type_str == "RAW10")
    {
        isp_prm.info.dt = RawDataTypes::RAW10;
    }
    else if (raw_type_str == "RAW12")
    {
        isp_prm.info.dt = RawDataTypes::RAW12;
    }
    else if (raw_type_str == "RAW14")
    {
        isp_prm.info.dt = RawDataTypes::RAW14;
    }
    else if (raw_type_str == "RAW16")
    {
        isp_prm.info.dt = RawDataTypes::RAW16;
    }
    std::cout << "Sensor DT: " << raw_type_str;

    isp_prm.info.bpp = static_cast<int>(j_root["info"]["bpp"]);
    int max_bit = int(j_root["info"]["max_bit"]);
    isp_prm.info.max_val = (1 << max_bit) - 1;
    isp_prm.info.width = static_cast<int>(j_root["info"]["width"]);
    isp_prm.info.height = static_cast<int>(j_root["info"]["height"]);
    isp_prm.info.mipi_packed = static_cast<int>(j_root["info"]["mipi_packed"]);
    std::cout << "Sensor Resolution: " << isp_prm.info.width << "*" << isp_prm.info.height;
    fs.close();
    std::cout << "parse exit";
    return HdrIspErrCode::SUCCESS;
}

size_t ReadFileToMem(std::string file_name, void* mem, int size)
{
    FILE* fp = nullptr;

    fp = fopen(file_name.c_str(), "rb");

    if (!fp) {
        std::cout << file_name << " open failed";
        return -1;
    }

    auto rd = fread(mem, 1, size, fp);

    fclose(fp);

    return rd;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("no josn cfg input");
        return 0;
    }

    IspPrms isp_prm;

    auto ret = ParseSensorInfo(argv[1], isp_prm);
    if (ret != HdrIspErrCode::SUCCESS)
    {
        printf("josn cfg input [%s] parse failed", argv[1]);
        return -1;
    }

    char* mem = new char[isp_prm.info.width * isp_prm.info.height * 2];
    auto rd = ReadFileToMem(isp_prm.raw_file, mem, isp_prm.info.width * isp_prm.info.height * 2);
    
    std::cout << "rd " << rd << "\r\n";

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ImgPubNode>(isp_prm.topic, mem, \
                isp_prm.info.width * isp_prm.info.height * 2, isp_prm.info.width, isp_prm.info.height));
    rclcpp::shutdown();

    delete[] mem;
    return 0;
}

#else


int main(int argc, char** argv)
{
    printf("no ros2 support\r\n");
    return 0;
}

#endif