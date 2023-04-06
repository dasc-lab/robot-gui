#include "teleop_panel.hpp"

#include <stdio.h>
#include <QPainter>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QRadioButton>
#include <QPushButton>

#include <memory>

#include "geometry_msgs/msg/twist.hpp"
#include "pluginlib/class_list_macros.hpp"

//#include "drive_widget.hpp"

namespace dasc_robot_gui
{
// Here is the implementation of the TeleopPanel class.  TeleopPanel
// has these responsibilities:
//
// - Act as a container for GUI elements DriveWidget and QLineEdit.
// - Publish command velocities 10 times per second (whether 0 or not).
// - Saving and restoring internal state from a config file.
//
// We start with the constructor, doing the standard Qt thing of
// passing the optional *parent* argument on to the superclass
// constructor, and also zero-ing the velocities we will be
// publishing.
TeleopPanel::TeleopPanel(QWidget * parent)
: rviz_common::Panel(parent),
  linear_velocity_(0),
  angular_velocity_(0)
{
  // Next we lay out the "output topic" text entry field using a
  // QLabel and a QLineEdit in a QHBoxLayout.
  QHBoxLayout * topic_layout = new QHBoxLayout;
  topic_layout->addWidget(new QLabel("Robot Namespace:"));
  output_topic_editor_ = new QLineEdit;
  topic_layout->addWidget(output_topic_editor_);

  // create the button
  QHBoxLayout * button_layout = new QHBoxLayout;
  arm_button_ = new QPushButton("Arm", this);
  disarm_button_ = new QPushButton("Disarm", this);
  button_layout->addWidget(arm_button_);
  button_layout->addWidget(disarm_button_);

  // Lay out the topic field above the control widget.
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(topic_layout);
  layout->addLayout(button_layout);
  setLayout(layout);

  // Create a timer for sending the output.  Motor controllers want to
  // be reassured frequently that they are doing the right thing, so
  // we keep re-sending velocities even when they aren't changing.
  //
  // Here we take advantage of QObject's memory management behavior:
  // since "this" is passed to the new QTimer as its parent, the
  // QTimer is deleted by the QObject destructor when this TeleopPanel
  // object is destroyed.  Therefore we don't need to keep a pointer
  // to the timer.
  //QTimer * output_timer = new QTimer(this);

  // Next we make signal/slot connections.
  connect(output_topic_editor_, SIGNAL(editingFinished()), this, SLOT(updateTopic()));
  connect(arm_button_, &QPushButton::clicked, this, 
		  [this](){
		  this->sendArmCmd(true);
		  }
  );
  connect(disarm_button_, &QPushButton::clicked, this, 
		  [this](){
		  this->sendArmCmd(false);
		  }
  );
  //connect(arm_button_, signal(QPushButton::clicked()), this, SLOT(sendArmCmd(true));
  //connect(disarm_button_, signal(clicked()), this, slot(sendArmCmd(false));
  //connect(output_timer, SIGNAL(timeout()), this, SLOT(sendArmCmd(false)));

  // Start the timer.
  //output_timer->start(100);

  // Make the control widget start disabled, since we don't start with an output topic.
  //drive_widget_->setEnabled(false);

  // Create the velocity node.
  velocity_node_ = std::make_shared<rclcpp::Node>("teleop_panel_velocity_node");
}

// Read the topic name from the QLineEdit and call setTopic() with the
// results.  This is connected to QLineEdit::editingFinished() which
// fires when the user presses Enter or Tab or otherwise moves focus
// away.
void TeleopPanel::updateTopic()
{
  setTopic(output_topic_editor_->text());
}

// Set the topic name we are publishing to.
void TeleopPanel::setTopic(const QString & new_topic)
{
  // Only take action if the name has changed.
  if (new_topic != output_topic_) {
    output_topic_ = new_topic;
    // If a publisher currently exists, destroy it.
    if (velocity_publisher_ != NULL) {
      velocity_publisher_.reset();
    }
    // If the topic is the empty string, don't publish anything.
    if (output_topic_ != "") {
      // The call to create_publisher() says we want to publish data on the new topic name.
      velocity_publisher_ = velocity_node_->create_publisher<geometry_msgs::msg::Twist>(
        output_topic_.toStdString(), 1);
    }
    // rviz_common::Panel defines the configChanged() signal.  Emitting it
    // tells RViz that something in this panel has changed that will
    // affect a saved config file.  Ultimately this signal can cause
    // QWidget::setWindowModified(true) to be called on the top-level
    // rviz_common::VisualizationFrame, which causes a little asterisk ("*")
    // to show in the window's title bar indicating unsaved changes.
    Q_EMIT configChanged();
  }

  // Gray out the control widget when the output topic is empty.
  //drive_widget_->setEnabled(output_topic_ != "");
}

// Publish the control velocities if ROS is not shutting down and the
// publisher is ready with a valid topic name.
void TeleopPanel::sendArmCmd(bool arm)
{
  if (rclcpp::ok() && velocity_publisher_ != NULL) {
    geometry_msgs::msg::Twist msg;
    msg.linear.x = arm ? 1 : 0;
    msg.linear.y = 0;
    msg.linear.z = 0;
    msg.angular.x = 0;
    msg.angular.y = 0;
    msg.angular.z = 0;
    velocity_publisher_->publish(msg);
  }
}

// Save all configuration data from this panel to the given
// Config object.  It is important here that you call save()
// on the parent class so the class id and panel name get saved.
void TeleopPanel::save(rviz_common::Config config) const
{
  rviz_common::Panel::save(config);
  config.mapSetValue("Topic", output_topic_);
}

// Load all configuration data for this panel from the given Config object.
void TeleopPanel::load(const rviz_common::Config & config)
{
  rviz_common::Panel::load(config);
  QString topic;
  if (config.mapGetString("Topic", &topic)) {
    output_topic_editor_->setText(topic);
    updateTopic();
  }
}

}  // end namespace dasc_robot_gui

// Tell pluginlib about this class.  Every class which should be
// loadable by pluginlib::ClassLoader must have these two lines
// compiled in its .cpp file, outside of any namespace scope.
PLUGINLIB_EXPORT_CLASS(dasc_robot_gui::TeleopPanel, rviz_common::Panel)
// END_TUTORIAL
