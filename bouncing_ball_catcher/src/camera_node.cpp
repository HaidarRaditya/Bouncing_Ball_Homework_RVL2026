#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include <cv_bridge/cv_bridge.hpp>   
#include <opencv2/opencv.hpp>

class CameraNode : public rclcpp::Node {
public:
  CameraNode() : Node("camera_node") {

    declare_parameter("source", "0");
    declare_parameter("fps", 60.0);       
    declare_parameter("width", 640);      
    declare_parameter("height", 480);     

    std::string src = get_parameter("source").as_string();
    double fps = get_parameter("fps").as_double();
    w_ = get_parameter("width").as_int();
    h_ = get_parameter("height").as_int();

    is_file_ = !std::all_of(src.begin(), src.end(), ::isdigit);   

    if (is_file_) {
      cap_.open(src);
    } else {
      cap_.open(std::stoi(src), cv::CAP_V4L2); 
    }

    if (!cap_.isOpened()) {
      RCLCPP_ERROR(get_logger(), "Gagal membuka sumber: %s", src.c_str());
    } else {

      cap_.set(cv::CAP_PROP_FRAME_WIDTH, w_);
      cap_.set(cv::CAP_PROP_FRAME_HEIGHT, h_);
      cap_.set(cv::CAP_PROP_FPS, fps);
    }

   
    pub_ = create_publisher<sensor_msgs::msg::Image>("/camera/image_raw", rclcpp::SensorDataQoS());
    
    timer_ = create_wall_timer(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(1.0 / fps)),
        std::bind(&CameraNode::tick, this));
  }

private:
  void tick() {
    cv::Mat frame;
    if (!cap_.read(frame) || frame.empty()) {
      if (is_file_) cap_.set(cv::CAP_PROP_POS_FRAMES, 0);   
      return;
    }

    if (frame.cols != w_ || frame.rows != h_) {
      cv::resize(frame, frame, cv::Size(w_, h_));
    }

    std_msgs::msg::Header hdr;
    hdr.stamp = now();                       
    hdr.frame_id = "camera";
    pub_->publish(*cv_bridge::CvImage(hdr, "bgr8", frame).toImageMsg());
  }

  cv::VideoCapture cap_; 
  bool is_file_; 
  int w_, h_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CameraNode>());
  rclcpp::shutdown();
}