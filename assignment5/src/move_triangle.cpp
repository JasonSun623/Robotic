#include <ros/ros.h>
#include "std_msgs/String.h"
#include "turtlesim/Pose.h"
#include "assignment5/triangle.h"
#include <math.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/server/simple_action_server.h>
#include <turtlebot_actions/TurtlebotMoveAction.h>
#include<turtlebot_actions/TurtlebotMoveActionGoal.h>
#include<turtlebot_actions/TurtlebotMoveActionFeedback.h>
#include<actionlib/client/simple_client_goal_state.h>
#include<actionlib/client/simple_action_client.h>
#include<turtlebot_actions/TurtlebotMoveActionResult.h>


#define _USE_MATH_DEFINES
#define BUFFER_SIZE 1
#define SPEED 0.15 
#define SUBSCRIBER_MSG_TOPIC "cmd"
#define SERVER_NAME "turtlebot_move"
#define HEADER_FRAME_ID  "base_link"
#define NODE_NAME "move_triangle"

using namespace std;

struct State {
    bool forward;
    bool start;
    bool active;
    bool done;

    State() {
        forward = true;
        start = false;
        active = false;
        done = true;

    }
};

char nodeName [] = NODE_NAME;
ros::Publisher thePublisher;
bool clockwize = true;
double forwardDist = 0;


actionlib::SimpleActionClient<turtlebot_actions::TurtlebotMoveAction> *MoveClient;
State robotState;


void moveManager(actionlib::SimpleActionClient<turtlebot_actions::TurtlebotMoveAction> *MoveClient);
void DoneCallback(const actionlib::SimpleClientGoalState& state, const turtlebot_actions::TurtlebotMoveResultConstPtr resultMsg);
void ActiveCallback();
void FeedbackCallback(turtlebot_actions::TurtlebotMoveFeedbackConstPtr feedbackMsg);
float GetFloatPrecision(double value, double precision);


//Callback triangle
void commandtriangleReceived(const assignment5::triangle &msgIn) {
  
    forwardDist = msgIn.sideLength;
    clockwize = msgIn.clw;

    if (!robotState.start) {
        moveManager(MoveClient);
    }

}

// Feedback callback
void FeedbackCallback(turtlebot_actions::TurtlebotMoveFeedbackConstPtr feedbackMsg) {

    ROS_INFO_STREAM("Forward distance: " << feedbackMsg.get()->forward_distance);
    ROS_INFO_STREAM("turn around distance: " << feedbackMsg.get()->turn_distance);

}

// Active state callback
void ActiveCallback() {

    ROS_INFO("Goal just went active");
    robotState.active = true;
    robotState.done = false;
}


// Done state callback
void DoneCallback(const actionlib::SimpleClientGoalState& state,
                  const turtlebot_actions::TurtlebotMoveResultConstPtr resultMsg) {
    robotState.active = false;
    robotState.done = true;
    ROS_INFO("... Done");
}



//Now we are building the move publisher

void moveManager(actionlib::SimpleActionClient<turtlebot_actions::TurtlebotMoveAction> *MoveClient) {
    int counter = 7;
    robotState.start = true;
     //
    double   angleInRadians =  2.099;
    angleInRadians -=0.0872665;
    turtlebot_actions::TurtlebotMoveActionGoal goalMsg;
    goalMsg.header.frame_id =HEADER_FRAME_ID;
    for (int i = 1; i < counter; i++) {

        if (robotState.forward) {

            goalMsg.goal.turn_distance = 0.000;
            goalMsg.goal.forward_distance = GetFloatPrecision(forwardDist, 1);
            goalMsg.goal.forward_distance -= 0.03;
            robotState.forward = false;
        }
        else {

            clockwize ? goalMsg.goal.turn_distance = -angleInRadians : goalMsg.goal.turn_distance = angleInRadians;
            goalMsg.goal.forward_distance = 0.000;
            robotState.forward = true;
        }

        if (robotState.done) {
            if (MoveClient->isServerConnected()) {
                ROS_INFO(" Server connected");
                MoveClient->sendGoal(goalMsg.goal, &DoneCallback, &ActiveCallback, &FeedbackCallback);               
                MoveClient->waitForResult();         
                

            }
        }
    }
    robotState.start = false;



}
int main(int argc, char *argv[]) {
    ros::init(argc, argv, nodeName);
    ros::NodeHandle nh;
    MoveClient = new actionlib::SimpleActionClient<turtlebot_actions::TurtlebotMoveAction>(SERVER_NAME, true);
    ros::Subscriber subTriangle = nh.subscribe(SUBSCRIBER_MSG_TOPIC, BUFFER_SIZE, &commandtriangleReceived);
   

     ROS_INFO_STREAM("Waiting for publisher ............. !!");

    ros::spin();
    return 0;
}
float GetFloatPrecision(double value, double precision)
 {
     return (floor((value * pow(10, precision) + 0.5)) / pow(10, precision)); 
 }

