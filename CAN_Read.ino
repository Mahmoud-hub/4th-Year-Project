/****************************************************************************
CAN Read on PORT 1

CAN ID 1: Throttle control
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
  tCAN message;
  if (mcp2515_check_message()){
      if (mcp2515_get_message(&message)){
                 
                 if(message.data[0] <= 0x01) timeCount = 0;
                 Serial.print(timeCount, DEC);
                 Serial.print(":");
                 //Serial.print("ID: ");
                 //Serial.print(message.id,HEX);
                 //Serial.print(", ");
                 //Serial.print("Data: ");
                 
                 for(int i=0;i<message.header.length;i++){	
                    Serial.print(message.data[i],DEC);
                    //Serial.print(" ");
                  }
                 
                 Serial.println("");
                 timeCount++;
             }
  }
}
