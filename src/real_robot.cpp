#include "diffdrive_arduino/real_robot.h"


#include "hardware_interface/types/hardware_interface_type_values.hpp"




RealRobot::RealRobot()
    : 
    // arduino_(cfg.device, cfg.baud_rate, cfg.timeout),
    arduino_("/dev/ttyUSB0", 57600, 1000),
    // l_wheel_(cfg.left_wheel_name, cfg.enc_counts_per_rev),
    // r_wheel_(cfg.right_wheel_name, cfg.enc_counts_per_rev),
    // l_wheel_("l_wheel",1920),
    // r_wheel_("r_wheel",1920)
    l_wheel_("base_to_left_wheel",1920),
    r_wheel_("base_to_right_wheel",1920)
    // clk(Clk)
{

  time_ = std::chrono::system_clock::now();

  // loop_rate_ = cfg.loop_rate;
  loop_rate_ = 30;


}





return_type RealRobot::configure(const hardware_interface::HardwareInfo & info)
{
  if (configure_default(info) != return_type::OK) {
    return return_type::ERROR;
  }



  status_ = hardware_interface::status::CONFIGURED;
  return return_type::OK;
}

std::vector<hardware_interface::StateInterface> RealRobot::export_state_interfaces()
{
  // We need to set up a position and a velocity interface for each wheel

  std::vector<hardware_interface::StateInterface> state_interfaces;

  state_interfaces.emplace_back(hardware_interface::StateInterface(l_wheel_.name, hardware_interface::HW_IF_VELOCITY, &l_wheel_.vel));
  state_interfaces.emplace_back(hardware_interface::StateInterface(l_wheel_.name, hardware_interface::HW_IF_POSITION, &l_wheel_.pos));
  state_interfaces.emplace_back(hardware_interface::StateInterface(r_wheel_.name, hardware_interface::HW_IF_VELOCITY, &r_wheel_.vel));
  state_interfaces.emplace_back(hardware_interface::StateInterface(r_wheel_.name, hardware_interface::HW_IF_POSITION, &r_wheel_.pos));

  return state_interfaces;
}

std::vector<hardware_interface::CommandInterface> RealRobot::export_command_interfaces()
{
  // We need to set up a velocity command interface for each wheel

  std::vector<hardware_interface::CommandInterface> command_interfaces;

  command_interfaces.emplace_back(hardware_interface::CommandInterface(l_wheel_.name, hardware_interface::HW_IF_VELOCITY, &l_wheel_.cmd));
  command_interfaces.emplace_back(hardware_interface::CommandInterface(r_wheel_.name, hardware_interface::HW_IF_VELOCITY, &r_wheel_.cmd));

  return command_interfaces;
}


return_type RealRobot::start()
{

  arduino_.sendEmptyMsg();
  // arduino.setPidValues(9,7,0,100);
  // arduino.setPidValues(14,7,0,100);
  arduino_.setPidValues(30, 20, 0, 100);


  status_ = hardware_interface::status::STARTED;

  return return_type::OK;
}

return_type RealRobot::stop()
{
  status_ = hardware_interface::status::STOPPED;

  return return_type::OK;
}

hardware_interface::return_type RealRobot::read()
{

  // TODO fix chrono duration

  // Calculate time delta
  auto new_time = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = new_time - time_;
  double deltaSeconds = diff.count();
  time_ = new_time;


  if (!arduino_.connected())
  {
    return return_type::ERROR;
  }

  arduino_.readEncoderValues(l_wheel_.enc, r_wheel_.enc);

  double pos_prev = l_wheel_.pos;
  l_wheel_.pos = l_wheel_.calcEncAngle();
  l_wheel_.vel = (l_wheel_.pos - pos_prev) / deltaSeconds;

  pos_prev = r_wheel_.pos;
  r_wheel_.pos = r_wheel_.calcEncAngle();
  r_wheel_.vel = (r_wheel_.pos - pos_prev) / deltaSeconds;



  return return_type::OK;

  
}

hardware_interface::return_type RealRobot::write()
{

  if (!arduino_.connected())
  {
    return return_type::ERROR;
  }

  arduino_.setMotorValues(l_wheel_.cmd / l_wheel_.rads_per_count / loop_rate_, r_wheel_.cmd / r_wheel_.rads_per_count / loop_rate_);




  return return_type::OK;


  
}



#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(
  RealRobot,
  hardware_interface::SystemInterface
)