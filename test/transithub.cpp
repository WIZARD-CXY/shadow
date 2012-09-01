#include "transithub.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
using namespace std;

#include<sys/ioctl.h>
#include<fcntl.h>
#include<termios.h>
#include<string.h>


transitHub::transitHub():bOpen(false),fd(-1)
{
}

transitHub::~transitHub()
{
    portClose();
}

bool transitHub::portOpen(int comNumber)
{

    portClose();
    stringstream conv;
    string numberStr;
    conv<<comNumber;
    conv>>numberStr;

    ::sleep(1);
    struct termios newtio;
    string devStr = string("/dev/ttyS")+(numberStr);
    fd = open(devStr.c_str(), O_RDWR | O_NOCTTY );//|O_NONBLOCK);
    if (fd <0)
    {
        bOpen = false;
        return false;
    }
    else
        bOpen =true;
    bzero(&newtio, sizeof(newtio));

    newtio.c_cflag = B57600 | CS8 | CLOCAL | CREAD;


    newtio.c_iflag = 0;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);

    return true;
}
bool transitHub::isOpen()
{
    return bOpen;
}
bool transitHub::portClose()
{
    if(isOpen())
        close(fd);

    bOpen = false;

    return true;
}
int transitHub::transmit(int *data,packet_types type)
{

    char str[200];
    int length = makeStdData(str,data,type);
#ifdef GAITDEBUG
    static ofstream dataTransmitted("./debugLogs/dataTransmitted.txt");

    for(int i = 0; i<length; i++)
        dataTransmitted<<hex<<setiosflags(ios::uppercase)<<static_cast<unsigned int>(static_cast<unsigned char>(str[i]))<<" ";
    dataTransmitted<<endl;
#endif

    //tcflush(fd,TCIFLUSH);/////////////
    int leftBytes;
    ioctl(fd,FIONREAD,&leftBytes);
    if(leftBytes>0)
    {
       char *temp = new char[leftBytes+1];
       receive(temp,leftBytes);
       delete []temp;
      //  //GAITDEBUG
      //  static ofstream wakeUpRecord("./debugLogs/wakeUpRecord.txt");
      //  wakeUpRecord<<getGaitTime()<<"    receive buffer left, problem?   "<<leftBytes<<" more chars!"<<endl;
    }

    //////////////////////////////////
    //NON_BLOCK
//    int i = 0,j;
//    while((j=write(fd, str+i, length-i))<length-i)
//    {
//        j=j<0?0:j;
//        i+=j;
//    }
//    int nwrite = length;
    //////////////////////////////////
    int nwrite = write(fd, str, length);
    //tcdrain(fd);
    if (nwrite == -1)
    {
#ifdef GAITDEBUG
        cout<<" Write SerialPort Error! Occur in function \"transmit\""<<endl;
#endif
    }
    return nwrite;

}

int transitHub::receive(char *str,int length)
{
    int actualLength;

    struct termios options;
    tcgetattr(fd, &options);
    options.c_cc[VMIN] = length;
    options.c_cc[VTIME] = 1;
    tcsetattr(fd,TCSANOW,&options);

    actualLength = read(fd, str, length);
    str[actualLength] = '\0';
    /////////////////////////////
//    int i = 0,j;
//    while((j=read(fd, str+i, length-i))<length-i)
//    {
//        j=j<0?0:j;
//        i+=j;
//    }
//    actualLength = length;
    /////////////////////////////

    return actualLength;
}
int transitHub::makeStdData(char *str,int *data,packet_types type)
{
    int len=0;
    str[len++] = (char)(0xff);				//start with "0xff 0xff"
    str[len++] = (char)(0xff);
    str[len++] = type;						//the type, i.e. 0x01,0x02,0x03... see more in pc_mega_com_protocol


    if(type == PACKET_SET_INIT)		//4.PACKET_SET_INIT
    {
        char sum = 0;
        str[len++] = data[0] & 0x00ff;
        str[len++] = (data[1]>>8) & 0x00ff;
        str[len++] = data[1] & 0x00ff;
        sum += str[len-1] + str[len-2] + str[len-3];
        str[len++] = (~sum & 0x00ff);
        str[len] = '\0';
        return len;
    }

    else if(type == PACKET_LOCK)			//6.PACKET_LOCK
    {
        str[len++] = data[0] & 0x00ff;
        str[len++] = data[1] & 0x00ff;
        str[len] = '\0';
        return len;
    }
    return 0;
}

