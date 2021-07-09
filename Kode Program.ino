
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
 
#define SS_PIN 10 //rfid
#define RST_PIN 9 //rfid
#define LED_G 5 //define green LED pin
#define LED_R 4 //define red LED
//#define PRO 2 // define pir sensor
#define SERVO 8 //define push Button
#define BUZZER 3 //define buzzer 
#define POTENSIO A0
#define ACCESS_DELAY 1000
#define DENIED_DELAY 500
#define trigPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define echoPin 7
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
Servo servo;
//JIKA TIDAK TAMPIL PADA LAYAR I2C GANTI ALAMAT SEPERTI DIBAWAH INI
//0x3F, 0x27, 0x20 ADALAH ALAMAT VENDOR I2C LCD
LiquidCrystal_I2C lcd(0x27, 16,2 );

int state = LOW;             // by default, no motion detected
int valB = 0;
int valP = 0;
int x = 0;
int y = 0;
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
bool access = false;
int derajat = 0;
String id;
int sensorValue = 0;
int z = 0;
int a = 0;
//String state = "Not Authorized"

void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();          // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);
  //pinMode(BTN, INPUT);
  //pinMode(PIR, INPUT);
  lcd.begin();
  lcd.backlight(); 
  servo.attach(8);
  servo.write(0);
  delay(1000);
  Serial.println("Put your card to the reader...");
  Serial.println();

}
void loop() 
{
  rfid();
  prox();
  Poten();
  Serial.println(derajat);
  
  //pir();
  //buttonCon();
}

void Poten(){
  sensorValue = analogRead(POTENSIO); //Mengecek nilai keluaran dari potensiometer
  
  /*Logika if yang mengecek apabila nilai keluaran potensiometer lebih dari 1000
  maka Pin 13 (pin buzzer dan led) akan mendapat tegangan, apabila di bawah itu
  maka Pin 13 akan kehilangan tegangan.*/
  if(sensorValue > 1000){
    if(derajat == 0 && a == 1){
      derajat = 90;
    servo.write(derajat);
    a = 0;
    delay(300);
    }else{}
  }else{
    //a = 1;
    if(derajat == 90 && z==1  ){
    derajat = 0;
    servo.write(derajat);
    a = 1;
    z = 0;
    delay(100);
    }else{}
  }
}

void rfid()
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  id = content.substring(1);
  if (content.substring(1) == "DA E6 66 19") //change here the UID of the card/cards that you want to give access
  { id = "DAE66619";
    Serial.println("Authorized access");
    Serial.println();
    a = 1;
    derajat = 90;
    servo.write(derajat);
    delay(100);
    LcdShow("Allowed", id);
    digitalWrite(LED_G, HIGH);
    tone(BUZZER, 100);
    delay(100);
    noTone(BUZZER);
    delay(100);
    tone(BUZZER, 100);
    delay(100);
    digitalWrite(LED_G, LOW);
    noTone(BUZZER);
    LcdShow("Allowed", id);
    //delay(ACCESS_DELAY);
    //digitalWrite(RELAY, HIGH);
  }
 
 else   {
    Serial.println(" Access denied");
    LcdShow("Denied", id);
    digitalWrite(LED_R, HIGH);
    delay(100);
    tone(BUZZER, 300);
    delay(DENIED_DELAY);
    digitalWrite(LED_R, LOW);
    noTone(BUZZER);
    LcdShow("Denied", id);
  }
}

void prox(){
  //valP = analogRead(sensorpin);       // reads the value of the sharp sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor 
  if(distance > 25 && derajat == 90 && y < 5)
  {
    //delay(1000);
    distance = 0;
    y = y + 1;
    }
    
 
    if(distance > 25 && derajat == 90)
    {
    delay(5000);  
    z == 1;
    derajat = 0;
    servo.write(derajat);
    if( y == 5){
      y = 0;
      }
    }
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(1000);
  
  //if(distance>=15 && access){
  //}
}

void LcdShow(String akses, String ID){
  if(akses == "Allowed"){
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Access Allowed");
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("WELCOME " + ID);
    delay(500);
    lcd.clear();
    }
  else if(akses == "Denied"){
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Access Denied");
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Wrong Card Used");
    delay(700);
    lcd.clear();
  }
  else{Serial.println("ERROR");}
}

/*void pir()
{
  Serial.println(valP);
  valP = digitalRead(PIR);   // read sensor value
  if (valP == HIGH) {           // check if the sensor is HIGH
    servo.write(0);
    delay(100);                // delay 100 milliseconds 
    
    if (state == LOW) {
      Serial.println("Motion detected!"); 
      state = HIGH;       // update variable state to HIGH
    }
  } 
  else {
      servo.write(90);
      //digitalWrite(led, LOW); // turn LED OFF
      delay(200);             // delay 200 milliseconds 
      
      if (state == HIGH){
        Serial.println("Motion stopped!");
        state = LOW;       // update variable state to LOW
    }
  }
}*/

/*void buttonCon()
{
  delay(500);
  valB = digitalRead(BTN);  // read input value
  Serial.println(valB);
  
  if (x = 0 && valB == HIGH) {         // check if the input is HIGH (button released)
    x = x + 1;
    servo.write(90);
    delay(500);
    //digitalWrite(ledPin, LOW);  // turn LED OFF
  } 
  else if (x = 1 && valB == HIGH) 
  {
    x = x - 1;
    servo.write(0);
    delay(500);
    //digitalWrite(ledPin, HIGH);  // turn LED ON
  }
  //else{}
}*/
