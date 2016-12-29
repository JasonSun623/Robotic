

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Path.h>
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>


nav_msgs::Path getPath();
char nodeName [] = "rectangle_node";
ros::Publisher thePublisher;
nav_msgs::Path path;

nav_msgs::Path getPath() {

    geometry_msgs::PoseStamped pose1, pose2, pose3, pose4;
    tf::quaternionTFToMsg(tf::Quaternion(0, 0, 0, 1), pose1.pose.orientation);
    tf::quaternionTFToMsg(tf::Quaternion(0, 0, 0, 1), pose2.pose.orientation);
    tf::quaternionTFToMsg(tf::Quaternion(0, 0, 0, 1), pose3.pose.orientation);
    tf::quaternionTFToMsg(tf::Quaternion(0, 0, 0, 1), pose4.pose.orientation);
    pose1.pose.position.x = 0.0;
    pose1.pose.position.y = 3.0;
    

    pose2.pose.position.x = 5.0;
    pose2.pose.position.y = 3.0;

    pose3.pose.position.x = 5.0;
    pose3.pose.position.y = 0.0;

    pose4.pose.position.x = 0.0;
    pose4.pose.position.y = 0.0;

    path.poses.push_back(pose1);
    path.poses.push_back(pose2);
    path.poses.push_back(pose3);
    path.poses.push_back(pose4);
    return path;

}

int main(int argc, char *argv[]) {

    ros::init(argc, argv, nodeName);
    ros::NodeHandle nh;
    thePublisher = ros::Publisher(nh.advertise<nav_msgs::Path>("plan", 1000));

    sleep(3.0);
    thePublisher.publish(getPath());
    ROS_INFO_STREAM("RECTANGLE PLANNER: Path published !!");

    ros::spinOnce();
    return 0;
}