void transitHub::doRx(char *recv /* = NULL */,int failureCount /* = 0 */)
{
    char recBuffer[200];
    char type;
    int count = failureCount;
    while(failureCount==0?1:count--)
    {
        //step-1//head////////////////////////////////////
        receive(recBuffer,1);
        if(recBuffer[0] != (char)(0xee))
        {
            continue;
        }
        receive(recBuffer,1);
        if(recBuffer[0] != (char)(0xee))
        {
            continue;
        }
        //step-2//type//////////////////////////////////////
        receive(recBuffer,1);
        type = recBuffer[0];
        /*if((type & 0x80) != 0)
        {
        #ifdef GAITDEBUG
        	errorLogTH<<getGaitTime()<<" type error! Occur in function \"doRx\""<<endl;
        #endif
        	continue;
        }*/
        //step-3//gyro////////////////////////////////////
        if((type & 0x10) != 0)
        {
            char checksum = 0;
            receive(recBuffer,13);
            for(int i=0; i<13; i++)
            {
                checksum += recBuffer[i];
            }
            if((checksum&0x00ff) != 0x00ff)
            {
#ifdef GAITDEBUG
                cout<<" gyro error! Occur in function \"doRx\""<<endl;
#endif
                continue;
            }

        }
        //step-4//press/////////////////////////////////
        if((type & 0x20) != 0)
        {
            char checksum = 0;
            receive(recBuffer,17);
            for(int i=0; i<17; i++)
            {
                checksum += recBuffer[i];
            }
            if((checksum&0x00ff) != 0x00ff)
            {
#ifdef GAITDEBUG
               cout<<" press error! Occur in function \"doRx\""<<endl;
#endif
                continue;
            }

        }
        //step-5//motor pos////////////////////////////////////
        if((type & 0x01) != 0)
        {
            char checksum = 0;
            receive(recBuffer,1);
            checksum += recBuffer[0];
            int len = static_cast<unsigned char>(recBuffer[0]) - 1;
            receive(recBuffer,len);
            for(int i=0; i<len; i++)
            {
                checksum += recBuffer[i];
            }
            if((checksum&0x00ff) != 0x00ff)
            {
#ifdef GAITDEBUG
               cout<<" motor pos error! Occur in function \"doRx\""<<endl;
#endif
                continue;
            }



        }

    }
}


//void transitHub::doLoopTx(int *body /* = NULL */, int *camera /* = NULL */)
//{
//#ifdef TEST_ZJB
//    static ofstream timeTest("./debugLogs/timeTest.txt");
//    gettimeofday(&tpend,NULL);
//    static long x1 ;
//    x1 = 1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
//    if(x1>100000)
//        timeTest<<getGaitTime()<<"    x1:when end    "<<x1<<endl;
//    gettimeofday(&tpstart,NULL);
//    //::usleep(100);
//#endif
//
//
//    char recv[3];					//for storing the received data
//
//    transmit(store_data);
//
//#ifdef TEST_ZJB
//    gettimeofday(&tpend,NULL);
//    static long x2 ;
//    x2 = 1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
//    if(x2>100000)
//        timeTest<<getGaitTime()<<"    x2:after 1st transfer    "<<x2<<endl;
//#endif
//
//    while(1)
//    {
//        doRx(recv);
//        if(recv[1]==(char)NONE)	//if lost, redo
//        {
//            transmit(store_data);
//#ifdef GAITDEBUG
//            Motiondata<<getGaitTime()<<"	"<<"lost"<<endl;
//#endif
//            continue;
//        }
//        if(recv[1]==(char)FAILURE)	//if error, redo
//        {
//            transmit(store_data);
//#ifdef GAITDEBUG
//            Motiondata<<getGaitTime()<<"	"<<"error"<<endl;
//#endif
//            continue;
//        }
//
//#ifdef GAITDEBUG
//        Motiondata<<getGaitTime()<<" ";
//        for(int i=0; i<MOTORNUM; i++)
//        {
//            Motiondata<<store_data[i]<<" ";
//        }
//        Motiondata<<endl;
//#endif
//
//#ifdef TEST_ZJB
//        gettimeofday(&tpend,NULL);
//        static long x3 ;
//        x2 = 1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
//        if(x3>100000)
//            timeTest<<getGaitTime()<<"    x3:after 1st receive    "<<x3<<endl;
//#endif
//
//        while(recv[0]==(char)'O')//when the lower control board doesn't require for data, just ping
//        {
//            doCheckTx();
//            doRx(recv);
//#ifdef TEST_ZJB
//            gettimeofday(&tpend,NULL);
//            static long x4 ;
//            x4 = 1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
//            if(x4>100000)
//                timeTest<<getGaitTime()<<"    x4:after ping    "<<x4<<endl;
//#endif
//        }
//
//        break;
//    }
//#ifdef TEST_ZJB
//    gettimeofday(&tpend,NULL);
//    static long x5 ;
//    x5 = 1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
////    MotiondataReturned<<getGaitTime()<<"    loop end time used:"<<x1<<"    loop out time used:"<<x4<<endl;
//    if(x5>100000)
//        timeTest<<getGaitTime()<<"    x5:when out    "<<x5<<endl;
//#endif
//
//    //pthread_mutex_lock(&m_lock);
//    //pthread_mutex_unlock(&m_lock);
//}

void transitHub::doCheckTx(int id /* = 0 */,bool pos /* = false */, bool temp /* = false */, bool vol /* = false */, bool load /* = false */)
{
    int data[2];
    data[0] = id;
    data[1] = (pos?0x01:0)|(temp?0x02:0)|(vol?0x04:0)|(load?0x08:0);
    if(data[0] != 0 && data[1] != 0)
    {
        transmit(data,PACKET_CHECK);
    }
    else
    {
        transmit(NULL,PACKET_PING);
    }
}
void transitHub::doWriteCommandTx(int idOrItem, int para, packet_types type)
{
    //PACKET_SET_INIT£ºdata = {2 values, i.e. 1st refers to the motor id, 2nd refers to the goal position}
    //PACKET_GYRO_PRESS: data = {2 value, i.e. 1st refers to gyro if it's 0, to press if it's 1, 2nd refers to return command if it's 1, to NOT return command if it's 0}
    //PACKET_LOCK: data = {2 values, i.e. 1st refers to the motor id, 2nd refers to lock if it's 1, to unlock if it's 0}
    int data[2];
    data[0] = idOrItem;
    data[1] = para;
    if(type == PACKET_SET_INIT || type == PACKET_GYRO_PRESS || type == PACKET_LOCK)
        transmit(data,type);
    else
        return;
}

