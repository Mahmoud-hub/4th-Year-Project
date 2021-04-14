/****************************************************************************
  CAN WRITE on PORT 0

 This code is designed to be the Sending ECU module. The purpose of this 
 code is to run a normal simulation of where an individual is applying
 the brakes. Also, in random simulation runs, an object is detected by
 the ECU and as such, the emergency brake is applied.

  CAN ID 1: Throttle control
  CAN ID 2: Braking Control
  CAN ID 3: Forward Colision Warning
*************************************************************************/
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
int modelRun = 0;
int count = 0;
int randomVal = 5;
int brakeStatusPercentage = 0x00;
int brakeRate = 0x00;
int currentCarSpeed = 0x3C;
int emerBRAKEFLAG = 0x00;
int runEmergencyBrake = 3;
int SimulationCounter = 0;
int emerStart = 1;

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
//A timer interrupt is being used to randomly trigger 
//the application of braking 
ISR(TIMER1_COMPA_vect){
 if(count >= randomVal){
   modelRun = 1; //Used to initiate braking
   count = 0;
   cli();
  }
 else{
   count++;
 }
}

//********************************Main Loop*********************************//
void loop(){
  tCAN message; //Current Car Speed Packet
  tCAN messageA; //Current Braking Percentage Packet
  tCAN messageB; //Forward Collision Warning Packet
  
  interrupts();
  hexConvertBrakeRate(); //Determine the brake rate used per execution
  
  //If the time for random number generator is met, the model will run
   if(modelRun == 1){
     carBrakeAdjust(); //Car braking percentage is adjusted
     carSpeedAdjust(); //Car speed is adjusted according to braking percentage
   }
 
  //Once the model reaches a speed of 0 km/hr, reset the simulation 
  //to repeat the process
  if(modelRun == 1 && currentCarSpeed <= 0x00){
    currentCarSpeed = 0x00; 
    brakeStatusPercentage = 0x00;
    modelRun = 0;
    SimulationCounter++;
    emerBRAKEFLAG = 0x00;
    randomVal = randomNum();
    emerStart = 1;

    //The following is used to determine if the emergency braking scenario will
    //be ran in this model run
    if(SimulationCounter >= runEmergencyBrake){
      runEmergencyBrake = randomNum();
      SimulationCounter = 0;
      emerBRAKEFLAG = 0x01; //If the simulation equally divides into the 
                            //randomValue generated, braking will 
                            //occur in this model run
    }
     sei();
  }

  //Current Car Speed Packet is Created and Sent over Datalines
  message.id = 0x1; //ID Number for car Speed
  message.header.rtr = 0;
  message.header.length = 2;
  message.data[0] = currentCarSpeed;
  message.data[1] = 0x00; //Bit used to identify this is not malicious, used to identify
                          //in code for verification and does not add value in simulation
  mcp2515_bit_modify(CANCTRL, (1 << REQOP2) | (1 << REQOP1) | (1 << REQOP0), 0);
  mcp2515_send_message(&message);

  //This had to be implemented as the model would sometimes go bellow 0 km/hr with the 
  //calculations so a 0 km/hr message is first transmitted and then the model is
  //officially reset back to 60 km/hr
  if(currentCarSpeed <= 0x00){
    currentCarSpeed = 0x3C;
  }

  delay(10);

  //Current Car Braking Percentage is Created and Sent over Datalines
  messageA.id = 0x2; //ID Number for car Brake
  messageA.header.rtr = 0;
  messageA.header.length = 2;
  messageA.data[0] = brakeStatusPercentage;
  messageA.data[1] = 0x00; //Bit used to identify this is not malicious, used to identify
                          //in code for verification and does not add value in simulation
  mcp2515_bit_modify(CANCTRL, (1 << REQOP2) | (1 << REQOP1) | (1 << REQOP0), 0);
  mcp2515_send_message(&messageA);

  delay(10);
  
  //Current Forward Collision Warning Reading is Created and Sent over Datalines
  messageB.id = 0x3; //ID Number for Forward Collision Waring (Emergency Brake Application)
  messageB.header.rtr = 0;
  messageB.header.length = 2;
  messageB.data[0] = emerBRAKEFLAG;
  messageB.data[1] = 0x00; //Bit used to identify this is not malicious, used to identify
                          //in code for verification and does not add value in simulation
  mcp2515_bit_modify(CANCTRL, (1 << REQOP2) | (1 << REQOP1) | (1 << REQOP0), 0);
  mcp2515_send_message(&messageB);

  delay(10);
}

//********************************carSpeedAdjust*********************************//
//This adjusts the current car speed
void carSpeedAdjust(){
  //If the model is running with no emergency braking
  if(emerBRAKEFLAG == 0x00){
    currentCarSpeed = (currentCarSpeed - ((0x01) * brakeStatusPercentage));
  }

  //If the model is running with emergency braking
  if(emerBRAKEFLAG == 0x01){
    currentCarSpeed = currentCarSpeed - 0xA;
  }
}

//********************************carBrakeAdjust********************************//
//Adjust the braking percentage so braking of the car can be simulated.
void carBrakeAdjust(){
  if(emerBRAKEFLAG == 0x00){
    brakeStatusPercentage = (brakeStatusPercentage + brakeRate);
  }
}

//********************************randomNum*************************************//
//Used to randomly generate the human reaction time from 1 to 6 miliseconds to then
//start a model iteration
int randomNum(){
  return (random(0, 6));
}

//**************************hexConvertBrakeRate*********************************//
//This is used to randomly determine the braking rate every iteration and 
//convert that value into HEX as the data packet contents for the CAN 
//messages is in HEX
int hexConvertBrakeRate(){
  switch(randomNum()){
    case 1:
      brakeRate = 0x01;
    break;
    case 2:
      brakeRate = 0x02;
    break;
    case 3:
      brakeRate = 0x03;
    break;
    case 4:
      brakeRate = 0x04;
    break;
    case 5:
      brakeRate = 0x05;
    break;
  }
}
