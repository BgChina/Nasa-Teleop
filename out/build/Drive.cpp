#include <iostream>
// for delay function.
#include <chrono>
#include <map>
#include <string>
#include <thread>

// for signal handling
#include <signal.h>
#include "Serial.h"
#include <JetsonGPIO.h>

using namespace std;
const map<string, int> output_pins{
    {"JETSON_XAVIER", 18}, {"JETSON_NANO", 33}, {"JETSON_NX", 33}, {"CLARA_AGX_XAVIER", 18}, {"JETSON_TX2_NX", 32},
};

/*
   * Init rover packet publisher
   * @1st param: tells what the topic name should be used for ehat we are publishing
   * @2nd param: sets the queue size that will be held for this topic, we are using 1 to ensure we always get the newest packet
   */
m_rov_pub_ = nh_.advertise<controller::Rover>("rover", 1);

/*Setting up to listen to the controller and IR nodes respectively
 * @1st param: name of the topic we want to listen to updates on
 * @2nd param: sets the queue size that we want to maintain here, using 1 for newest packet
 * @3rd param: location of the callback function that will be called when there is an update
 * @4th param: what we want to pass as context or parameters to the callback function this will pass the object we are in right now
 */
m_joy_sub_ = nh_.subscribe<controller::JoyCon>("joycon", 1, &Serial::joystickCallback, this);
}

int get_output_pin()
{
    if (output_pins.find(GPIO::model) == output_pins.end())
    {
        cerr << "PWM not supported on this board\n";
        terminate();
    }

    return output_pins.at(GPIO::model);
}

inline void delay(int s) { this_thread::sleep_for(chrono::seconds(s)); }

static bool end_this_program = false;

void signalHandler(int s) { end_this_program = true; }

int main(int argc, char** argv)
{

    if (argc > 2) {
        std::cerr << "Too many arguments!! Please only include USB Device path!" << std::endl;
        return 1;
    }
    if (argc < 2) {
        std::cerr << "Too few arguments!! Please include USB Device path! \n" <<
            "Command should look like: rosrun controller fpga_stream [path of usb device] \n" <<
            "ex. rosrun controller fpga_stream /dev/ttyUSB1" << std::endl;
        return 1;
    }

    ros::init(argc, argv, "Drive");
    //argv[1] should be path to usb interface
    Serial serial = Serial(argv[1]);

    ros::spin();
}