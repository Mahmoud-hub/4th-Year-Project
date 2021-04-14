/****************************************************************************
CAN Read on PORT 1

 This code is designed to be the Recieving ECU module. The purpose of this 
 code is to collect the data being sent by the malicious and sending ECU. 

  CAN ID 1: Throttle control
  CAN ID 2: Braking Control
  CAN ID 3: Forward Colision Warning

*************************************************************************/
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
int timeCount = 0;
//********************************Setup Loop*********************************//
void setup() {
  Serial.begin(9600); // For debug use
  delay(1000);
  
  Canbus.init(CANSPEED_500);
  delay(1000);
}

//********************************Main Loop*********************************//
void loop(){
  tCAN message;//Recieving Message
  if (mcp2515_check_message()){
      if (mcp2515_get_message(&message)){
                 if(message.id == 0x1 && message.data[0] == 0x00){
                  timeCount = 0;
                 }
                 Serial.print(timeCount, DEC); //Current Time
                 Serial.print(":"); 
                 Serial.print(message.id,HEX); //ID is first printed
                 Serial.print(":");
                 Serial.print(message.data[0],DEC); //Data contents
                 Serial.print(":");
                 Serial.print(message.data[1],DEC); //Validation for if the code is malicious
                                                    //Used for validation and does not add 
                                                    //value for simulation
                 Serial.println("");
                 timeCount++;
      }
  }
}
