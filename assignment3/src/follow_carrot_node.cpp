
#include <ros/ros.h>
#include <tf/transform_datatypes.h>
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/PoseStamped.h" 
#include <nav_msgs/Path.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <math.h>
#include <geometry_msgs/Point.h>

#define BUFFER_SIZE 1000 
#define SPEED 0.5 
#define LOOK_AHEAD 0.9
#define PLAN_FRAME   "odom"
#define PATH_FRAME   "map"
#define ROBOT_FRAME  "base_footprint"
#define PUBLISHER_TOPIC "cmd_vel"
#define SUBSCRIBER_TOPIC "plan"




void Path_Received_CallBack(nav_msgs::Path pathMsg);
double calculateAngle(geometry_msgs::Pose current, geometry_msgs::Pose target);
bool checkLookAhead(geometry_msgs::Pose target);
double calculateDistance(geometry_msgs::Pose current, geometry_msgs::Pose target);
geometry_msgs::PoseStamped CurrentRobotPose();

void printReceivedData();
double calculateAngularError(double angle, double angleRobot);
void rotationProcessing(double angular_Velo, double linear_Velo);
void processForward(double linear_Velo, geometry_msgs::Pose target);
void moveToWayPoint(geometry_msgs::Pose wayPoint);

char nodeName [] = "follow_carrot_node";
double angleCmd = 0;
ros::Publisher thePublisher;
nav_msgs::Path path;




tf::TransformListener *listener;
bool received, running = false;
std::vector<geometry_msgs::Pose> carrotPoses;

void Path_Received_CallBack(nav_msgs::Path pathMsg) {

    if (!running) {
        while (!carrotPoses.empty())carrotPoses.pop_back();
        path = pathMsg;
        received = true;

        tf::StampedTransform transform;
        try {
            bool transform_exist = false;
            ros::Time now = ros::Time::now();
            transform_exist = listener->waitForTransform(PATH_FRAME, PLAN_FRAME, now, ros::Duration(5.0));
            if (transform_exist) {
                listener->lookupTransform(PATH_FRAME, PLAN_FRAME, now, transform);
                ROS_INFO_STREAM("Transform found");
                double phi = 2.0 * asin(transform.getRotation().z());
                std::vector<geometry_msgs::PoseStamped> data = pathMsg.poses;

                for (int i = 0; i < data.size(); i++) {
                    double x_t = data[i].pose.position.x - transform.getOrigin().x(); //translation
                    double y_t = data[i].pose.position.y - transform.getOrigin().y(); //translation
                    double x_r = x_t * cos(phi) + y_t * sin(phi);
                    double y_r = -x_t * sin(phi) + y_t * cos(phi);
                    geometry_msgs::Pose p;
                    p.position.x = x_r;
                    p.position.y = y_r;
                    p.position.z = 0;
                    p.orientation.x = transform.getRotation().x();
                    p.orientation.y = transform.getRotation().y();
                    p.orientation.z = transform.getRotation().z();
                    p.orientation.w = transform.getRotation().w();

                    carrotPoses.push_back(p);
                }
                printReceivedData();
                for (int i = 0; i < carrotPoses.size(); i++) {
                    moveToWayPoint(carrotPoses[i]);
                }
                running = false;
            } else
                ROS_INFO_STREAM("Transform Not found");
        } catch (tf::TransformException ex) {
            ROS_ERROR("%s", ex.what());
            ros::Duration(1.0).sleep();
        }
    }
}

void moveToWayPoint(geometry_msgs::Pose wayPoint) {
    ROS_INFO_STREAM("Next way point:");
    ROS_INFO_STREAM(wayPoint);

    geometry_msgs::Twist theMsg;

    geometry_msgs::PoseStamped currentPose = CurrentRobotPose();
    angleCmd = calculateAngularError(calculateAngle(currentPose.pose, wayPoint), tf::getYaw(currentPose.pose.orientation));
    double speedCmd = 0;
    ROS_INFO_STREAM(angleCmd);

    rotationProcessing(angleCmd, speedCmd);
    currentPose = CurrentRobotPose();
    double dista = calculateDistance(currentPose.pose, wayPoint);
    processForward(dista, wayPoint);
}

double calculateDistance(geometry_msgs::Pose current, geometry_msgs::Pose target) {

    return sqrt((pow(target.position.y - current.position.y, 2.0)) + (pow(target.position.x - current.position.x, 2.0)));
}

