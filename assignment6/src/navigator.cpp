/*
 * send_goals.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: roiyeho
 */

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <move_base_msgs/MoveBaseFeedback.h>
#include <move_base_msgs/MoveBaseResult.h>
#include <move_base_msgs/MoveBaseGoal.h>
#include <move_base/move_base.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_datatypes.h>
#include <iostream>
#include <vector>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

using namespace std;
/*
void doneCb(const actionlib::SimpleClientGoalState& state,
            const move_base_msgs::MoveBaseGoalConstPtr& result)
{
    ROS_INFO("Finished in state [%s]", state.toString().c_str());
    ROS_INFO("Answer: %f", result->goal.target_pose.pose.position.x);
    ros::shutdown();
}

// Called once when the goal becomes active
void activeCb()
{
    ROS_INFO("Goal just went active");
}

// Called every time feedback is received for the goal
void feedbackCb(const move_base_msgs::MoveBaseFeedbackConstPtr& feedback)
{
    ROS_INFO("Got Feedback of length %f", feedback->base_position.pose.position.x);
}
*/
void setGoal(double goal_x, double goal_y, double goal_theta, move_base_msgs::MoveBaseGoal &goal)
{
    goal.target_pose.header.frame_id = "base_link";
    goal.target_pose.header.stamp = ros::Time::now();

    goal.target_pose.pose.position.x = goal_x;
    goal.target_pose.pose.position.y = goal_y;

    // Convert the Euler angle to quaternion
    double radians = goal_theta * (M_PI/180);
    tf::Quaternion quaternion;
    quaternion = tf::createQuaternionFromYaw(radians);

    geometry_msgs::Quaternion qMsg;
    tf::quaternionTFToMsg(quaternion, qMsg);
    goal.target_pose.pose.orientation = qMsg;

}

int main(int argc, char** argv) {
    ros::init(argc, argv, "navigator");
    ros::NodeHandle nh;
    vector<move_base_msgs::MoveBaseGoal> goals;
    double goal_x, goal_y, goal_theta;
    move_base_msgs::MoveBaseGoal goal1;
    move_base_msgs::MoveBaseGoal goal2;
    move_base_msgs::MoveBaseGoal goal3;
    move_base_msgs::MoveBaseGoal goal4;
    //setting goals and putting them in the vector of goals
    goal_x = 3;
    goal_y = 3;
    goal_theta = 0;
    setGoal( goal_x, goal_y, goal_theta, goal1);
    goals.push_back(goal1);

    goal_x = -3;
    goal_y = 3;
    goal_theta = 0;
    setGoal( goal_x, goal_y, goal_theta, goal2);
    goals.push_back(goal2);

    goal_x = -3;
    goal_y = -3;
    goal_theta = 0;
    setGoal( goal_x, goal_y, goal_theta, goal3);
    goals.push_back(goal3);

    goal_x = 3;
    goal_y = -3;
    goal_theta = 0;
    setGoal( goal_x, goal_y, goal_theta, goal4);
    goals.push_back(goal4);

    // Create the string move_base"
    string move_base_str = "/move_base";


    // create the action client
    MoveBaseClient ac(move_base_str, true);

    // Wait for the action server to become available
    ROS_INFO("Waiting for the move_base action server");
    ac.waitForServer(ros::Duration(5));

    ROS_INFO("Connected to move base server");

    // Send a goal to move_base
    for (int i= 0; i<goals.size(); i++){
        ROS_INFO("Sending goal to robot: x = %f, y = %f",
                 goals.at(i).target_pose.pose.position.x, goals.at(i).target_pose.pose.position.y);
        //ac.sendGoal(goal, &doneCb, &activeCb, &feedbackCb);
        ac.sendGoal(goals.at(i));

        // Wait for the action to return
        ac.waitForResult();

        if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        {
            ROS_INFO("Robot has reached the goal!");
            continue;
        }
        else
            ROS_INFO("The base of robot failed for some reason");
    }
    return 0;
}


