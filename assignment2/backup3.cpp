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
  void DriveToGoal();
  void shooting();
  void pointing( double angle);
  void CalculateDistanceError(double X, double Y);
  void CalculateAngleErrorInRadians(double Y, double X);



  char nodeName []= "pointshoot_node";
  volatile double currentX =0.0;
  volatile double currentY =0.0;
  double goalX =0.0;
  double goalY =0.0;
  double angle_in_radians= 0.0;
  double distance = 0.0;
  double angle_Orientation =0.0;
  double goalOrientation =0.0;

  nav_msgs::Odometry current_odmMsg;
  ros::Publisher thePublisher;
  bool arrived = false;
   //Processing Functions 
  void CalculateDistanceError(double X, double Y)
  {
    distance = ((int)(10.0*sqrt((pow(X, 2) )+ (pow (Y, 2) )))/10.0F);

  }

  void CalculateAngleErrorInRadians(double Y,double X)
  {
    angle_in_radians = (int)(10.0*(atan2(Y, X)))/10.0F;
  }

  //Callbacks for our Subscribers.
  void goalPositionReceived_CallBack(const geometry_msgs::PoseStamped &goalMsg)
  {  arrived = false;
   goalX =goalMsg.pose.position.x;
   goalY =goalMsg.pose.position.y;
   

   double X =goalMsg.pose.position.x- currentX;
   double Y =goalMsg.pose.position.y- currentY;
   goalOrientation =  tf::getYaw(goalMsg.pose.orientation);
   CalculateDistanceError(X, Y);
   CalculateAngleErrorInRadians(Y,X);

   DriveToGoal ();

 }

 void RobotCurrentPositionReceived_CallBack(const nav_msgs::Odometry  odmMsg)
 {
  current_odmMsg = odmMsg;
  currentX = odmMsg.pose.pose.position.x;
  currentY = odmMsg.pose.pose.position.y;
  angle_Orientation = (int)(10.0*(tf::getYaw(odmMsg.pose.pose.orientation)))/10.0F;

 }

//Main Function
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

void DriveToGoal()
{ 
  pointing(angle_in_radians);
  arrived = true;
   pointing(goalOrientation);
}

//This is the Straight driving Function
void shooting()
{
   ROS_INFO_STREAM("  Shooting already");

  geometry_msgs::Twist moveMsg;
  double speed =20*M_PI/180;

  moveMsg.linear.x=fabs(speed);
  moveMsg.linear.y=0;
  moveMsg.linear.z=0;
  // Becasue we are driving in a straight line, we should keep the Angular to 0
  moveMsg.angular.x =0;
  moveMsg.angular.y =0;
  moveMsg.angular.z =0;


  double T0 = ros::Time::now().toSec();
  double current_distance = double(0);
  ros::Rate rosRate(100);
  
  do
  {
    thePublisher.publish(moveMsg);
    double T1 = ros::Time::now().toSec();
    current_distance = ((int)(10.0*speed *(T1-T0)))/10.0F;
    ros::spinOnce();
    rosRate.sleep();

  } while (current_distance <distance); 
  moveMsg.linear.x=0;
  thePublisher.publish(moveMsg);


}

//This is the Rotation Function
void pointing(double angle)
{
 double angular_speed = 20*M_PI/180;
 geometry_msgs::Twist RotationMsg;
  //Because we are rotating, so all Linears values should be reset to 0
 RotationMsg.linear.x =0;
 RotationMsg.linear.y =0;
 RotationMsg.linear.z =0;

 RotationMsg.angular.x =0;
 RotationMsg.angular.y =0;
 RotationMsg.angular.z=fabs(angular_speed);

 thePublisher.publish(RotationMsg);
 double T0 = ros::Time::now().toSec();
 double current_angle = double(0);
 ros::Rate rosRate(1000);
 
 do
 {
  thePublisher.publish(RotationMsg);
  double T1 = ros::Time::now().toSec();
  current_angle = ((int)(10.0*angular_speed  *(T1-T0)))/10.0F;

  ros::spinOnce();
  rosRate.sleep();

 }while (current_angle <angle); //Checking the Current angle with the goal Angle
    RotationMsg.angular.z=0;
    thePublisher.publish(RotationMsg);
    if(!arrived)
    {
      shooting();   
    }

}
