#include "mbed.h"

#include "fsl_port.h"

#include "fsl_gpio.h"

// #include "Thread.h"
// #include "Callback.h"

#define UINT14_MAX        16383

// FXOS8700CQ I2C address

#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0

#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0

#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1

#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1

// FXOS8700CQ internal register addresses

#define FXOS8700Q_STATUS 0x00

#define FXOS8700Q_OUT_X_MSB 0x01

#define FXOS8700Q_OUT_Y_MSB 0x03

#define FXOS8700Q_OUT_Z_MSB 0x05

#define FXOS8700Q_M_OUT_X_MSB 0x33

#define FXOS8700Q_M_OUT_Y_MSB 0x35

#define FXOS8700Q_M_OUT_Z_MSB 0x37

#define FXOS8700Q_WHOAMI 0x0D

#define FXOS8700Q_XYZ_DATA_CFG 0x0E

#define FXOS8700Q_CTRL_REG1 0x2A

#define FXOS8700Q_M_CTRL_REG1 0x5B

#define FXOS8700Q_M_CTRL_REG2 0x5C

#define FXOS8700Q_WHOAMI_VAL 0xC7


I2C i2c( PTD9,PTD8);

Serial pc(USBTX, USBRX);

Timer debounce;

InterruptIn button(SW2);

DigitalOut redLED(LED1);

EventQueue queue(32*EVENTS_EVENT_SIZE);

// EventQueue queue1(32*EVENTS_EVENT_SIZE);

// EventQueue queue2(32*EVENTS_EVENT_SIZE);

Thread thread1;
Thread thread2;

int m_addr = FXOS8700CQ_SLAVE_ADDR1;

int flag = 0;

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);

void FXOS8700CQ_writeRegs(uint8_t * data, int len);


void toggle(){
    redLED = !redLED;
}


void logger(){

    if(debounce.read_ms()>1000){
        
        flag = 1;
        queue.call_every(500,toggle);
        debounce.reset();

    }

}

void data_thread(){
   pc.baud(115200);


   uint8_t who_am_i, data[2], res[6];

   int16_t acc16;

   float t[3];
   float data_x[100],data_y[100],data_z[100];
   int wait_time = 0;
   float distance = 0;
   int log[100];


   // Enable the FXOS8700Q


   FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);

   data[1] |= 0x01;

   data[0] = FXOS8700Q_CTRL_REG1;

   FXOS8700CQ_writeRegs(data, 2);


   // Get the slave address

   FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);


   while (true) {
      FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);
      acc16 = (res[0] << 6) | (res[1] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[0] = ((float)acc16) / 4096.0f;
      acc16 = (res[2] << 6) | (res[3] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[1] = ((float)acc16) / 4096.0f;
      acc16 = (res[4] << 6) | (res[5] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[2] = ((float)acc16) / 4096.0f;
      
      wait(0.1);

      if(wait_time > 0 && wait_time < 100){
        data_y[wait_time] = t[1];

         distance = distance + ((t[1]+9.8)*0.01)/2 ;

        if(distance > 5){
            log[wait_time] = 1;
        }
        else{
            log[wait_time] = 0;
        }
         pc.printf("%1.4f\n%d\n",distance,log[wait_time]);
      }

      // else{
      //     log = 0;      
      // }
      
      if(flag == 1){
        wait_time = wait_time + 1;          
      }
   }   
}

void button_thread(){
   debounce.start();
   button.rise(&logger);
   queue.dispatch(20000);
}

int main() {
    thread1.start(data_thread);
    thread2.start(button_thread);
}

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
   char t = addr;
   i2c.write(m_addr, &t, 1, true);
   i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
   i2c.write(m_addr, (char *)data, len);
}