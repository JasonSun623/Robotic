#include <ros/ros.h>
#include "std_msgs/String.h"
#include "assignment5/triangle.h"
#include <math.h>
#include "assignment5/triangle.h"
#include <stdlib.h>     /* srand, rand */
#include <iomanip>      // std::setprecision


#define BUFFER_SIZE 1   
#define SPEED 0.15 
#define PUBLISHER_MSG_TOPIC "cmd"
#define NODE_NAME "publisher_triangle"

using namespace std;


  assignment5::triangle getTriangle();
  char nodeName []= NODE_NAME;
  ros::Publisher thePublisher;
  assignment5::triangle getTriangle()
  {
      double side, clw;
      string cl;
       /* initialize random seed: */
       srand( time(NULL) ); //Randomize seed initialization
       clw = rand() % 2; // Generate a random number between 0 and 1      
       if(clw==0)
       {
           clw=false; 
           cl="false";
       }
       else
       {
           clw=true; 
           cl="true";      
       }      
        srand( time(NULL) ); //Randomize seed initialization
        side = rand() % 5+1; // Generate a random number between 0 and 1 
        
        
      assignment5::triangle triangleMsg;
 
      triangleMsg.sideLength =side;
      triangleMsg.clw=clw;
      ROS_INFO_STREAM("Forward distance: " << triangleMsg.sideLength);
      ROS_INFO_STREAM("Clockwise: " <<cl);
    return triangleMsg;
    
  }

  int main(int argc, char  *argv[])
  {

  	ros::init(argc,argv,nodeName);
  	ros::NodeHandle nh;
  	thePublisher = ros::Publisher(nh.advertise<assignment5::triangle>(PUBLISHER_MSG_TOPIC,BUFFER_SIZE));
        
        sleep(3.0);
        thePublisher.publish(getTriangle());
        ROS_INFO_STREAM("TRIANGLE PLANNER: triangle published !!");
        
  	ros::spinOnce();
  	return 0;
  }



