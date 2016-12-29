#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#define INPUT1_TOPIC "cmd_vel1"
#define INPUT3_TOPIC "cmd_vel3"
#define INPUT0_TOPIC "cmd_vel0"
#define PUBLISHER_TOPIC "cmd_vel"
#define NODE_NAME "arbiter"
/**
 * Priority based arbiter, forwards highest priority commands
 */
using namespace std;
class Arbiter {
public:
    // members

    // methods
    Arbiter();

protected:
    ros::NodeHandle nh_;
    ros::Publisher vel_pub_;
    ros::Subscriber input3_sub, input1_sub, input0_sub;
    ros::Timer timer_;

    int rate_; // update and publish rate (Hz)
    
    bool input0, input3, input1;
    geometry_msgs::Twist vel_cmd3, vel_cmd1, vel_cmd0;


    // methods
    void on_Input0_Received(geometry_msgs::Twist msg);
    void on_Input1_Received(geometry_msgs::Twist msg);
    void on_Input3_Received(geometry_msgs::Twist msg);
    
    
    void behaviour_Control();
    
    bool input3IsTrigerred();
    bool input0IsTrigerred();
    bool input1IsTrigerred();
    
    void do_publish_cmd3();  
    void do_publish_cmd1();
    void do_publish_cmd0();


};

///////////////////////////////////////////////////////////////////////////

Arbiter::Arbiter() {
    input0 = false; //teleop
    input1 = false; //escape
    input3 = false; //cruise
    rate_ = 10; //We had to make the rate lower to handle messages 
    vel_pub_ = nh_.advertise<geometry_msgs::Twist>(PUBLISHER_TOPIC, 1);
    input0_sub = nh_.subscribe<geometry_msgs::Twist>(INPUT0_TOPIC, 100, &Arbiter::on_Input0_Received, this);
    input1_sub = nh_.subscribe<geometry_msgs::Twist>(INPUT1_TOPIC, 100, &Arbiter::on_Input1_Received, this);
    input3_sub = nh_.subscribe<geometry_msgs::Twist>(INPUT3_TOPIC, 100, &Arbiter::on_Input3_Received, this);
    timer_ = nh_.createTimer(ros::Duration(1.0 / rate_), boost::bind(&Arbiter::behaviour_Control, this));
}

/* 
    Callbacks Events for inputs 
 */
void Arbiter::on_Input0_Received(geometry_msgs::Twist msg) {
    vel_cmd0 = msg;
    input0 = true;
}

void Arbiter::on_Input1_Received(geometry_msgs::Twist msg) {
    vel_cmd1 = msg;
    input1 = true;
}

void Arbiter::on_Input3_Received(geometry_msgs::Twist msg) {
    vel_cmd3 = msg;
    input3 = true;
}


/*
    Main routine using the timer event to control the robot 
 */
void Arbiter::behaviour_Control() {

     if(!input1IsTrigerred())      //Check priority I
     {
         if(input0IsTrigerred())   //Check priority II
         {
             do_publish_cmd0();
            
         }
         else                       // Do least priority
         {
             do_publish_cmd3();
         }    
     }
     else 
     {
         do_publish_cmd1();
     }    
}

/*Main Function*/
int main(int argc, char **argv) {
    ros::init(argc, argv, NODE_NAME);
    Arbiter arbiter;
    ros::spin();

    return 0;
}

/*
     Check if any input was triggered 
 */
bool Arbiter::input3IsTrigerred() { 
    return input3?true:false;
}
bool Arbiter::input1IsTrigerred() { 
    return input1?true:false;
}
bool Arbiter::input0IsTrigerred() { 
    return input0?true:false;
}


/* 
    If an input Event is triggered then do the corresponding behaviour below
 */
void Arbiter::do_publish_cmd3()
{
   vel_pub_.publish(vel_cmd3);
   ros::spinOnce();
   input3 = false;
   return;
}
void Arbiter::do_publish_cmd1()
{
   vel_pub_.publish(vel_cmd1);
   ros::spinOnce();
   input1 = false;
   return;
}
void Arbiter::do_publish_cmd0()
{
   vel_pub_.publish(vel_cmd0);
   ros::spinOnce();  
   input0 =false;
    return;
}