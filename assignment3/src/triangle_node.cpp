

#include <ros/ros.h>
#include <nav_msgs/Path.h>
#include <geometry_msgs/PoseStamped.h>
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>

char nodeName [] = "triangle_node";
ros::Publisher thePublisher;
nav_msgs::Path path;
nav_msgs::Path getPath();

nav_msgs::Path getPath() {

    geometry_msgs::PoseStamped pose1, pose2, pose3;
    pose1.pose.position.x = 4.0;
    pose1.pose.position.y = 3.0;

    pose2.pose.position.x = 8.0;
    pose2.pose.position.y = 0.0;

    pose3.pose.position.x = 0.0;
    pose3.pose.position.y = 0.0;
    pose1.pose.orientation.x = 0.000;
    pose1.pose.orientation.y = 0.000;
    pose1.pose.orientation.z = -1.35;
    pose1.pose.orientation.w = 0.991;


    pose1.pose.orientation = tf::createQuaternionMsgFromYaw(1.5708);
    pose2.pose.orientation = tf::createQuaternionMsgFromYaw(1.5708);
    pose3.pose.orientation = tf::createQuaternionMsgFromYaw(1.5708);

    path.poses.push_back(pose1);
    path.poses.push_back(pose2);
    path.poses.push_back(pose3);
    return path;

}

int main(int argc, char *argv[]) {

    ros::init(argc, argv, nodeName);
    ros::NodeHandle nh;

    thePublisher = ros::Publisher(nh.advertise<nav_msgs::Path>("plan", 1000));

    sleep(3.0);
    thePublisher.publish(getPath());
    ROS_INFO_STREAM("TRINAGLE PLANNER: Path published !!");

    ros::spinOnce();
    return 0;
}