#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include <cmath>

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial xbee(D1, D0);

BBCar car(pin5, pin6, servo_ticker);

void car_control(Arguments *in, Reply *out);
RPCFunction Car(&car_control, "park");

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

void car_control(Arguments *in, Reply *out)
{
   int x = in->getArg<int>();
   int y = in->getArg<int>();
   char z = in->getArg<char>();

   char direction[20];
   int d1 = x;
   int d2 = y;

   int time1 = (d2-7)*60;
   int time2 = (d1-3)*60; //south

   int time3 = (d1+12)*60; //north

   int time4 = (d2+20)*60; 
   int time5 = (d1+1)*60; // east

   int time6 = (d2+2)*60; //west

   if(z == 's')
   {
      printf("d1:%d, d2:%d, direction: south\r\n",d1,d2);
      car.turn(100, -0.15);
      ThisThread::sleep_for(std::chrono::milliseconds(1240));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));

      car.goStraight(100);
      ThisThread::sleep_for(std::chrono::milliseconds(time1));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));

      car.turn(100, 0.15);
      ThisThread::sleep_for(std::chrono::milliseconds(1240));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));

      car.goStraight(100);
      ThisThread::sleep_for(std::chrono::milliseconds(time2));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));      
   }

   else if(z == 'n')
   {
      printf("d1:%d, d2:%d, direction: north\r\n",d1,d2);
      car.turn(100, 0.15);
      ThisThread::sleep_for(std::chrono::milliseconds(1240));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));

      car.goStraight(100);
      ThisThread::sleep_for(std::chrono::milliseconds(time1));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));

      car.turn(100, 0.15);
      ThisThread::sleep_for(std::chrono::milliseconds(1240));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));

      car.goStraight(100);
      ThisThread::sleep_for(std::chrono::milliseconds(time3));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));      
   }

   else if(z == 'e')
   {
      printf("d1:%d, d2:%d, direction: east\r\n",d1,d2);

      car.goStraight(-100);
      ThisThread::sleep_for(std::chrono::milliseconds(time4));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));

      car.turn(100, -0.15);
      ThisThread::sleep_for(std::chrono::milliseconds(1200));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));

      car.goStraight(100);
      ThisThread::sleep_for(std::chrono::milliseconds(time5));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));      
   }

   else if(z == 'w')
   {
      printf("d1:%d, d2:%d, direction: west\r\n",d1,d2);

      car.goStraight(100);
      ThisThread::sleep_for(std::chrono::milliseconds(time6));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));

      car.turn(100, 0.15);
      ThisThread::sleep_for(std::chrono::milliseconds(1200));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));

      car.goStraight(100);
      ThisThread::sleep_for(std::chrono::milliseconds(time5));
      car.stop();
      ThisThread::sleep_for(std::chrono::milliseconds(500));      
   }
}