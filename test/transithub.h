#ifndef LIBS_MOTION_TRANSITHUB_H_
#define LIBS_MOTION_TRANSITHUB_H_

#include <fstream>
#include <string>
using namespace std;

#define LINUX
#define FAILURE 0
#define SUCCUSS 1
#define NONE 2
#define REPLY 3



//data packets enum
enum packet_types
{
    PACKET_NULL,		//0
    PACKET_MOTION,		//1
    PACKET_PING,		//2
    PACKET_CHECK,		//3
    PACKET_SET_INIT,	//4
    PACKET_GYRO_PRESS,	//5
    PACKET_LOCK,		//6
};

class transitHub
{
private:
    bool bOpen;
    int fd;


public:
    transitHub();
    ~transitHub();

    bool portOpen(int comNumber);
    //"COM1","COM2" or "/dev/ttyS0"
    bool isOpen();
    bool portClose();

    int transmit(int *data,packet_types type = PACKET_MOTION);	//!the interface is not quite good!
    //the return value refers to the actual number of the transmitted data
    //PACKET_MOTION£ºdata = {20 values, i.e. 20 motors's goal position}
    //PACKET_PING: data = {null}
    //PACKET_CHECK£ºdata = {2 values, i.e. 1st refers to the motor id, 2nd refers to the control word}
    //PACKET_SET_INIT£ºdata = {2 values, i.e. 1st refers to the motor id, 2nd refers to the goal position}
    //PACKET_GYRO_PRESS: data = {2 value, i.e. 1st refers to gyro if it's 0, to press if it's 1, 2nd refers to return command if it's 1, to NOT return command if it's 0}
    //PACKET_LOCK: data = {2 values, i.e. 1st refers to the motor id, 2nd refers to lock if it's 1, to unlock if it's 0}
    int receive(char *str,int length);
    //the return value refers to the actual number of the received data
    int makeStdData(char *str,int *data,packet_types type);
    //generate the result string(str) from the input(data), according to the packet type(type),return the length of the string


    void doRx(char *recv = NULL,int failureCount = 0);
    //basic task of receiving processes
   // void doLoopTx(int *body = NULL, int *camera = NULL);
    //basic task of transmitting processes
    void doWriteCommandTx(int idOrItem, int para, packet_types type);
    void doCheckTx(int id = 0,bool pos = false, bool temp = false, bool vol = false, bool load = false);

};
#endif // LIBS_MOTION_TRANSITHUB_H_
