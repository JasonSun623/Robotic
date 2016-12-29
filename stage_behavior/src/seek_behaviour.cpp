#include <ros/ros.h>
#include "sensor_msgs/LaserScan.h"
#include <tf/transform_datatypes.h>
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/PoseStamped.h" 
#include <nav_msgs/Path.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <math.h>

#define GOAL_TOPIC "move_base_simple/goal"
#define PUBLISHER_TOPIC "cmd_vel0"
#define NODE_NAME "seek_behaviour"
#define PLAN_FRAME   "odom"
#define ROBOT_FRAME  "base_footprint"
#define SUBSCRIBER_BUFFER_SIZE 1 //!<Size of buffer for subscriber.
#define PUBLISHER_BUFFER_SIZE 1000 //!<Size of buffer for publisher.
#define TOLERANCE 0.02

using namespace std;

enum State {
    IDLE,
    RUNNING,
    ARRIVED,
};

class SeekBehaviour {
public:
    // members

    // methods
    SeekBehaviour();

protected:
    // members
    ros::NodeHandle nh;
    ros::Subscriber goal_sub;
    ros::Publisher commandPub; // Publisher to the robot's velocity command topic
    ros::Subscriber laserSub; // Subscriber to the robot's laser scan topic
    geometry_msgs::PoseStamped currentPose, goal;
    double angleToGoal;

    State currentState; // state of seeking procedure
    tf::TransformListener *listener;
    tf::StampedTransform transform;
    int rate_;
    double robotAngle;
    bool running, rotate,stop_at_target;
    double danger_angle,danger_distance,speedCommand,angleCommand,max_linear_velocity,max_angular_velocity; //<Distance of closest point from laser scan.
    double avoid_distance_front,avoid_distance_side,angle_to_velocity,skip_sensor_dist ;

    void on_Goal_Received(geometry_msgs::PoseStamped msg);
    void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan);
    geometry_msgs::PoseStamped getRobotPose();
    double calculateAngle(geometry_msgs::Pose current, geometry_msgs::Pose target);
    double CalculateDistanceError(geometry_msgs::Pose current, geometry_msgs::Pose target);
    double CalculateAngleError(double angle, double angleRobot);
    double avoidCollisionAngle(double angleCommand, double speedCommand);
    double avoidCollisionSpeed(double angleCommand, double speedCommand);
    void publishMessage(double angleCommand, double speedCommand);
    bool proximity(geometry_msgs::Pose target);


};

SeekBehaviour::SeekBehaviour() {
    currentState = IDLE;
    listener = new tf::TransformListener();
    avoid_distance_front = 0.2;
    angle_to_velocity = 2.0;
    avoid_distance_side = 0.14;
    max_angular_velocity = 0.5;
     rate_ = 50;
     stop_at_target = true;
     max_linear_velocity =0.1;
    skip_sensor_dist = 0.06;
    commandPub = nh.advertise<geometry_msgs::Twist>(PUBLISHER_TOPIC, 10);
    goal_sub = nh.subscribe<geometry_msgs::PoseStamped>(GOAL_TOPIC, 100, &SeekBehaviour::on_Goal_Received, this);
    laserSub = nh.subscribe("base_scan", 1, &SeekBehaviour::scanCallback, this);


}



// Process the incoming laser scan message

void SeekBehaviour::scanCallback(const sensor_msgs::LaserScan::ConstPtr& msg) {
    
  //Calculation of array size from angle range and angle increment.
  int size = msg->ranges.size();
  //Variables whith index of the lowest value in array.
  int minIndex = 0;
  //This cycle goes through array and finds minimum (too small values are skipped)
  for(int i=minIndex; i<size; i++)
  {
    if (msg->ranges[i] < msg->ranges[minIndex] && msg->ranges[i] >skip_sensor_dist){
      minIndex = i;
    }
  }
  //Calculation of angles from indexes and storing data to class variables.
  danger_angle = (minIndex-size)*msg->angle_increment;
  danger_distance = msg->ranges[size/2];
  if(currentState == RUNNING)
  {
  //Transformation
  ros::Duration(0.1).sleep();
  currentPose =getRobotPose();
  if (proximity(goal.pose))
  {
    publishMessage(0.0,0.0);
    if(stop_at_target){
      currentState = ARRIVED;
      ROS_INFO_STREAM("Arrived at goal!! ");
            ROS_INFO_STREAM(CalculateDistanceError(getRobotPose().pose, goal.pose));
      }
  }
   angleCommand = CalculateAngleError(calculateAngle(currentPose.pose, goal.pose), tf::getYaw(currentPose.pose.orientation));
  if (fabs(angleCommand) > 1.0)
  {
    speedCommand = 0;
  }
  else if (fabs(angleCommand) > 0.5)
  {
    speedCommand = max_linear_velocity/2;
  }
  else
  {
    speedCommand = max_linear_velocity;
  }

  //Invoking method for publishing message
  publishMessage(avoidCollisionAngle(angleCommand, speedCommand),avoidCollisionSpeed(angleCommand, speedCommand));
  }
  else if(currentState == ARRIVED)
  {
      currentState =IDLE;
  }
}
bool SeekBehaviour::proximity(geometry_msgs::Pose target) {

    double distanceToGoal = CalculateDistanceError(getRobotPose().pose, target);
    return distanceToGoal > TOLERANCE ? false : true;
}
double SeekBehaviour::CalculateDistanceError(geometry_msgs::Pose current, geometry_msgs::Pose target) {
       return sqrt((pow(target.position.y - current.position.y, 2.0)) + (pow(target.position.x - current.position.x, 2.0)));

}

