//here we are using the point-Shoot techniques to move the Robot to a specific location in the world 
#include <ros/ros.h>
#include <tf/transform_datatypes.h>
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/PoseStamped.h" 
#include "nav_msgs/Odometry.h"
#include "std_msgs/String.h"
#include <math.h>


/*COMMAND TO PUBLISH TARGET IN TERMINAL
	 
	 rostopic pub /assignment2/goal geometry_msgs/PoseStamped '{ header: {stamp: now, frame_id: "map"}, pose: { position: {x: -4.560, y: 0.204, z: 0.0}, orientation: {w: 1.0}}}'

*/
   void DriveToGoal (const geometry_msgs::PoseStamped &goalMsg);
   void setState(bool state);
   void shooting(const geometry_msgs::PoseStamped &goalMsg);
   void pointing(double goal_angle, double dist);


   char nodeName []= "pointshoot_node";
   volatile double currentX =0.0;
   volatile double currentY =0.0;
   volatile double currentZ =0.0;
   volatile double currentOrientationZ =0.0;
   double goalX =0.0;
   double goalY =0.0;
   double angle_in_radians= 0.0;
   double distance = 0.0;

   nav_msgs::Odometry current_odmMsg;
   ros::Publisher thePublisher;
   bool arrived = false;
   void goalPositionReceived_CallBack(const geometry_msgs::PoseStamped &goalMsg)
   { arrived = false;
     goalX =goalMsg.pose.position.x;
     goalY =goalMsg.pose.position.y;
     angle_in_radians = atan2(goalY - currentY, goalX - currentX);
     double X =goalMsg.pose.position.x- currentX;
     double Y =goalMsg.pose.position.y- currentY;
     distance = sqrt((pow(X, 2) )+ (pow (Y, 2) ));
    
     ROS_INFO_STREAM("  Angle in radians: " << angle_in_radians);
     ROS_INFO_STREAM("  Distance to goal: " << distance);
     DriveToGoal (goalMsg);


  }
  void RobotCurrentPositionReceived_CallBack(const nav_msgs::Odometry  odmMsg)
  {
   current_odmMsg = odmMsg;
   currentX = odmMsg.pose.pose.position.x;
   currentY = odmMsg.pose.pose.position.y;
   currentZ = odmMsg.pose.pose.position.z;
   
 }

 int main(int argc, char  *argv[])
 {

   ros::init(argc,argv,nodeName);
   ros::NodeHandle nh;

   ros::Subscriber subGoalPosition = nh.subscribe("/goal", 1000, &goalPositionReceived_CallBack);
   ros::Subscriber subCurentPose= nh.subscribe("/odom", 1000, &RobotCurrentPositionReceived_CallBack);
   thePublisher = ros::Publisher(nh.advertise<geometry_msgs::Twist>("cmd_vel",1000));

   ros::spin();
   return 0;
 }


 void DriveToGoal (const geometry_msgs::PoseStamped &goalMsg)
 { 

    //ROS_INFO_STREAM("  Position X after: " << moveMsg.linear.x);
    // ROS_INFO_STREAM("  Position Y after: " << moveMsg.linear.y);
  //rosRate.sleep();
  //shooting(goalMsg);
  pointing(angle_in_radians, distance);
  shooting(goalMsg);
  
}

void setState(bool state)
{
  arrived = state;
}
void shooting(const geometry_msgs::PoseStamped &goalMsg)
{

  /*geometry_msgs::Twist moveMsg;
  moveMsg.linear.x=currentX;
  moveMsg.linear.y=currentY;
  moveMsg.linear.z=currentZ;
  moveMsg.angular.x =0;
  moveMsg.angular.y =0;
  moveMsg.angular.z =0;
  moveMsg.linear.x =currentX;
  //ROS_INFO_STREAM("  Position X: " << currentX);
  //ROS_INFO_STREAM("  Position Y: " << currentY);

  ros::Rate rosRate(1.0);
  (currentX<goalMsg.pose.position.x)?(moveMsg.linear.x+=1,thePublisher.publish(moveMsg),ros::spinOnce(),rosRate.sleep(), shooting(goalMsg)):(setState(true));*/
  geometry_msgs::Twist moveMsg;
  double speed =20;
 
   moveMsg.linear.x =abs(speed);


  //Now we need to set the linear y and z to 0
  moveMsg.linear.z =0;
  moveMsg.linear.y=0;

  moveMsg.angular.x =0;
  moveMsg.angular.y =0;
  moveMsg.angular.z =0;
  double current_distance = double(0);
  double T0 = ros::Time::now().toSec();
  ros::Rate rosRate(100);
  
  do
  {
    thePublisher.publish(moveMsg);
    double T1 = ros::Time::now().toSec();
    current_distance = speed *(T1-T0);
    ros::spinOnce();
    rosRate.sleep();

  } while (current_distance <=distance); 
  moveMsg.linear.x=0;
  moveMsg.linear.y=0;
  moveMsg.linear.z=0;
  moveMsg.angular.x =0;
  moveMsg.angular.y =0;
  moveMsg.angular.z =0;
  thePublisher.publish(moveMsg);


}
void pointing(double goal_angle, double dist)
{
   ROS_INFO_STREAM("  Pointing already");
    double angular_speed = 25*M_PI/180;
    geometry_msgs::Twist theMsg;
    theMsg.linear.x=0;
    theMsg.linear.y=0;
    theMsg.linear.z=0;
    theMsg.angular.y =0; 
    theMsg.angular.x =0;
    theMsg.angular.z =abs(angular_speed);

    double current_angle = double(0);
    double T0 = ros::Time::now().toSec();
    ros::Rate rosRate(100);

    do
    {
      /* code */
      ROS_INFO_STREAM("  in pointing loop");
      theMsg.angular.z+=0.1;
      thePublisher.publish(theMsg);
      double T1 = ros::Time::now().toSec();
      current_angle = angular_speed *(T1-T0);
      ros::spinOnce();
      rosRate.sleep();

    } while (current_angle <angle_in_radians); //turning towards the goal 
    theMsg.angular.z=0;
    thePublisher.publish(theMsg);
}


