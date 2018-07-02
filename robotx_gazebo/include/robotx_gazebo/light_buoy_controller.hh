#pragma once

#include <gazebo/gazebo.hh>
#include <std_msgs/ColorRGBA.h>
#include <std_srvs/Trigger.h>
#include <ros/ros.h>

/* Controls the RobotX light buoy (scan the code challenge)
 * Selects a random sequence of 3 colors selected from red, green, blue, and yellow
 * without consecutive repeats. This sequence is displayed simultaneously on the light buoy's
 * 3 panels, each for 1 second, followed by a second with the panels off. A new sequence
 * can be generated by calling a ROS service.
 */
class LightBuoyController : public gazebo::ModelPlugin
{
public:
  void Load(gazebo::physics::ModelPtr _parent, sdf::ElementPtr _sdf);
  static void init();
private:
  // Creates a std_msgs::ColorRGBA message from 4 doubles
  static std_msgs::ColorRGBA color(double r, double g, double b, double a);

  // The 4 colors (plus OFF) that can be displayed
  static const std_msgs::ColorRGBA RED;
  static const std_msgs::ColorRGBA GREEN;
  static const std_msgs::ColorRGBA BLUE;
  static const std_msgs::ColorRGBA YELLOW;
  static const std_msgs::ColorRGBA OFF;

  using colors_t = std::pair<std_msgs::ColorRGBA, std::string>;
  // List of the color options with their string name for logging
  static const colors_t COLORS[5];

  // Publisher to set color to each of the 3 panels using the gazebo_ros_color plugin
  ros::Publisher panel_pubs_[3];
  // Node handle
  ros::NodeHandle nh_;
  // Service to generate and display a new color sequence
  ros::ServiceServer change_pattern_server_;
  // Timer triggered every 1 second (in simulated time) to update the color
  ros::Timer timer_;
  // Track current index in pattern
  uint8_t state_ = 0;
  // Display the next color in the sequence, or start over if at the end
  void incrementState(const ros::TimerEvent& _event);
  // The current pattern to display, pattern_[3] is always OFF
  using pattern_t = std::array<uint8_t, 4>;
  pattern_t pattern_;
  // Generate a new pattern and reset state to OFF
  bool changePattern(std_srvs::Trigger::Request& req, std_srvs::Trigger::Response& res);
  void changePattern(std::string& new_pattern);
  // Locks state_ and pattern_
  std::mutex mutex_;
};
