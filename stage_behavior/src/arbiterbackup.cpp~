#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#define ESCAPE_TOPIC "cmd_vel1"
#define CRUISE_TOPIC "cmd_vel3"
#define TELOP_TOPIC "cmd_vel0"
#define PUBLISHER_TOPIC "cmd_vel"

/**
 * Priority based arbiter, forwards highest priority commands
 */
enum State {
    TELEOP,
    CRUISE,
    ESCAPE
};

class Arbiter {
public:
    // members

    // methods
    Arbiter();

protected:
    ros::NodeHandle nh_;
    ros::Publisher vel_pub_;
    ros::Subscriber cruise_sub, escape_sub, teleop_sub;
    ros::Timer timer_;

    int rate_; // update and publish rate (Hz)
    
    bool teleop, cruise, escape;
    geometry_msgs::Twist cruise_vel_cmd, escape_vel_cmd, teleop_vel_cmd;


    // methods
    void publishVel(geometry_msgs::Twist cmd_vel);
    void onTelop_Received(geometry_msgs::Twist msg);
    void onEscape_Received(geometry_msgs::Twist msg);
    void onCruise_Received(geometry_msgs::Twist msg);
    void behaviour_Control();
    bool cruiseIsTrigerred();
    bool teleopIsTrigerred();
    bool escapeIsTrigerred();
    void do_publish_cruise();  
    void do_publish_escape();
    void do_publish_teleop();


};

///////////////////////////////////////////////////////////////////////////

Arbiter::Arbiter() {
    escape = false;
    cruise = false;
    teleop = false;
    rate_ = 30;
    vel_pub_ = nh_.advertise<geometry_msgs::Twist>(PUBLISHER_TOPIC, 1);
    teleop_sub = nh_.subscribe<geometry_msgs::Twist>(TELOP_TOPIC, 100, &Arbiter::onTelop_Received, this);
    escape_sub = nh_.subscribe<geometry_msgs::Twist>(ESCAPE_TOPIC, 100, &Arbiter::onEscape_Received, this);
    cruise_sub = nh_.subscribe<geometry_msgs::Twist>(CRUISE_TOPIC, 100, &Arbiter::onCruise_Received, this);
    timer_ = nh_.createTimer(ros::Duration(1.0 / rate_), boost::bind(&Arbiter::behaviour_Control, this));
}

void Arbiter::publishVel(geometry_msgs::Twist vel_cmd) {
   vel_pub_.publish(vel_cmd);
   ros::spinOnce();
    return;
}
//TIME BOXING 
void Arbiter::onTelop_Received(geometry_msgs::Twist msg) {
    teleop_vel_cmd = msg;
    teleop = true;

}

void Arbiter::onEscape_Received(geometry_msgs::Twist msg) {
    escape_vel_cmd = msg;
    escape = true;
}

void Arbiter::onCruise_Received(geometry_msgs::Twist msg) {
    cruise_vel_cmd = msg;
    cruise = true;
}

void Arbiter::behaviour_Control() {

     if(!teleopIsTrigerred())
     {
         if(cruiseIsTrigerred()&& !escapeIsTrigerred())
         {
             do_publish_cruise();
         }
         else
         {
             do_publish_escape();
         }    
     }
     else 
     {
         do_publish_teleop();
     }


}

int main(int argc, char **argv) {
    ros::init(argc, argv, "arbiter");
    Arbiter arbiter;
    ros::spin();

    return 0;
}

bool Arbiter::cruiseIsTrigerred() { 
    return cruise?true:false;
}
bool Arbiter::escapeIsTrigerred() { 
    return escape?true:false;
}
bool Arbiter::teleopIsTrigerred() { 
    return teleop?true:false;
}
void Arbiter::do_publish_cruise()
{
   vel_pub_.publish(cruise_vel_cmd);
   ros::spinOnce();
   cruise = false;
   return;
}
void Arbiter::do_publish_escape()
{
   vel_pub_.publish(escape_vel_cmd);
   ros::spinOnce();
   escape = false;
   return;
}
void Arbiter::do_publish_teleop()
{
   vel_pub_.publish(teleop_vel_cmd);
   ros::spinOnce();  
   teleop =false;
    return;
}