double calculateAngularError(double angle, double angleRobot) {
    double agl = angle - angleRobot;
    //normalizing angle to <-pi; pi>checkProximity
    if (fabs(agl) > M_PI) {
        agl = angle - copysign(2 * M_PI, agl);
    }
    return agl;
}

double calculateAngle(geometry_msgs::Pose current, geometry_msgs::Pose target) {

    double angle = atan2((target.position.y - current.position.y), (target.position.x - current.position.x));
    return angle;
}

bool checkLookAhead(geometry_msgs::Pose target) {

    double distanceToGoal = calculateDistance(CurrentRobotPose().pose, target);
    return distanceToGoal > LOOK_AHEAD ? false : true;
}

geometry_msgs::PoseStamped CurrentRobotPose() {
    tf::StampedTransform transform;
    geometry_msgs::PoseStamped current_robot_pose;
    listener->lookupTransform(PLAN_FRAME, ROBOT_FRAME, ros::Time(0), transform);

    current_robot_pose.header.frame_id = ROBOT_FRAME;
    current_robot_pose.header.stamp = ros::Time(0);
    current_robot_pose.pose.position.x = transform.getOrigin().x();
    current_robot_pose.pose.position.y = transform.getOrigin().y();
    current_robot_pose.pose.position.z = transform.getOrigin().z();
    current_robot_pose.pose.orientation.w = transform.getRotation().w();
    current_robot_pose.pose.orientation.x = transform.getRotation().x();
    current_robot_pose.pose.orientation.y = transform.getRotation().y();
    current_robot_pose.pose.orientation.z = transform.getRotation().z();

    return current_robot_pose;
}

//Main Function

int main(int argc, char *argv[]) {

    ros::init(argc, argv, nodeName);
    ros::NodeHandle nh;
    listener = new tf::TransformListener();
    ros::Subscriber rectangleSub = nh.subscribe(SUBSCRIBER_TOPIC, BUFFER_SIZE, &Path_Received_CallBack);
    thePublisher = ros::Publisher(nh.advertise<geometry_msgs::Twist>(PUBLISHER_TOPIC, BUFFER_SIZE));
    ROS_INFO_STREAM("Local OPlanner is running, waiting for the path...");

    ros::spin();
    return 0;
}

//This function publishes the Twist messages.

/*
 This function is simply used to debug and print out the Received Path
 */
void printReceivedData() {
    switch (carrotPoses.size()) {
        case 4:
            ROS_INFO_STREAM(" Rectangle Path received ");
            break;
        case 3:
            ROS_INFO_STREAM(" Triangle Path received ");
            break;
        default:
            ROS_INFO_STREAM(" Unknown Path received ");
            break;
    }
    for (int i = 0; i < carrotPoses.size(); i++) {
        ROS_INFO_STREAM("  [  Goal point :" << i + 1 << " X= " << carrotPoses[i].position.x << "  Y= " << carrotPoses[i].position.y << "]");
    }
}

void processForward(double linear_Velo, geometry_msgs::Pose wayPoint) {
    linear_Velo = calculateDistance(CurrentRobotPose().pose, wayPoint);
    geometry_msgs::Twist msg;
    if (angleCmd < 0)msg.angular.z = -0.02;
    else msg.angular.z = 0.02;
    msg.linear.x = SPEED;
    ros::spinOnce();
    double current_distance = double(0);
    double T0 = ros::Time::now().toSec();
    ros::Rate rosRate(100);

    do {
        if (!checkLookAhead(wayPoint)) {
            thePublisher.publish(msg);
            double T1 = ros::Time::now().toSec();
            current_distance = SPEED * (T1 - T0);
            ros::spinOnce();
            rosRate.sleep();
        } else {
            if (current_distance < fabs(linear_Velo))
                break;
        }

    } while (current_distance < fabs(linear_Velo));
}

void rotationProcessing(double angular_Velo, double linear_Velo) {
    running = true;
    geometry_msgs::Twist msg;
    msg.angular.z = copysignf(fmin(SPEED, fabs(angular_Velo * SPEED)), angular_Velo);
    msg.linear.x = SPEED / 1.5;
    ros::spinOnce();
    double current_angle = double(0);
    double T0 = ros::Time::now().toSec();
    ros::Rate rosRate(100);

    do {
        thePublisher.publish(msg);
        double T1 = ros::Time::now().toSec();
        current_angle = SPEED * (T1 - T0);
        ros::spinOnce();
        rosRate.sleep();

    } while (current_angle < fabs(angular_Velo));
}
