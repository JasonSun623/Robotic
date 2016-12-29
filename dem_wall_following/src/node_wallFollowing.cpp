#include "node_wallFollowing.h"
#include <math.h>
#define PI 3.141592		//!<Mathematical constant (default value: 3.141592).	

//Constructor and destructor
NodeWallFollowing::NodeWallFollowing(ros::Publisher pub, double wallDist, double maxSp, int dir, double pr, double su, double di, double an)
{
	wallDistance = wallDist;
	maxSpeed = maxSp;
	direction = dir;
	P = pr;
	S = su;
	D = di;
	angleCoef = an;
	r = 0;
	sumR = 0;
	distMin = 0;	//minimum distance masured by sensor
	angleMin = 0;	//angle, at which was measured the shortest distance
	go = 1;			//in case of obstacle, change to 0
	pubMessage = pub;
}

NodeWallFollowing::~NodeWallFollowing()
{
}

//Publisher
void NodeWallFollowing::publishMessage()
{
	//preparing message
	geometry_msgs::Twist msg;
	
	msg.angular.z = direction*(P*r + S*sumR + D*diffR) + angleCoef * (angleMin - PI*direction/2);		//PI regulator
	
	if (distFront < wallDistance){
		msg.linear.x = 0;
	}
	else if (distFront < wallDistance * 2){
		msg.linear.x = 0.5*maxSpeed;
	}
	else if (fabs(angleMin)>1.75){
		msg.linear.x = 0.4*maxSpeed;
	}
	else {
		msg.linear.x = maxSpeed;
	}
	
	//sending information about message to console
	ROS_INFO("Sending msg: linear.x=%f, angular.z=%f",msg.linear.x,msg.angular.z);
	
	//publishing message
	pubMessage.publish(msg);
}

//Subscriber
void NodeWallFollowing::messageCallback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
	//Calculation of array size from angle range and angle increment.
	//int size = (int) ((msg->angle_max - msg->angle_min)/msg->angle_increment);
	int size = msg->ranges.size();
	//ROS_INFO("array_length: [%d]", size); 
	
	//Variables whith index of highest and lowest value in array.
	int minIndex = size*(direction+1)/4;
	
	//This cycle goes through array and finds minimum
	for(int i=(size*(direction+1)/4); i<(size*(direction+3)/4); i++)
	{
		if (msg->ranges[i] < msg->ranges[minIndex] && msg->ranges[i] > 0.0){
			minIndex = i;
		}
	}
	
	//Calculation of angles from indexes and storing data to class variables.
	angleMin = (minIndex-size/2)*msg->angle_increment;
	distMin = msg->ranges[minIndex];
	distFront = msg->ranges[size/2];
	diffR = 2*(distMin - wallDistance) - r;
	r = distMin - wallDistance;
	sumR += r;
	
	//Sending info about processed data
	ROS_INFO("min: angle=%f, distance=%f, front=%f", angleMin, distMin, distFront);
	
	//Invoking method for publishing message
	publishMessage();
}


