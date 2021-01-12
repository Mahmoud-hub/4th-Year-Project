/****************************************************************************
  CAN WRITE on PORT 0

  CAN ID 1: Throttle control
*************************************************************************/

#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
int modelRun = 0;
int count = 0;
int randomVal = 5;

tCAN message;

//********************************Setup Loop*********************************//
void setup(){
  Serial.begin(9600);
  delay(2000);

  Canbus.init(CANSPEED_500);
  delay(1000);

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624 * 0.25;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  cli();//disable interrupts
}
//********************************Timer ISR*********************************//

ISR(TIMER1_COMPA_vect){
 if(count >= randomVal){
   modelRun = 1;
   count = 0;
   cli();
  }
 else{
   count++;
 }
}

//********************************Main Loop*********************************//

void loop(){
  randomSeed(analogRead(A0));
  noInterrupts();
  message.id = 0x01; //formatted in HEX
  message.header.rtr = 0;
  message.header.length = 1; //formatted in DEC

  
  //If car is running at constant speed:
  if(modelRun == 0){
    message.data[0] = 0x3C;
  }
  interrupts();
  //If the time for random number generator is met
   if(modelRun == 1){
     carStopModel(); //The car stop model is ran
   }
 
  //Once all iterations of the message are read, reset message counter
  if(modelRun == 1 && message.data[0] <= 0x00){
    message.data[0] = 0x3C;
    modelRun = 0;
    randomVal = randomNum();
    sei();
  }

  mcp2515_bit_modify(CANCTRL, (1 << REQOP2) | (1 << REQOP1) | (1 << REQOP0), 0);
  mcp2515_send_message(&message);

  delay(10);
}

void carStopModel(){
  message.data[0] = (message.data[0] - 0x01);//HEX
}

int randomNum(){
  return random(0, 6);
}
