#include <dht.h>
dht DHT;
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <AFMotor.h>
#include <Wire.h>
#include <LiquidCrystal.h>

//tells the arduino what each pin use is connected to
#define trigPin 13
#define echoPin 12
#define servoMainPin 10
#define BACKLIGHT_PIN  7
#define LED_ON  1
#define DHT11_PIN 5


Servo servoMain; // Define our Servo
AF_DCMotor motor2(2, MOTOR12_64KHZ); // create motor #2, 64KHz pwm
AF_DCMotor motor1(1, MOTOR12_64KHZ); // create motor #1, 64KHz pwm
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Addr, En, Rw, Rs, d4, d5, d6, d7, backlighpin, polarity
int i = 0;
int state = 0;

void setup() //Sets each defined pin as an input or output so that the arduino what to expect at each pin
{
  Serial.begin(9600); // set up Serial library at 9600 bps
    servoMain.attach(servoMainPin); // servo on digital pin 10
  pinMode(trigPin, OUTPUT); //Unltrasonic output
  pinMode(echoPin, INPUT); //Ultrasonic input
    motor2.setSpeed(200); // set the speed to 200
    motor1.setSpeed(200); // set the speed to 200
    lcd.begin (16,4); // initialize the lcd
    lcd.setBacklight(LED_ON); // Switch on the backlight
    state = 0;
  delay(2000); //wait 2 seconds
}

