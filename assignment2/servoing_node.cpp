//This node is using the servoing technique to make the Robot go to a specified location in the world.
/*We will need:
  A subscriber that receives goal position (Message type: geometry_msgs/PoseStamped )from a topic named "/goal" 
  A Subscriber that recieves the current location (Message type: nav_msgs/Odometry)  of the Robot in the world through the topic: "/odom "  
  A Publisher that sends the  the "goal position" (Message type: geometry_msgs/Twist) to the Robot through the topic: "/cmd_vel "

  */

#include <ros/ros.h>
#include "std_msgs/String.h"
#include <tf/transform_datatypes.h>
#include "geometry_msgs/Twist.h"
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>

  char nodeName []= "servoing_node";
  ros::Publisher thePublisher;
  void goalPositionReceived_CallBack(const geometry_msgs::PoseStamped &goalMsg)
  {


  }
  void RobotCurrentPositionReceived_CallBack(const nav_msgs::Odometry & odmMsg)
  {

  	
  }

  int main(int argc, char  *argv[])
  {

  	ros::init(argc,argv,nodeName);
  	ros::NodeHandle nh;

  	ros::Subscriber subGoalPosition = nh.subscribe("goal", 1000, &goalPositionReceived_CallBack);
  	ros::Subscriber subCurentPose= nh.subscribe("/odom", 1000, &RobotCurrentPositionReceived_CallBack);
  	thePublisher = ros::Publisher(nh.advertise<geometry_msgs::Twist>("cmd_vel",1000));

  	ros::spin();
  	return 0;
  }