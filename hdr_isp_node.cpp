#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include "EasyBMP.h"



#if ROS2_SUPPORT

#include <chrono>
#include <iostream>
#include <memory>

#include "rclcpp/clock.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/string.hpp"
#include "common/pipeline.h"

using std::placeholders::_1;

class IspHandleNode : public rclcpp::Node
{
public:
    IspHandleNode(const std::string topic, Frame& frame, IspPrms& isp_prm)
    : Node("IspHandleNode")
    {
        frame_ = &frame;
        isp_prms_ = &isp_prm; 
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            topic, 10, std::bind(&IspHandleNode::topic_callback, this, _1));  
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/img/isp_result", 10);  
        //size_t size = isp_prm.info.width * 2 * isp_prm.info.height;
        isp_pipe_ = std::move(GetIspPipeImplFromDevice(isp_prm.device, isp_prm.pipe));
        if (!isp_pipe_->IsPipeVaild()) {
            LOG(ERROR) << "pipeline is invailed";
            return;
        }
        auto size = isp_prm.info.width * 3 * isp_prm.info.height;
        pub_img_ = std::make_shared<sensor_msgs::msg::Image>();
        pub_img_->data.resize(size);
        isp_pipe_->LoadPrms(&isp_prm);
        clock_ = std::make_shared<rclcpp::Clock>(RCL_ROS_TIME);
    }   
    ~IspHandleNode() {
      //isp_handle_thread_.join();
    };

private:
    Frame* frame_ = nullptr;
    IspPrms* isp_prms_ = nullptr;
    std::unique_ptr<IspPipeline> isp_pipe_;
    //TSQueue<IspSubImage> sub_queue_;
    //boost::lockfree::queue<IspSubImage> sub_queue_;
    std::thread isp_handle_thread_;

private:
    void topic_callback(const sensor_msgs::msg::Image::SharedPtr img)
    {
        //RCLCPP_INFO(this->get_logger(), "I heard image: code %s, width %d, height %d",  img->encoding, img->width, img->height);
        if (img->width != isp_prms_->info.width || img->height != isp_prms_->info.height) {
            RCLCPP_INFO(this->get_logger(), "input size is not equal with sub");
            return;
        }
        RCLCPP_INFO(this->get_logger(), "hdr isp get image, start handle");
        //handle data
        auto start_run_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        frame_->CpMemToFrame(&img->data[0], isp_prms_->info.height * isp_prms_->info.width * 2, isp_prms_->device);
        isp_pipe_->RunPipe(frame_, isp_prms_);
        auto end_run_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        float fps = 1000.0 / (end_run_tick - start_run_tick).count();
        RCLCPP_INFO(this->get_logger(), "handle finished, real fps %f, pub reuslt frame", fps);
        //std::unique_ptr<sensor_msgs::msg::Image> pub_img = std::make_unique<sensor_msgs::msg::Image>();
        pub_img_->is_bigendian = false;
        pub_img_->height = img->height;
        pub_img_->width = img->width;
        pub_img_->encoding = "bgr8";//mat_type2encoding(frame.type());
        pub_img_->step = static_cast<sensor_msgs::msg::Image::_step_type>(pub_img_->width * 3);//static_cast<sensor_msgs::msg::Image::_step_type>(img_msg_->width * 2);//msg.step = static_cast<sensor_msgs::msg::Image::_step_type>(frame.step);
        size_t size = pub_img_->width * 3 * pub_img_->height;
        pub_img_->data.resize(size);
        pub_img_->header.frame_id = "hdr_isp";
        pub_img_->header.stamp = this->now();//clock_->now();
        memcpy(&pub_img_->data[0], frame_->data.bgr_u8_o->addr, size);
        //publisher_->publish(std::move(pub_img));  
        publisher_->publish(*pub_img_);  
        RCLCPP_INFO(this->get_logger(), "pubfinished wait new frame....\r\n");
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
    rclcpp::Clock::SharedPtr clock_;
    std::shared_ptr<sensor_msgs::msg::Image> pub_img_;
  // do stuff...
};


HdrIspErrCode RosIspOnlineRun(int argc, char** argv, Frame& frame, IspPrms& isp_prm)
{
    LOG(INFO) << "========APP RUN ONLINE MODE(ROS2)=======";
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<IspHandleNode>(isp_prm.topic, frame, isp_prm));
    rclcpp::shutdown();
    return HdrIspErrCode::SUCCESS;
}

#else
#include "common/pipeline.h"

HdrIspErrCode RosIspOnlineRun(int argc, char** argv, Frame& frame, IspPrms& isp_prm)
{
    return HdrIspErrCode::ERR_UNIMPLEMENT;
}

#endif