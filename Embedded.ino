

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <Servo.h>
Servo servo1;
Servo servo2;
//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 7
#define RST_PIN 11
MFRC522 mfrc522(SS_PIN, RST_PIN);


LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
// Pins
int nRainDigitalIn = 5;
int fuelLevelPin = A0;
int buzzer = 4;
int seatButton = 48;
int seatSwitch = 18;
bool isSeatBeltOn = false;
bool previousState = false;
int xKeyJoyStick = A1;                                               
int YKeyJoyStick = A2;                                               
int xPos;
int yPos;
int servo1Pin = 8;
int servo2Pin = 9;  
int initialPositionX = 90;
int initialPositionY = 90;
int forwardMotor1 = 40;
int backwardMotor1 = 41;

int lineDetector1 = 32;
int lineDetector2 = 48;
int outOfLaneLED = 31;
int engineButton = 36;


int speedPin = 2;
SemaphoreHandle_t lockSemaphore;
SemaphoreHandle_t engineSemaphore;
SemaphoreHandle_t unlockSemaphore;



void setup() {

  Serial.begin(9600);
  Wire.begin();
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
 lcd.noDisplay();
  lcd.setCursor(0,0);
  lcd.print("RAIN:");
  lcd.setCursor(7,0);
  lcd.print("BELT:");
  lcd.setCursor(0,1);
  lcd.print("Fuel:");
//  lcd.setCursor(5,0);
//  lcd.print("N"); 
 pinMode(engineButton,INPUT_PULLUP);
  pinMode(lineDetector1,INPUT);
   pinMode(lineDetector2,INPUT);
  pinMode(fuelLevelPin,INPUT);
  pinMode(seatButton,INPUT_PULLUP);
  pinMode(buzzer,OUTPUT);
  digitalWrite(buzzer,LOW);
  servo1.attach(servo1Pin);
  servo1.write(initialPositionX);
  servo2.attach(servo2Pin);
  servo2.write(initialPositionY);
  pinMode(forwardMotor1,OUTPUT);
   pinMode(backwardMotor1,OUTPUT);
  pinMode(xKeyJoyStick, INPUT) ;                     
  pinMode(YKeyJoyStick, INPUT) ;
  pinMode(outOfLaneLED,OUTPUT);
  pinMode(speedPin,OUTPUT);
  pinMode(seatSwitch,INPUT);
  
  analogWrite(speedPin,40);
  //digitalWrite(outOfLaneLED ,HIGH);
 // attachInterrupt(digitalPinToInterrupt(seatButton),changeSeatBelt,CHANGE);
   SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  lockSemaphore = xSemaphoreCreateBinary();
  engineSemaphore = xSemaphoreCreateBinary();
  unlockSemaphore = xSemaphoreCreateBinary();
  
 
  xTaskCreate(lcdTask, "LCD TASK", 1000, NULL, 1, NULL );
  xTaskCreate(servoTask,"SERVO TASK",1000,NULL,1,NULL);
  xTaskCreate(moveTask,"MOVE TASK",1000,NULL,2,NULL);
    xTaskCreate(rfidTask,"RIFD TASK",1000,NULL,4,NULL);
  xTaskCreate(engineTask,"ENGINE TASK",1000,NULL,3,NULL);


  //lockSemaphore =  SemaphoreCreateBinary();
  //xTaskCreate(seatBeltTask,"SEAT TASK",1000,NULL,1,NULL);
}


void changeSeatBelt() {
  if(!digitalRead(seatButton)&& !previousState) {
    previousState = true;
    isSeatBeltOn =!isSeatBeltOn;
  }
  else {
    if(digitalRead(seatButton))
      previousState = false;
    }
  }


void loop() {
  Serial.begin(9600);
   // Serial.println(digitalRead(buzzer));
   //Serial.println(digitalRead(outOfLaneLED ));
  //Serial.println(digitalRead(lineDetector1));
  delay(1000);
}


 void lcdTask( void *pvParameters){
  TickType_t xLastWakeTime;
  const char *pcTaskName = "LCD TASK";
  xLastWakeTime = xTaskGetTickCount();
  while (1) {
     xSemaphoreTake(engineSemaphore,portMAX_DELAY);
    if(digitalRead(seatSwitch)){
       lcd.setCursor(12,0);
        lcd.print("Y");
    }
    else {
       lcd.setCursor(12,0);
       tone(buzzer, 1000, 500);
       lcd.print("N");
    }
  if(!digitalRead(nRainDigitalIn)){
        lcd.setCursor(5,0);
        lcd.print("Y");
  }
   else {
   lcd.setCursor(5,0);
     lcd.print("N");
   }
   lcd.setCursor(5,1);
   int fuelReading = analogRead(fuelLevelPin);
//   String fuelLCD = fuelReading > 680 ? "F" : "E";
//   if(fuelReading >600)
//    fuelLCD = "H";
    if(fuelReading > 695){
      lcd.print("F");
    }
    if(fuelReading > 580 && fuelReading <695) {
      lcd.print("H");
    }

    if(fuelReading <580){
      lcd.print("E");
    }
   //lcd.print(fuelLCD);
   xSemaphoreGive(engineSemaphore);
   Serial.println(analogRead(fuelLevelPin));
  vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 250 ) );
 } 
}

