void onImage(const sensor_msgs::msg::Image::SharedPtr msg) {
  cv::Mat frame = cv_bridge::toCvCopy(msg, "bgr8")->image;

  rclcpp::Time t(msg->header.stamp);
  double dt = have_last_ ? (t - last_t_).seconds() : 0.0;
  dt = std::clamp(dt, 0.0, 0.2);          
  last_t_ = t; have_last_ = true;

  cv::Point2f c; cv::Rect box;
  bool found = detect(frame, c, box);

  for (auto& b : balls_) b.update(dt, frame.cols, frame.rows);

  int best = -1; double bestD = 1e9;
  if (found)
    for (size_t i = 0; i < balls_.size(); ++i) {
      double d = balls_[i].distanceTo(c);
      if (balls_[i].contains(c) && d < bestD) { best = i; bestD = d; }
    }

  for (size_t i = 0; i < balls_.size(); ++i) {
    if ((int)i == best) {
      balls_[i].addHold(dt);
      if (balls_[i].hold() >= hold_duration_) {
        score_++; publishScore();
        balls_[i].respawn(frame.cols, frame.rows, ball_speed_);
      }
    } else balls_[i].resetHold();       
  }

}