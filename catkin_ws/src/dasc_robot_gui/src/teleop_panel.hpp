#ifndef TELEOP_PANEL_HPP_
#define TELEOP_PANEL_HPP_

#ifndef Q_MOC_RUN
#include <memory>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rviz_common/panel.hpp"
#endif
#include <QRadioButton>
#include <QPushButton>

class QLineEdit;

namespace dasc_robot_gui
{


// BEGIN_TUTORIAL
// Here we declare our new subclass of rviz_common::Panel.  Every panel which
// can be added via the Panels/Add_New_Panel menu is a subclass of
// rviz_common::Panel.
//
// TeleopPanel will show a text-entry field to set the output topic
// and a 2D control area.  The 2D control area is implemented by the
// DriveWidget class, and is described there.
class TeleopPanel : public rviz_common::Panel
{
  // This class uses Qt slots and is a subclass of QObject, so it needs
  // the Q_OBJECT macro.
  Q_OBJECT

public:
  // QWidget subclass constructors usually take a parent widget
  // parameter (which usually defaults to 0).  At the same time,
  // pluginlib::ClassLoader creates instances by calling the default
  // constructor (with no arguments).  Taking the parameter and giving
  // a default of 0 lets the default constructor work and also lets
  // someone using the class for something else to pass in a parent
  // widget as they normally would with Qt.
  explicit TeleopPanel(QWidget * parent = 0);

  // Now we declare overrides of rviz_common::Panel functions for saving and
  // loading data from the config file.  Here the data is the
  // topic name.
  virtual void load(const rviz_common::Config & config);
  virtual void save(rviz_common::Config config) const;

// Next come a couple of public Qt slots.

public Q_SLOTS:
  // The control area, DriveWidget, sends its output to a Qt signal
  // for ease of re-use, so here we declare a Qt slot to receive it.
  //void setVel(float linear_velocity_, float angular_velocity_);

  // In this example setTopic() does not get connected to any signal
  // (it is called directly), but it is easy to define it as a public
  // slot instead of a private function in case it would be useful to
  // some other user.
  void setTopic(const QString & topic);

// Here we declare some internal slots.

protected Q_SLOTS:
  // sendvel() publishes the current velocity values to a ROS
  // topic.  Internally this is connected to a timer which calls it 10
  // times per second.
  void sendArmCmd(bool arm=false);

  // updateTopic() reads the topic name from the QLineEdit and calls
  // setTopic() with the result.
  void updateTopic();

// Then we finish up with protected member variables.

protected:
  // The control-area widget which turns mouse events into command
  // velocities.
  //DriveWidget * drive_widget_;

  // One-line text editor for entering the outgoing ROS topic name.
  QLineEdit * output_topic_editor_;

  // Armed radio button
  QPushButton *arm_button_;
  QPushButton *disarm_button_;

  // The current name of the output topic.
  QString output_topic_;

  // The ROS node and publisher for the command velocity.
  std::shared_ptr<rclcpp::Node> velocity_node_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr velocity_publisher_;

  // The latest velocity values from the drive widget.
  float linear_velocity_;
  float angular_velocity_;
  // END_TUTORIAL
};

}  // end namespace dasc_robot_gui

#endif  // TELEOP_PANEL_HPP_
