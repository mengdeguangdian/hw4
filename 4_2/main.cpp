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

int is_stop = 1; 
int is_stop2 = 0;

void line_detect(Arguments *in, Reply *out);
RPCFunction line(&line_detect, "line");

int main() {
   char buf[256], outbuf[256];
   FILE *devin = fdopen(&xbee, "r");
   FILE *devout = fdopen(&xbee, "w");
   while (1) {
      memset(buf, 0, 256);
      for( int i = 0; ; i++ ) {
         char recv = fgetc(devin);
         if(recv == '\n') {
            printf("\r\n");
            break;
         }
         buf[i] = fputc(recv, devout);
      }
   RPC::call(buf, outbuf);
   }
}

void ping_function()
{
   /*float val;
   pc.set_baud(9600);

   //ThisThread::sleep_for(std::chrono::seconds(1));
   while(1) 
   {
        printf("ping_function\r\n");
        ping.output();
        ping = 0; wait_us(200);
        ping = 1; wait_us(5);
        ping = 0; wait_us(5);

        ping.input();
        printf("value\r\n");
        while(ping.read() == 0);
        t.start();
        while(ping.read() == 1);
        val = t.read();
        val = val*1770.04f;

        printf("value\r\n");
        if(val > 10) led1 = 1;
        else 
        {
            led1 = 0;
            is_stop = 0;
            car.stop();
            break;
        }
        printf("Ping = %lf\r\n", val);
        t.stop();
        t.reset();
        ThisThread::sleep_for(std::chrono::milliseconds(100));
   }*/

   parallax_ping  ping1(ping);
   while(1) {
      //printf("%f\r\n",(float)ping1);
      if((float)ping1>20) led1 = 1;
      else {
         led1 = 0;
         if(is_stop2==1) is_stop = 0;
         //car.stop();
         //break;
      }
      ThisThread::sleep_for(10ms);
   }
}

void line_detect(Arguments *in, Reply *out)
{
   float xx1 = in->getArg<float>();
   float yy1 = in->getArg<float>();
   float xx2 = in->getArg<float>();
   float yy2 = in->getArg<float>();
   
   printf("%f, %f, %f, %f\r\n",xx1,yy1,xx2,yy2);
   float angle;
   int spin_time;
   angle = atan(abs((yy1-yy2)/(xx1-xx2)))*180/M_PI;
   printf("%f degree\r\n",angle);
   angle = 90-angle;
   spin_time = int(2500*angle/180);
   
   t_ping.start(ping_function);
   //car.goStraight(-100);
   //ThisThread::sleep_for(std::chrono::milliseconds(200));
   if(((xx1-xx2)*(yy1-yy2)) < 0)
   {
      car.turn(-100, 0.15);
      ThisThread::sleep_for(std::chrono::milliseconds(spin_time));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));
      is_stop2 = 1;
      while(is_stop)
      {
        car.goStraight(-100);
        ThisThread::sleep_for(std::chrono::milliseconds(200));
      }
      car.stop();
   }
   else if(((xx1-xx2)*(yy1-yy2)) > 0)
   {
      car.turn(-100, -0.15);
      ThisThread::sleep_for(std::chrono::milliseconds(spin_time));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));
      is_stop2 = 1;
      while(is_stop)
      {
        car.goStraight(-100);
        ThisThread::sleep_for(std::chrono::milliseconds(200));
      }
      car.stop();
   }
}