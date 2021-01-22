#ifndef REAL_ROBOT_H
#define REAL_ROBOT_H


#include "hardware_interface/base_interface.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "hardware_interface/types/hardware_interface_status_values.hpp"

#include "arduino_comms.h"
#include <cstring>
#include "wheel.h"

#include "rclcpp/rclcpp.hpp"

using hardware_interface::return_type;

class RealRobot : public hardware_interface::BaseInterface<hardware_interface::SystemInterface>
{

public:
  struct Config
  {
    std::string left_wheel_name = "left_wheel";
    std::string right_wheel_name = "right_wheel";
    float loop_rate = 30;
    std::string device = "/dev/ttyUSB0";
    int baud_rate = 57600;
    int timeout = 1000;
    int enc_counts_per_rev = 1920;
  };





public:
  RealRobot();

  return_type configure(const hardware_interface::HardwareInfo & info) override;

  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

  return_type start() override;

  return_type stop() override;

  
  return_type read() override;

  return_type write() override;



private:
  ArduinoComms arduino_;

  Wheel l_wheel_;
  Wheel r_wheel_;

  // rclcpp::Time time_;
  // rclcpp::Duration period_ = rclcpp::Duration(0,0);
  float loop_rate_;

  // rclcpp::Clock &clk;

  std::chrono::time_point<std::chrono::system_clock> time_;
  
};


#endif // REAL_ROBOT_H