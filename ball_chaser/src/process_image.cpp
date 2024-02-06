#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;


void drive_robot(float lin_x, float ang_z)
{
    // Pass the requested velocities to the service
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = linear_x;
    srv.request.angular_z = angular_z;

    if (!client.call(srv)) {
        ROS_ERROR("Failed to call service command_robot");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    
    int left = 0;
    int right = 0;
    int forward = 0;
    for (int i = 0; i < img.height * img.step; i += 3) {
        if (img.data[i] == white_pixel && img.data[i + 1] == white_pixel && img.data[i + 2] == white_pixel) {
            int col = (i % (img.width * 3)) / 3;
            if (col < img.width / 3) {
                left++;
            } else if (col < img.width * 2 / 3) {
                forward++;
            } else {
                right++;
            }
        }
    }
    if (left > right && left > forward) {
        drive_robot(0.0, 0.5);
    } else if (right > left && right > forward) {
        drive_robot(0.0, -0.5);
    } else if (forward > left && forward > right) {
        drive_robot(0.5, 0.0);
    } else {
        drive_robot(0.0, 0.0);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}