#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>

static std::string topic = "/robot/odom";
static std::string parent_frame = "/world";
static std::string child_frame = "/odom";

ros::Publisher pud_odom_enu_;

void odom_callback(const nav_msgs::OdometryConstPtr &odom)
{
  static tf::TransformBroadcaster br;
  tf::Transform tf;
  // convert NED to ENU and fill tf and odom
  // translation
  geometry_msgs::Pose odom_pose;
  odom_pose.position.x = odom->pose.pose.position.x;
  odom_pose.position.y = -odom->pose.pose.position.y;
  odom_pose.position.z = -odom->pose.pose.position.z;
  // rotation
  odom_pose.orientation.x = odom->pose.pose.orientation.x;
  odom_pose.orientation.y = -odom->pose.pose.orientation.y;
  odom_pose.orientation.z = -odom->pose.pose.orientation.z;
  odom_pose.orientation.w = odom->pose.pose.orientation.w;

  // odometry
  nav_msgs::Odometry odom_enu;
  odom_enu.header = odom->header;
  odom_enu.pose.pose = odom_pose;
  odom_enu.pose.pose.orientation = odom_pose.orientation;
  // odom_enu.twist.twist = odom->twist.twist; // velocity never used

  tf::poseMsgToTF(odom_pose, tf);
  tf::StampedTransform stamped_tf(tf, odom->header.stamp, parent_frame, child_frame);
  br.sendTransform(stamped_tf);
  pud_odom_enu_.publish(odom_enu);
}

int main(int argc, char **argv)
{

  ros::init(argc, argv, "odom2tf");
  ros::NodeHandle nh;
  ros::NodeHandle private_nh("~");

  private_nh.getParam("odom_topic", topic);
  private_nh.getParam("parent_frame", parent_frame);
  private_nh.getParam("child_frame", child_frame);

  ros::Subscriber odom_sub = nh.subscribe(topic, 10, odom_callback);
  pud_odom_enu_ = nh.advertise<nav_msgs::Odometry>("qvio/odometry_enu", 10);
  ros::spin();

  return 0;
}