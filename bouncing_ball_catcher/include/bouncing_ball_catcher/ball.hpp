#pragma once
#include <opencv2/opencv.hpp>
#include <random>

class Ball {
public:
  Ball(int w, int h, int r, double speed) : radius_(r) { respawn(w, h, speed); }

  void respawn(int w, int h, double speed) {
    static std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> ux(radius_, w - radius_), uy(radius_, h - radius_),
        ang(0.0, 6.2831853), col(60, 255);
    x_ = ux(rng); y_ = uy(rng);
    double a = ang(rng);
    vx_ = speed * std::cos(a); vy_ = speed * std::sin(a);
    color_ = cv::Scalar(col(rng), col(rng), col(rng));
    hold_ = 0.0;
  }

  void update(double dt, int w, int h) {          
    x_ += vx_ * dt; y_ += vy_ * dt;
    if (x_ < radius_)     { x_ = radius_;     vx_ = -vx_; }
    if (x_ > w - radius_) { x_ = w - radius_; vx_ = -vx_; }
    if (y_ < radius_)     { y_ = radius_;     vy_ = -vy_; }
    if (y_ > h - radius_) { y_ = h - radius_; vy_ = -vy_; }
  }

  double distanceTo(const cv::Point2f& p) const { return std::hypot(p.x - x_, p.y - y_); }
  bool contains(const cv::Point2f& p) const { return distanceTo(p) <= radius_; }
  void addHold(double dt) { hold_ += dt; }
  void resetHold() { hold_ = 0.0; }
  double hold() const { return hold_; }
  cv::Point center() const { return {(int)x_, (int)y_}; }
  int radius() const { return radius_; }
  cv::Scalar color() const { return color_; }

private:
  double x_, y_, vx_, vy_, hold_;
  int radius_;
  cv::Scalar color_;
};