//Ping Function to get the distance from the HC-SR04
unsigned long ping()
{
   // Trigger the uSonic sensor (HC-SR04) to send out a ping
   long duration, distance; //Initial Distance measure
   digitalWrite(trigPin, LOW);  // Added this line
     delayMicroseconds(1); // Added this line
   digitalWrite(trigPin, HIGH); //  delayMicroseconds(1000); - Removed this line
     delayMicroseconds(1); // Added this line
   digitalWrite(trigPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   distance = (duration/2) / 29.1;
   delay(100);
   //clearlcd();
   return (distance);
}

//Forward Fuction sets motors to go forward
void forward()
{
      //Serial.println("Forward");    
      motor2.setSpeed(200);     // set the speed to 200
      motor1.setSpeed(200);     // set the speed to 200
      motor2.run(FORWARD); // going FORWARD
      motor1.run(FORWARD); // going FORWARD  
      delay(200);
      botstopF();            
}

//Right Function sets motors for right
void right()
{
      //Serial.println("right");    
      motor2.setSpeed(200);     // set the speed to 200
      motor1.setSpeed(200);     // set the speed to 200
      motor2.run(BACKWARD); // going BACKWARD
      motor1.run(FORWARD); // going FORWARD 
      delay(200);
      //botstopR();     
}

// Left Function sets motors for left
void left()
{        
      //Serial.println("left");    
      motor2.setSpeed(200);     // set the speed to 200
      motor1.setSpeed(200);     // set the speed to 200
      motor2.run(FORWARD); // going FORWARD
      motor1.run(BACKWARD); // going BACKWARD
      delay(200);
      //botstopL(); 
}

// Back Fnction sets motors going backwards
void back()
{
      //Serial.println("back");    
      motor2.setSpeed(200);     // set the speed to 200
      motor1.setSpeed(200);     // set the speed to 200
      motor2.run(BACKWARD); // going BACKWARD
      motor1.run(BACKWARD); // going BACKWARD  
      delay(100);
      botstopB();
}

//Function used to stop motors when going forward
void botstopF()
{ 
      motor2.setSpeed(0);     // set the speed to 200
      motor1.setSpeed(0);     // set the speed to 200
      motor2.run(FORWARD); // going forward
      motor1.run(FORWARD); // going forward
      delay(100);
}

//Function used to stop motors when going backwards, due to issue with general stop
void botstopB()
{   
      motor2.setSpeed(0);     // set the speed to 200
      motor1.setSpeed(0);     // set the speed to 200
      motor2.run(BACKWARD); // going forward
      motor1.run(BACKWARD); // going forward
      delay(100);
}

// Clear the LCD Display
void clearlcd()
{
  lcd.clear();
  lcd.home();
}

// Function to gather DHT11 sensor data for temp, returns reading
unsigned long DHTTemp()
{
  int chk = DHT.read11(DHT11_PIN);
  Serial.println("Temparature ");
  Serial.println(DHT.temperature, 1);
  return (DHT.temperature);
}

// Function to gather DHT11 sensor data for humi, returns reading
unsigned long DHTHum()
{
  int chk = DHT.read11(DHT11_PIN);
  Serial.println("Humidity " );
  Serial.println(DHT.humidity, 1);
  return (DHT.humidity);
}

// Function to gather data from the MQ-2 sensor, converts to voltage returns converted value as float
float MQ2()
{
  float sensor_volt;
  float sensorValue;
    sensorValue = analogRead(A0);
    sensor_volt = sensorValue/1024*5.0;
  Serial.println("MQ-2 Volt: " );
  Serial.println(sensor_volt);
  return (sensor_volt);
}

void loop()
{ 
  
  long int forwdis, leftdis, rightdis, temp, hum;
  float gas;
  
  // Used for connecting bluetooth device through bluetooth module
  if(Serial.available() > 0)
  {
    state = Serial.read();
    Serial.print("I received: ");
    Serial.println(state);
    delay(10);
  }
  
// When bluetooth connection sends 0 enters this state which is self navigation
if (state == '0') 
{
  gas = MQ2(); // Runs MQ2 function saves value in gas
  lcd.setCursor(6,1);
  lcd.print("Gas:"); // Display gas on LCD display
  lcd.print(gas); // Prints gas
  
  temp = DHTTemp(); // Runs DHTTemp function saves value in temp
  lcd.setCursor(6,2); 
  lcd.print("Temp:"); // Displays temp on LCD display
  lcd.print(temp); // Prints temp
  
  hum = DHTHum(); // Runs DHTHum function save value in hum
  lcd.setCursor(16,2);
  lcd.print("Humi:"); // Displays hum on LCD
  lcd.print(hum); // Prints Hum
  
  servoMain.write(90); // Centre Servo
  delay(150);
  forwdis = ping(); // Runs Ping fucntion reurns distance saves as forward distance
  Serial.println (forwdis); // Print forward distance
  
  i++;
  if (i == 8) // Used as LCD screen would ocasionally break, this could clear it fixing problem comment out if no issues
  {
    clearlcd();
    i = 0;
  }

  if (forwdis >= 12) //If the forward dis is greater than 12 cm keep going forward
  {
      forward();
      clearlcd();
      lcd.setCursor(7,0); 
      lcd.print("FORWARD"); // Display direction of movment
      lcd.setCursor(12,4); 
      lcd.print(forwdis); // Print distance in cm
      lcd.print("CM");    
  }

  else if (forwdis < 12)  //If distance is less than 12 cm then
  {
    back();
    
    // Look left and get left distance
    servoMain.write(165); 
    delay(150);
    leftdis = ping();
    Serial.println (leftdis);
    
    // Look right and get right distance
    servoMain.write(15);
    delay(150);
    rightdis = ping();
    Serial.println (rightdis);
    
    // Reset servo centre possition
    servoMain.write(90);
    delay(150);
      
      // If Left distance is greater than right then go left
      if (leftdis > rightdis) 
      {
        clearlcd();
        lcd.setCursor(7,0); 
        lcd.print("LEFT"); // Display direction of movment
        lcd.setCursor(13,4); 
        lcd.print(leftdis); // Print distance in cm
        left();
        
      }

      // If right distance is greater than left distance go right
      else if (rightdis > leftdis) 
      {
        clearlcd();
        lcd.setCursor(7,0); 
        lcd.print("RIGHT"); // Display direction of movment
        lcd.setCursor(13,4); 
        lcd.print(rightdis); // Print distance in cm
        right();
        
    }
    
      // If each are the same then go backwards and clear LCD
      else if (rightdis == leftdis) 
      {
        clearlcd();
        back();  
      }   
  }
}

// If bluetooth connection sends 1 change state into idle mode in this mode motors pause while sensors still read
else if (state == '1')      // If 1 Forwards
    {
      motor2.run(RELEASE);  
      motor1.run(RELEASE);  
      
      clearlcd();
        lcd.setCursor(7,0); 
        lcd.print("IDLE"); // Display direction of movment
        Serial.println ("IDLE");
        
      gas = MQ2();
      lcd.setCursor(6,1);
      lcd.print("Gas:"); 
      lcd.print(gas);
        
      temp = DHTTemp();
      lcd.setCursor(6,2); 
      lcd.print("Temp:");
      lcd.print(temp);
      
      hum = DHTHum();
      lcd.setCursor(16,2);
      lcd.print("Humi:"); 
      lcd.print(hum);
      
      motor2.run(RELEASE);   
      motor1.run(RELEASE);  
      delay(1000);
      
    }
    
}