double SeekBehaviour::CalculateAngleError(double angle, double angleRobot) {
    double agl = angle - angleRobot;
    //normalizing angle to <-pi; pi>checkProximity
    if (fabs(agl) > M_PI) {
        agl = angle - copysign(2 * M_PI, agl);
    }
    return agl;
}
//Process the incoming goals

void SeekBehaviour::on_Goal_Received(geometry_msgs::PoseStamped goalMsg) {
    
    if(currentState ==IDLE)
    {
        ROS_INFO_STREAM(goalMsg);
        goal = goalMsg;
        ROS_INFO_STREAM("Now moving to goal");
        currentState =RUNNING;
    }

}

//Calculate angel between 2 lines

double SeekBehaviour::calculateAngle(geometry_msgs::Pose current, geometry_msgs::Pose target) {

    double angle = atan2((target.position.y - current.position.y), (target.position.x - current.position.x));
    return angle;
}

//Get the current robot's Pose

geometry_msgs::PoseStamped SeekBehaviour::getRobotPose() {


    bool transform_exist = false;
    ros::Time now = ros::Time::now();
    geometry_msgs::PoseStamped current_robot_pose;
    transform_exist = listener->waitForTransform(PLAN_FRAME, ROBOT_FRAME, now, ros::Duration(2.0));
    if (transform_exist) {
        listener->lookupTransform(PLAN_FRAME, ROBOT_FRAME, ros::Time(0), transform);

        current_robot_pose.header.frame_id = ROBOT_FRAME;
        current_robot_pose.pose.position.x = transform.getOrigin().x();
        current_robot_pose.pose.position.y = transform.getOrigin().y();
        current_robot_pose.pose.position.z = transform.getOrigin().z();
        double w = current_robot_pose.pose.orientation.w = transform.getRotation().w();
        double x = current_robot_pose.pose.orientation.x = transform.getRotation().x();
        double y = current_robot_pose.pose.orientation.y = transform.getRotation().y();
        double z = current_robot_pose.pose.orientation.z = transform.getRotation().z();
        robotAngle = atan2(2 * (y * x + w * z), w * w + x * x - y * y - z * z);
    }
    return current_robot_pose;
}
double SeekBehaviour::avoidCollisionAngle(double angleCommand, double speedCommand)
{
  if (speedCommand < 0.001)  //It does not move
  {
    return angleCommand;
  }
  else if (danger_distance < avoid_distance_front && fabs(danger_angle) < M_PI/6.0)  //Obstacle in front
  {
    return copysign(0.3,-danger_angle);
  }
  else if (danger_distance < avoid_distance_side && fabs(danger_angle) < 4.0*M_PI/6.0)  //Obstacle on the side.
  {
    return copysign(0.2,-danger_angle);
  }
  else
  {
    return angleCommand;
  }
}

double SeekBehaviour::avoidCollisionSpeed(double angleCommand, double speedCommand)
{
  if (speedCommand < 0.001)  //It does not move
  {
    return speedCommand;
  }
  else if (danger_distance < avoid_distance_front && danger_distance >= avoid_distance_side && fabs(danger_angle) < M_PI/6.0)  //Obstacle far, front
  {
    return 0.8*speedCommand;
  }
  else if (fabs(danger_angle) < 4.0*M_PI/6.0 && danger_distance < avoid_distance_side)  //Obstacle close
  {
    return 0.6*speedCommand;
  }
  else
  {
    return speedCommand;
  }
}
void SeekBehaviour::publishMessage(double angleCommand, double speedCommand)
{
  //preparing message
  geometry_msgs::Twist msg;

  msg.linear.x = speedCommand;
  msg.angular.z = msg.angular.z = copysignf(fmin(max_angular_velocity, fabs(angleCommand*angle_to_velocity)), angleCommand);

  //publishing message
  commandPub.publish(msg);
}
/*Main Function*/
int main(int argc, char **argv) {
    ros::init(argc, argv, NODE_NAME);
    SeekBehaviour seek_behaviour;
    ROS_INFO("Node Started correctly and is now waiting for goal...");

    ros::spin();
    return 0;
}


