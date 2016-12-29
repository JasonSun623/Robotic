#include "node_wallFollowing.h"

#define SUBSCRIBER_BUFFER_SIZE 1	//!<Size of buffer for subscriber.

#define PUBLISHER_BUFFER_SIZE 1000	//!<Size of buffer for publisher.

// #define PUBLISHER_TOPIC "/syros/base_cmd_vel"

#define PUBLISHER_TOPIC "/cmd_vel"

// #define SUBSCRIBER_TOPIC "/syros/laser_laser"

#define SUBSCRIBER_TOPIC "/base_scan"


/*! \brief Starting node for task: "Wall Following"
 * 
 * In main function is created Subscribing node, which transmits messages 
 * to NodeWallFollowing object. There are the messages processed and commands 
 * generated.
 */

int main(int argc, char **argv)
{
	//Initialization of node
	ros::init(argc, argv, "wallFollowing");
	ros::NodeHandle n;
	
	//Creating publisher
	ros::Publisher pubMessage = n.advertise<geometry_msgs::Twist>(PUBLISHER_TOPIC, PUBLISHER_BUFFER_SIZE);
	
	//Creating object, which stores data from sensors and has methods for
	//publishing and subscribing
	NodeWallFollowing *nodeWallFollowing = new NodeWallFollowing(pubMessage, 0.13, 0.1, 1, 10, 0, 5, 1);
	
	//Creating subscriber and publisher
	ros::Subscriber sub = n.subscribe(SUBSCRIBER_TOPIC, SUBSCRIBER_BUFFER_SIZE, &NodeWallFollowing::messageCallback, nodeWallFollowing);
	ros::spin();

	return 0;
}
