/*#include"mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

using namespace std::chrono;

DigitalInOut ping(D11);
Timer T;

BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(D1,D0); //tx,rx
Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
DigitalIn encoder(D12);
volatile int steps;
volatile int last;

BBCar car(pin5, pin6, servo_ticker);

int mode = 1;

char s;
int X1 = 0;
int X2 = 0;
int Y1 = 0;
int Y2 = 0;

int num, m;
double d;
double angle;

int A;
int i = 0, b = 0, e =0;
int j = 0, D = 0;
int BACK = 0;
int dot = 0, dot_ = 0;
int digit =0, digit_ =0;
char recv[1];
double L; 
// L: distance between car and Apriltag(horizontal)
// ang: angle from Apriltag to car

Thread t;
Thread B;

int NUM(char);


void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;

}

void cargo(){
    while(1){
        if(mode == 1 || mode == 3){
        ////////////line detection//////////////

            printf("1x1 = %3d, x2 = %3d, Y1 = %3d, Y2 = %3d\n",X1, X2, Y1, Y2);
            //ThisThread::sleep_for(1s);
            ThisThread::sleep_for(1s);

            if(Y2 != 0){
                ThisThread::sleep_for(10ms);
                printf("2x1 = %3d, x2 = %3d, Y1 = %3d, Y2 = %3d\n",X1, X2, Y1, Y2);
                double X = (double)X1-(double)X2;
                double Y = (double)Y2-(double)Y1;
                double P = X/Y;
                printf("X = %f, Y = %f\n", X, Y);
                printf("X/Y = %f\n", P);

                angle = abs((atan(P)*180)/3.14);    // calculate the angle of the line
                printf("angle = %f\n", angle);


                if(X2 > X1){       // if car at west, turn left
                    for (m = 0; m < 1.25 * angle; m++){
                        car.turn(200, 0.05);
                        ThisThread::sleep_for(9ms);
                    }
                    car.stop();
                }else{                  // if car at east, turn right

                    for (m = 0; m < 1.25 * angle; m++){
                        car.turn(200, -0.05);
                        ThisThread::sleep_for(9ms);
                    }
                    car.stop();
                }
                ThisThread::sleep_for(100ms);
                printf("end turn!!!\n");
        
            ThisThread::sleep_for(100ms);//(5s);
        ///////////////circle around the stuff////////////////////////
            while(A != 1 || mode == 3){
            float val;

            ping.output();
            ping = 0; wait_us(200);
            ping = 1; wait_us(5);
            ping = 0; wait_us(5);

            ping.input();
            while(ping.read() == 0);
            T.start();
            while(ping.read() == 1);
            val = T.read();
            printf("Ping = %lf\r\n", val*17700.4f);
            T.stop();
            T.reset();

        //ThisThread::sleep_for(1s);

            if(int(val*17700.4f) > 30){
                car.goStraight(-400);
                ThisThread::sleep_for(175ms);
            }else{
                if(mode == 3){
                    car.stop();
                    return;
                }else if(mode == 1){
                    car.stop();
                    printf("close!\n");
                    
                    for(i = 0; i < 6; i++){
                    // turn right
                    car.turn(200, -0.3);
                    A = 1;
                    ThisThread::sleep_for(400ms);
                    }
                    printf("end turn!\n");
                    car.stop();
                    /*
                    for(i = 0; i < 5; i++){
                    // turn right
                    car.goStraight(200);
                    ThisThread::sleep_for(200ms);
                    }*/
