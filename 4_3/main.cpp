#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include <cmath>
using namespace std;

DigitalOut led1(LED2);
Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial xbee(D1, D0);

BBCar car(pin5, pin6, servo_ticker);

//ping
BufferedSerial pc(USBTX, USBRX);
DigitalInOut ping(D10);
Thread t_ping;
void ping_function();
Timer t;

parallax_ping  ping1(ping);
//int is_stop = 1; 
//int is_stop2 = 0;

int is_complete = 0;
int is_complete2 = 0;
int judge1 = 0;
int judge2 = 0;
void calibration(Arguments *in, Reply *out);
RPCFunction line(&calibration, "calibration");

int main() {
   char buf[256], outbuf[256];
   FILE *devin = fdopen(&xbee, "r");
   FILE *devout = fdopen(&xbee, "w");
   xbee.set_baud(9600);
   //xbee.write("hello\r\n",10);
   while (1) {
      memset(buf, 0, 256);
      for( int i = 0; ; i++ ) {
         if(is_complete == 0)
         {
         char recv = fgetc(devin);
         if(recv == '\n') {
            printf("\r\n");
            break;
         }
         buf[i] = fputc(recv, devout);
         }
      }
      RPC::call(buf, outbuf);
      //memset(outbuf, 0, 256);
      //ThisThread::sleep_for(std::chrono::milliseconds(3000));
   }
}

void ping_function()
{
   while(1) {
      //printf("%f\r\n",(float)ping1);
      if((float)ping1>20) led1 = 1;
      else {
         led1 = 0;
         //car.stop();
         //break;
      }
      ThisThread::sleep_for(10ms);
   }
}

void calibration(Arguments *in, Reply *out)
{
    if(is_complete == 0)
    {
        is_complete = 1;
        float Tx = in->getArg<float>();
        float Tz = in->getArg<float>();
        float Ry = in->getArg<float>();
        Tz = abs(Tz)*6.2;
        Tx = Tx*6.2;
        printf("Tx = %f, Tz = %f, Ry = %f\r\n",Tx,Tz,Ry);
        
        float angle;
        int reverse_time;
        int forward_time;
        if(Ry>0 && Ry<90) reverse_time = int(abs(Tx)*60/cos(Ry/180*M_PI));
        else if(Ry<360 && Ry>270) reverse_time = int(abs(Tx)*60/cos((360-Ry)/180*M_PI));
        //steer the car

        t_ping.start(ping_function);
        if((Ry<=1)||(Ry>=359))
        {
            is_complete2 = 1;
            printf("ping: %f\r\n",float(ping1));
        }
        else
        {
        //if(((Tz-30) <= abs(Tx)) || float(ping1)<30)
        if(Tz>100)
        {
            /*if(Ry>0 && Ry<90)
            {
                car.turn(100,-0.15);
                ThisThread::sleep_for(std::chrono::milliseconds(1240));
                car.stop();
                ThisThread::sleep_for(std::chrono::milliseconds(500));
                car.goStraight(100);
                ThisThread::sleep_for(std::chrono::milliseconds(reverse_time));
                car.stop();
            }
            else if(Ry<360 && Ry>270)
            {
                car.turn(100,0.15);
                ThisThread::sleep_for(std::chrono::milliseconds(1240));
                car.stop();
                ThisThread::sleep_for(std::chrono::milliseconds(500));
                car.goStraight(100);
                ThisThread::sleep_for(std::chrono::milliseconds(reverse_time));
                car.stop();
            }*/
        }
        else //forwrad
        {
            if(Ry>0 && Ry<90) angle = Ry/2*1.3;
            else if(Ry<360 && Ry>270) angle = (360-Ry)/2*1.3;
            printf("rotation angle: %f degree\r\n",angle);
            forward_time = int(2500*angle/90); //? /180
            int time_res = forward_time + 500;
            int time_res2 = 1700 - time_res;
            int forward_time2;
            int forward_time3;
            if(Ry>0 && Ry<90)
            {
                //judge1 = judge2;
                //judge2 = 0;
                printf("left\r\n");
                if(Ry<5) forward_time2 = 6*60*tan(Ry/180*M_PI)*Tz;
                else if(Ry<9) forward_time2 = 4*60*tan(Ry/180*M_PI)*Tz;
                else forward_time2 = 2*60*tan(Ry/180*M_PI)*Tz;
                if(Ry<9) forward_time3 = 3*forward_time;
                else forward_time3 = 2*forward_time;
                //printf("%d %d\r\n",is_complete,is_complete2);
                //if(Ry < 5)
                //if(judge1 !=judge2)
                //{
                car.turn(-100,0.15);
                ThisThread::sleep_for(std::chrono::milliseconds(forward_time));
                car.stop();
                car.goStraight(-100);
                ThisThread::sleep_for(std::chrono::milliseconds(forward_time2));
                car.stop();
                ThisThread::sleep_for(std::chrono::milliseconds(500));
                car.turn(-100,-0.15);
                ThisThread::sleep_for(std::chrono::milliseconds(forward_time3));
                car.stop();
                //}
                //else 
                //{
                    //int time_res = forward_time + 500;
                    //ThisThread::sleep_for(std::chrono::milliseconds(time_res));
                //}
                //car.goStraight(-100);
                //ThisThread::sleep_for(std::chrono::milliseconds(300));
                //car.stop();
                //ThisThread::sleep_for(std::chrono::milliseconds(time_res2));
            }
            else if(Ry<360 && Ry>270)
            {
                //judge1 = judge2;
                //judge2 = 1;
                printf("right\r\n");
                if(Ry>355) forward_time2 = 6*60*tan((360-Ry)/180*M_PI)*Tz;
                else if(Ry>351) forward_time2 = 4*60*tan((360-Ry)/180*M_PI)*Tz;
                else forward_time2 = 2*60*tan((360-Ry)/180*M_PI)*Tz;
                if(Ry>351) forward_time3 = 3*forward_time;
                else forward_time3 = 2*forward_time; 
                //printf("%d %d\r\n",is_complete,is_complete2);
                //if(judge1 !=judge2)
                //if( Ry>355)
                //{
                //car.turn(-100,0.15);
                //ThisThread::sleep_for(std::chrono::milliseconds(forward_time));
                //car.stop();
                //ThisThread::sleep_for(std::chrono::milliseconds(500));
                //}
                car.turn(-100,-0.15);
                ThisThread::sleep_for(std::chrono::milliseconds(forward_time));
                car.stop();
                car.goStraight(-100);
                ThisThread::sleep_for(std::chrono::milliseconds(forward_time2));
                car.stop();
                ThisThread::sleep_for(std::chrono::milliseconds(500));
                car.turn(-100,0.15);
                ThisThread::sleep_for(std::chrono::milliseconds(forward_time3));
                car.stop();
                //else
                //{
                //    ThisThread::sleep_for(std::chrono::milliseconds(time_res));
                //}
                //car.goStraight(-100);
                //ThisThread::sleep_for(std::chrono::milliseconds(300));
                //car.stop();
                //ThisThread::sleep_for(std::chrono::milliseconds(time_res2));
            } 
        }
        }
        is_complete = 0;
        printf("ping: %f\r\n",float(ping1));
    }
}