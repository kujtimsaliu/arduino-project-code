/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#include <Arduino.h>

// Define HC-SR04 Pin 
const int trigPin = 6; 
const int echoPin = 5; 



#define RST_PIN 9 
#define SS_PIN 10 

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
byte accessUID[4] = { 0xF3, 0x2D, 0xBE, 0x0D };
int greenPin = 3;
int redPin = 2;
int buzzerPin = 4;
Servo servo;  // create servo object to control a servo
void setup() {
  servo.attach(8);
  servo.write(0);

  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);  // Initialize serial communications with the PC
  while (!Serial)
    ;                                 // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                        // Init SPI bus
  mfrc522.PCD_Init();                 // Init MFRC522
  delay(4);                           // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void rotateServoSlowly(Servo servo, int fromAngle, int toAngle, int delayTime) {
    int step = (fromAngle < toAngle) ? 1 : -1; // Determine the step direction
    int pos = fromAngle;

    while ((step > 0) ? (pos <= toAngle) : (pos >= toAngle)) {
        servo.write(pos);
        delay(delayTime);
        pos += step;
    }
}
int boolcheck = 0;
int secondBool = 0;


void loop() {
        // servo.write(90);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Set the trigPin HIGH for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin; pulseIn returns the duration (length of the pulse) in microseconds
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters
  float distance = duration * 0.034 / 2;
 
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  if(distance < 13.00 && boolcheck == 0 && secondBool == 0){
    digitalWrite(redPin, HIGH);
    delay(400);
    digitalWrite(redPin, LOW);
    delay(200);
    digitalWrite(redPin, HIGH);
    secondBool=1;
 
  }

  if(distance > 10.00 && boolcheck == 1){
    delay(1000);
    delay(200);
    digitalWrite(greenPin, LOW);
    delay(200);
    digitalWrite(greenPin, HIGH);
    delay(400);
    digitalWrite(greenPin, LOW);
    delay(300);
    digitalWrite(greenPin, HIGH);
    delay(200);
    digitalWrite(greenPin, LOW);
    delay(150);
    digitalWrite(greenPin, HIGH);
    delay(150);
    digitalWrite(greenPin, LOW);
    delay(100);
    digitalWrite(greenPin, HIGH);
    delay(50);
    digitalWrite(greenPin, LOW);
    delay(350);

    boolcheck=0;
    // rotateServoSlowly(servo, 90, 0, 15); // Slowly rotate back from 180 to 0 degrees
    // servo.write(0);
    rotateServoSlowly(servo, 70, 0, 15); // Slowly rotate from 0 to 180 degrees

  }

  delay(500);

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
  // mfrc522.PICC_DumpToSerial(&(mfrc522.uid.uidByte));
  if (mfrc522.uid.uidByte[0] == accessUID[0] && mfrc522.uid.uidByte[1] == accessUID[1] && mfrc522.uid.uidByte[2] == accessUID[2] && mfrc522.uid.uidByte[3] == accessUID[3]) {
      Serial.println("Access Granted");
      // servo.write(70);
      rotateServoSlowly(servo, 0, 70, 15); // Slowly rotate from 0 to 180 degrees
      // servo.write(90);
      boolcheck = 1;
      secondBool=0;
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, HIGH);
      delay(1000); // Wait for 1 second before next card read
   
  } else {
    Serial.println("Access Denied");
    digitalWrite(redPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(2000);
    secondBool=0;
    digitalWrite(redPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }
  mfrc522.PICC_HaltA();
}