/*
                }
            }
            }
        ThisThread::sleep_for(200ms);


        //////////////////////////////////////
        if(A == 1 && mode == 1){
            for(j = 0; j < 70; j++){
            // turn right
            //car.goStraight(200);
            //ThisThread::sleep_for(350ms);
                car.turn(600, -0.1);
                ThisThread::sleep_for(350ms);

            //car.goStraight(80);
            //ThisThread::sleep_for(350ms);
            }
            car.stop();

            mode = 2;
            b = 0;
            X1 = 0;
            X2 = 0;
            Y1 = 0;
            Y2 = 0;
        //return;
        }
        }}else if(mode == 2){
            printf("L = %f\n", L);
            ThisThread::sleep_for(2s);

            if(L != 0){
                ThisThread::sleep_for(10ms);
                printf("OK, L=%f\n", L);

                for(j = 0; j < 3; j++){
                // turn right
                car.turn(150, -0.1);
                ThisThread::sleep_for(300ms);
                }
                car.stop();
                ThisThread::sleep_for(1s);

                int M = int(L);
                //printf("M=%d\n", M);
                if(L > 0.5){
                    for(j = 0; j < (int(70*M/23)); j++){  // 23cm
                    // move to the AprilTag center
                        car.goStraight(-100);
                        ThisThread::sleep_for(5ms);
                    }
                    printf("end back\n");

                    ThisThread::sleep_for(1s);
                    car.stop();
                    ThisThread::sleep_for(180ms);


                    for(j = 0; j < 4; j++){
                    // turn left
                        car.turn(180, 0.1);
                        ThisThread::sleep_for(180ms);
                    }
                    car.stop();

                    ThisThread::sleep_for(1s);


                    for(j = 0; j < 10; j++){
                        car.goStraight(-100);
                        ThisThread::sleep_for(180ms);
                    }

                    car.stop();
                    mode = 3;
                    ThisThread::sleep_for(1s);
                    i = 0;
                }else{
                    for(j = 0; j < 5; j++){
                        car.goStraight(-100);
                        ThisThread::sleep_for(180ms);
                    }
                    mode = 3;
                    ThisThread::sleep_for(1s);
                    i = 0;
                    
                }
            }
        }
        ThisThread::sleep_for(300ms);
    }
}


int NUM(char c){    // check if value from openMV number
    if (c == '0' || c == '1'|| c == '2'|| c == '3'
    || c == '4'|| c == '5'|| c == '6'|| c == '7'
    || c == '8'|| c == '9'|| c == '.') return 1;
    else return 0;
}


int main(){
    pc.set_baud(9600);
    encoder_ticker.attach(&encoder_control, 10ms);
    steps = 0;
    last = 0;

    t.start(cargo);


    uart.set_baud(9600);
    while(1){
        if(uart.readable()){
            uart.read(recv, sizeof(recv));
            
            s = recv[0];
            //printf("%c", recv[0]);

            //////////line mode
            if(s == 'L'){
                b++;
            }else if (s == 'y'){
                i++;
            }else if (s == '='){
                i++;
            }else if(s == 'x'){
                i++;
            }else if(s == ':'){
                b++;
            }
            //printf("%d\n", i);

            if(mode == 1 || mode == 3){  
            ///////store X1, X2, Y1, Y2 from openMV//////////
                if(i == 2 && isdigit(recv[0]) == 1){
                    X1 = X1 * 10 +((int)s) - 48;
                }else if (i == 4 && isdigit(recv[0]) == 1){
                    Y1 = Y1 * 10 +((int)s) - 48;
                }else if (i == 6 && isdigit(recv[0]) == 1){
                    X2 = X2 * 10 +((int)s) - 48;
                }else if (i == 8 && isdigit(recv[0]) == 1){
                    Y2 = Y2 * 10 +((int)s) - 48;
                } 
            }else if(mode == 2){             //////////////tag mode
                ///////store for L///////
                if(b == 2 && NUM(recv[0]) == 1){
                    if(s == '.'){
                        dot = 1;
                        digit = 1;
                    }
                    else if(s != '.' && dot != 1){
                        L = L * 10 +((int)s) - 48;
                    }else if(s != '.' && dot == 1){
                        L = L + (pow(0.1,digit))*(((int)s) - 48);
                        digit++;
                    }
                }
            }
            //////////////////  
            //ThisThread::sleep_for(10ms);     
        } 
    }
}
*/
#include"mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include "mbed_events.h"

using namespace std::chrono;

