#define BLYNK_TEMPLATE_ID "TMPL3vvGUNC6V"
#define BLYNK_TEMPLATE_NAME "Blynk Smart Dustbin"
#define BLYNK_AUTH_TOKEN "f7iq1Smv-vvFgN7cmgtmpXMUB-a7IR7F"
#define BLYNK_PRINT Serial

#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

BlynkTimer timer;
Servo servo;

#define servoPin 13  // GPIO pin for servo control

int distance = 0; 
int binLevel = 0;

const int binLevelThreshold = 80;  // Threshold for notification
bool alarmState = false;  // To keep track of alarm state

void send_blynk(const char* msg) {
  Blynk.logEvent("bin_level_exceeded", msg);  // Send notification to Blynk app
}

// Handle bin level input change from Blynk app
BLYNK_WRITE(V1) {
  binLevel = param.asInt();  // Get value from virtual pin V1
  // Simulate distance based on bin level
  distance = map(binLevel, 0, 100, 21, 0); // Assuming 21cm is empty and 0cm is full
  Serial.print("New bin level set: ");
  Serial.print(binLevel);
  Serial.print("%, Distance: ");
  Serial.println(distance);
}

void controlBin() {
  Serial.print("Bin Level: ");
  Serial.print(binLevel);
  Serial.print(" %, Distance: ");
  Serial.println(distance);

  Blynk.virtualWrite(V0, distance);  // Update distance value on Blynk app
  Blynk.virtualWrite(V1, binLevel);  // Update bin level on Blynk app

  // Check if bin level exceeds the threshold
  if (binLevel > binLevelThreshold && !alarmState) {
    if (servo.read() != 0) {  // Check if servo is not already at 0 degrees
      servo.write(0);  // Rotate servo to 0 degrees (closed)
      Blynk.virtualWrite(V2, 0);  // Update servo position on Blynk app
      send_blynk("Bin level exceeded threshold!");  // Send notification to Blynk app
      alarmState = true;
      Serial.println("Notification sent: Bin level exceeded threshold!");
    }
  } else if (binLevel <= binLevelThreshold && alarmState) {
    if (servo.read() != 90) {  // Check if servo is not already at 90 degrees
      servo.write(90);  // Rotate servo to 90 degrees (open)
      Blynk.virtualWrite(V2, 90);  // Update servo position on Blynk app
      alarmState = false;
    }
  }
}

void setup() {
  Serial.begin(9600);
  servo.attach(servoPin);  // Attach servo to the specified pin

  Blynk.begin(auth, ssid, pass);  // Connect to Blynk server

  timer.setInterval(1000L, controlBin);  // Set timer interval for bin control
  
  // Initial position of the servo motor
  servo.write(90);  // Open position
  Blynk.virtualWrite(V2, 90);  // Update servo position on Blynk app
}

void loop() {
  Blynk.run();  // Run Blynk communication
  timer.run();  // Run timer for bin control
}