void servoTask( void *pvParameters){
  TickType_t xLastWakeTime;
  const char *pcTaskName = "SERVO TASK";
  xLastWakeTime = xTaskGetTickCount();
  while(1) {
     xSemaphoreTake(engineSemaphore,portMAX_DELAY);
    int posX = analogRead (xKeyJoyStick) ;  
    int posY = analogRead (YKeyJoyStick) ;
    if(posX < 500 && initialPositionX > 0) {
      initialPositionX = initialPositionX - 10;
      servo1.write(initialPositionX);
    }
    if(posX > 540 && initialPositionX <180) {
      initialPositionX = initialPositionX + 10;
      servo1.write(initialPositionX);
    }
    if(posY < 500 && initialPositionY > 0) {
      initialPositionY = initialPositionY - 10;
      servo2.write(initialPositionY);
    }
    if(posY > 540 && initialPositionY <180) {
      initialPositionY = initialPositionY + 10;
      servo2.write(initialPositionY);
    }
    xSemaphoreGive(engineSemaphore);
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 250 ) );
  }
}

void moveTask(void *pvParameters) {
   TickType_t xLastWakeTime;
  const char *pcTaskName = "SERVO TASK";
  xLastWakeTime = xTaskGetTickCount();
    while(1) {
       xSemaphoreTake(engineSemaphore,portMAX_DELAY);
        digitalWrite(forwardMotor1,LOW);
  digitalWrite(backwardMotor1,HIGH);
      bool isOutOfLane = digitalRead(lineDetector1);
      if(isOutOfLane){
           digitalWrite(outOfLaneLED,LOW);
          
      }
      else {
         tone(buzzer, 1000, 500);
        digitalWrite(outOfLaneLED,HIGH);
         
      }
      
      xSemaphoreGive(engineSemaphore);
       vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
  }
  
}


void seatBeltTask(void *pvParameters) {
  
   TickType_t xLastWakeTime;
  const char *pcTaskName = "SERVO TASK";
  xLastWakeTime = xTaskGetTickCount();
   while(1){
    xSemaphoreTake(engineSemaphore,portMAX_DELAY);
    if(!digitalRead(seatButton)){
      isSeatBeltOn = !isSeatBeltOn;
    }
     xSemaphoreGive(engineSemaphore);
     vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(250));
   }
  
}


void rfidTask(void *pvParameters) {
 TickType_t xLastWakeTime;
 xLastWakeTime = xTaskGetTickCount();
 xSemaphoreGive(lockSemaphore);
 xSemaphoreTake(lockSemaphore,portMAX_DELAY);
xSemaphoreGive(unlockSemaphore);
 boolean isunLocked = false;
 while(1) {

  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) 
    {
  //Show UID on serial monitor
    Serial.print("UID tag :");
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
      {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
  Serial.println();
  //D7 3C 51 33

  Serial.print("Message : ");
  if (mfrc522.uid.uidByte[0] == 0xD7 && 
     mfrc522.uid.uidByte[1] == 0x3C &&
     mfrc522.uid.uidByte[2] == 0x51 &&
     mfrc522.uid.uidByte[3] == 0x33) //change here the UID of the card/cards that you want to give access
  {
    if(isunLocked) {
       xSemaphoreTake(unlockSemaphore,portMAX_DELAY);
       xSemaphoreTake(lockSemaphore,portMAX_DELAY);
       Serial.println("LOCKED");
       isunLocked = false;
       
    }
    else {
      Serial.println("Unlocked");
       isunLocked  = true;
      xSemaphoreGive(lockSemaphore);
      xSemaphoreGive(unlockSemaphore);  
     }
  }
    }
  }
   vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1500));
 }
}

void engineTask(void *pvParameters) {
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
      
  bool isEngine = false;
  while(1) {
     xSemaphoreTake(lockSemaphore,portMAX_DELAY);
    if(!digitalRead(engineButton)){
      if(isEngine){
         Serial.println("ENGINNE STOP");
          xSemaphoreGive(engineSemaphore);  
         xSemaphoreTake(engineSemaphore,portMAX_DELAY);
         xSemaphoreGive(unlockSemaphore);
         digitalWrite(forwardMotor1,HIGH);
         lcd.noDisplay();
         isEngine=false;
    }
    else {
         Serial.println("ENGINE START ");
         xSemaphoreGive(engineSemaphore);
         isEngine = true;
         xSemaphoreGive(unlockSemaphore);
         xSemaphoreTake(unlockSemaphore,portMAX_DELAY);   
         lcd.display();
     }
    }
    xSemaphoreGive(lockSemaphore);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
  }
  
  
}