DigitalInOut ping(D11);
Timer T;

BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(D1,D0); //tx,rx
Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
volatile int steps;
volatile int last;
//***********/////XBee///**********/////
void cargo(Arguments *in, Reply *out);
RPCFunction rpccar(&cargo, "cargo");
static BufferedSerial xbee(D10, D9);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
EventQueue XQueue;
Thread X;
Thread X_;
/////////*******************////////

BBCar car(pin5, pin6, servo_ticker);

int mode = 1;

char s;
int X1 = 0, X2 = 0, Y1 = 0, Y2 = 0;     // line position

int num, m;
double d;
double angle;

int A;
int i = 0, b = 0, e =0;
int j = 0, D = 0;
int BACK = 0;
int dot = 0, dot_ = 0;
int digit =0, digit_ =0;
char recv[1];
double L; 
// L: distance between car and Apriltag(horizontal)

Thread t;
Thread B;

int NUM(char);      // to check whether message from openmv is number

// rpc function to make car run
void cargo(Arguments *in, Reply *out) {
    printf("start!cargo\n");
    printf("look for line...\n");
    while(1){
        if(mode == 1 || mode == 3){
        ////////////line detection//////////////

            printf("1x1 = %3d, x2 = %3d, Y1 = %3d, Y2 = %3d\n",X1, X2, Y1, Y2);
            //ThisThread::sleep_for(1s);
            ThisThread::sleep_for(1s);

            if(Y2 != 0){
                ThisThread::sleep_for(10ms);
                printf("2x1 = %3d, x2 = %3d, Y1 = %3d, Y2 = %3d\n",X1, X2, Y1, Y2);
                double X = (double)X1-(double)X2;
                double Y = (double)Y2-(double)Y1;
                double P = X/Y;
                printf("X = %f, Y = %f\n", X, Y);
                printf("X/Y = %f\n", P);

                angle = abs((atan(P)*180)/3.14);    // calculate the angle of the line
                printf("angle = %f\n", angle);

                if(X2 > X1){            // if car at west, turn left
                    for (m = 0; m < 1.25 * angle; m++){
                        car.turn(200, 0.05);
                        ThisThread::sleep_for(9ms);
                    }
                    car.stop();
                }else{                  // if car at east, turn right

                    for (m = 0; m < 1.25 * angle; m++){
                        car.turn(200, -0.05);
                        ThisThread::sleep_for(9ms);
                    }
                    car.stop();
                }
                ThisThread::sleep_for(100ms);
                printf("end turn!!!\n");
        
            ThisThread::sleep_for(100ms);//(5s);
        ///////////////circle around the stuff////////////////////////
            while(A != 1 || mode == 3){
                float val;

                ping.output();
                ping = 0; wait_us(200);
                ping = 1; wait_us(5);
                ping = 0; wait_us(5);

                ping.input();
                while(ping.read() == 0);
                T.start();
                while(ping.read() == 1);
                val = T.read();
                printf("Ping = %lf\r\n", val*17700.4f);
                T.stop();
                T.reset();

                // when car and stuff distance larger than 30, keep walking
                if(int(val*17700.4f) > 30){        
                    car.goStraight(-400);
                    ThisThread::sleep_for(175ms);
                }else{
                    // if at second line detection, arrived destination
                    if(mode == 3){
                        car.stop();
                        printf("arrived!\n");
                        return;
                    }else if(mode == 1){
                        car.stop();
                        printf("close enthough!start circling\n");
                        for(i = 0; i < 6; i++){
                            // turn right to get ready to circle
                            car.turn(200, -0.3);
                            A = 1;
                            ThisThread::sleep_for(400ms);
                        }
                        printf("end turn!\n");
                        car.stop();
                    }
                }
            }
        ThisThread::sleep_for(200ms);
        // after first line detection, start circling
        if(A == 1 && mode == 1){
            for(j = 0; j < 70; j++){
                car.turn(600, -0.1);
                ThisThread::sleep_for(350ms);
            }
            car.stop();
            printf("end circle\n");
            mode = 2;
            b = 0;
            X1 = 0;
            X2 = 0;
            Y1 = 0;
            Y2 = 0;
        }
        //////////////////////////////////////
        }}else if(mode == 2){
            // looking for Apriltag
            printf("L = %f\n", L);
            ThisThread::sleep_for(2s);

            if(L != 0){
                printf("start Apriltag\n");         // find Apriltag, start to move to it
                ThisThread::sleep_for(10ms);
                printf("OK, L=%f\n", L);

                for(j = 0; j < 3; j++){
                // turn right
                car.turn(150, -0.1);
                ThisThread::sleep_for(300ms);
                }
                car.stop();
                ThisThread::sleep_for(1s);

                int M = int(L);
                if(L > 0.5){
                    for(j = 0; j < (int(70*M/23)); j++){  
                    // move to the AprilTag center
                        car.goStraight(-100);
                        ThisThread::sleep_for(5ms);
                    }

                    ThisThread::sleep_for(1s);
                    car.stop();
                    ThisThread::sleep_for(180ms);

                    for(j = 0; j < 4; j++){
                    // turn left
                        car.turn(180, 0.1);
                        ThisThread::sleep_for(180ms);
                    }
                    car.stop();

                    ThisThread::sleep_for(1s);

                    for(j = 0; j < 10; j++){
                        car.goStraight(-100);
                        ThisThread::sleep_for(180ms);
                    }

                    car.stop();
                    printf("on the Apriltag\n");
                    mode = 3;
                    ThisThread::sleep_for(1s);
                    printf("start second line detection\n");
                    i = 0;
                }else{
                    for(j = 0; j < 5; j++){
                        car.goStraight(-100);
                        ThisThread::sleep_for(180ms);
                    }
                    mode = 3;
                    ThisThread::sleep_for(1s);
                    i = 0;
                    
                }
            }
        }
        ThisThread::sleep_for(300ms);
    }
}


