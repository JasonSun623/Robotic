

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <move_base_msgs/MoveBaseGoal.h>
#include <move_base/move_base.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_datatypes.h>
#include <iostream>
#include <vector>
#define NODE_NAME "navigator"
#define BASE_NAME "/move_base"
#define FRAME_ID "map"
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

using namespace std;

struct MyPoint {
    double x;
    double y;
    double w;

    MyPoint() {
        x = 0;
        y = 0;
        w = 0;
    }

};


class Navigator {
public:
    // members

    // methods
    Navigator();
    bool navigate_to_goal(void);


protected:
    // members
    ros::NodeHandle nh_;
    vector<move_base_msgs::MoveBaseGoal> allGoals;
    move_base_msgs::MoveBaseGoal goal_1;
    move_base_msgs::MoveBaseGoal goal_2;
    move_base_msgs::MoveBaseGoal goal_3;
    move_base_msgs::MoveBaseGoal goal_4;

    // methods
    void set_goal(move_base_msgs::MoveBaseGoal &goal, MyPoint pt);

};

///////////////////////////////////////////////////////////////////////////

Navigator::Navigator() :
nh_("~") {
    MyPoint pA, pB, pC, pD;
    //Corner A
    pA.x = 9.7802;
    pA.y = 5.8102;

    //Corner B
    pB.x = -6.92875;
    pB.y = 7.72207;

    //Corner C
    pC.x = 7.8068;
    pC.y = -6.2778;

    //Corner D
    pD.x = -8.90123;
    pD.y = -3.0669;

    set_goal(goal_1, pA);
    set_goal(goal_2, pB);
    set_goal(goal_3, pC);
    set_goal(goal_4, pD);
    allGoals.push_back(goal_1);
    allGoals.push_back(goal_2);
    allGoals.push_back(goal_3);
    allGoals.push_back(goal_4);
    allGoals.push_back(goal_1);




}

void Navigator::set_goal(move_base_msgs::MoveBaseGoal &goal, MyPoint pt) {
    goal.target_pose.header.frame_id = FRAME_ID;
    goal.target_pose.header.stamp = ros::Time::now();

    goal.target_pose.pose.position.x = pt.x;
    goal.target_pose.pose.position.y = pt.y;

    // euler angle => quaternion
    double r = pt.w * (M_PI / 180);
    tf::Quaternion quaternion;
    quaternion = tf::createQuaternionFromYaw(r);

    geometry_msgs::Quaternion q;
    tf::quaternionTFToMsg(quaternion, q);
    goal.target_pose.pose.orientation = q;
    return;
}

bool Navigator::navigate_to_goal(void) {
    MoveBaseClient ac(BASE_NAME, true);

    ROS_INFO("Waiting for server .....");
    bool severIsON = ac.waitForServer(ros::Duration(3));

    if (severIsON) {
        ROS_INFO("...Connected to server");
        for (int i = 0; i < allGoals.size(); i++) {
            ac.sendGoal(allGoals.at(i));
            ROS_INFO("Driving to goal:  %d", i + 1);

            ac.waitForResult();

            if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
                ROS_INFO("Robot has reached the goal: %d", i + 1);
                continue;
            } else {
                ROS_INFO("The base of robot failed for some reason");
                return false;
            }

        }
    } else {
        ROS_INFO("ERROR: couldn't load server !!!");
        return false;
    }
    ROS_INFO("SUCCESS: ALL GOALS ACHIEVED !!!");
    return true;
}

int main(int argc, char **argv) {
    ros::init(argc, argv, NODE_NAME);
    Navigator navigator;
    navigator.navigate_to_goal();
    ros::spin();


    return 0;
}



