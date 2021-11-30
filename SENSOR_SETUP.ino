//DFRobot.com
//Compatible with the Arduino IDE 1.0
//Library version:1.1


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DFRobot_EC.h"
#include <EEPROM.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

LiquidCrystal_I2C lcd(0x20,16,2);  // set the LCD address to 0x20(Cooperate with 3 short circuit caps) for a 16 chars and 2 line display

#define turbidityPin A0 //Turbidity Pin

#define phPin A1      //pH meter Analog output to Arduino Analog Input 0
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;

#define ecPin A2 //Electrical Conductivity Pin
float voltage,ecValue,temperature = 25;
DFRobot_EC ec;

void setup(){
  Serial.println("Ready");
  Serial.begin(115200);
  ec.begin();
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
}
  
void loop(){
//  Turbidity value reading and computation
  int turbidity = analogRead(turbidityPin);

//  pH Value reading and computation
  for(int i=0;i<10;i++){       //Get 10 sample value from the sensor for smooth the value 
      buf[i]=analogRead(phPin);
      delay(10);
    }
    for(int i=0;i<9;i++){       //sort the analog from small to large
      for(int j=i+1;j<10;j++){
        if(buf[i]>buf[j]){
          temp=buf[i];
          buf[i]=buf[j];
          buf[j]=temp;
        }
      }
    }
    avgValue=0;
    for(int i=2;i<8;i++)                      //take the average value of 6 center sample
      avgValue+=buf[i];
    float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
    phValue=3.5*phValue;

// Electrical Conductivity reading and computation
   static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U)  //time interval: 1s
    {
      timepoint = millis();
      voltage = analogRead(ecPin)/1024.0*5000;   // read the voltage
      ecValue =  ec.readEC(voltage,temperature);  // convert voltage to EC with temperature compensation
      Serial.print("temperature:");
      Serial.print(temperature,1);
      Serial.print("^C  EC:");
      Serial.print(ecValue,2);
      Serial.println("ms/cm");
    }
    ec.calibration(voltage,temperature);  
  
//  LCD Display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TUR: ");
  lcd.print(turbidity);
  lcd.setCursor(0, 1);
  lcd.print("PH: ");
  lcd.print(phValue);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("E-C: ");
  lcd.print(ecValue);
  delay(2000);
}