int NUM(char c){    // check if value from openMV number
    if (c == '0' || c == '1'|| c == '2'|| c == '3'
    || c == '4'|| c == '5'|| c == '6'|| c == '7'
    || c == '8'|| c == '9'|| c == '.') return 1;
    else return 0;
}

void XBee()
{
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

int main(){

    pc.set_baud(9600);
    steps = 0;
    last = 0;

    X.start(callback(&XQueue, &EventQueue::dispatch_forever));
    XQueue.call(&XBee);

    uart.set_baud(9600);
    while(1){
        if(uart.readable()){
            uart.read(recv, sizeof(recv));
            
            s = recv[0];

            //////////check what information from openmv
            if(s == 'L'){
                b++;
            }else if (s == 'y'){
                i++;
            }else if (s == '='){
                i++;
            }else if(s == 'x'){
                i++;
            }else if(s == ':'){
                b++;
            }

            if(mode == 1 || mode == 3){  
            ///////store X1, X2, Y1, Y2 from openMV//////////
                if(i == 2 && isdigit(recv[0]) == 1){
                    X1 = X1 * 10 +((int)s) - 48;
                }else if (i == 4 && isdigit(recv[0]) == 1){
                    Y1 = Y1 * 10 +((int)s) - 48;
                }else if (i == 6 && isdigit(recv[0]) == 1){
                    X2 = X2 * 10 +((int)s) - 48;
                }else if (i == 8 && isdigit(recv[0]) == 1){
                    Y2 = Y2 * 10 +((int)s) - 48;
                } 
            }else if(mode == 2){             //////////////tag mode
                ///////store for L///////
                if(b == 2 && NUM(recv[0]) == 1){
                    if(s == '.'){
                        dot = 1;
                        digit = 1;
                    }
                    else if(s != '.' && dot != 1){
                        L = L * 10 +((int)s) - 48;
                    }else if(s != '.' && dot == 1){
                        L = L + (pow(0.1,digit))*(((int)s) - 48);
                        digit++;
                    }
                }
            }
        } 
    }
}




