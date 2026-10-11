// Written by Jonas Nichols for Helios: Team 1 Cansat 2 Month Project 2026
// NOTE: Cannot be uploaded while OpenLog is connected
//
// TODO:
//      Find battery voltage
//      Find panel voltage (maybe done?????)
//      
//
// EXTRAS???
//      add lebron sunshine
//
// Error nums
// 1: ESPNOW start fail
// 2: ESPNOW peer add fail
// 3: BMP (alt/temp) fail
// 4: BNO (orien/velocity) fail
// 5: GNSS (GPS) fail


#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <String.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <SparkFun_u-blox_GNSS_v3.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>


struct dataFormat {
  char teamId[4];
  int timeElapsed;
  int packetCount;
  int curStage;
  uint8_t mechState;
  float altitude;
  int temperature;
  float batteryVoltage;
  float latitude;
  float longitude;
  int satsUsed;
  float gyroX;
  float gyroY;
  float gyroZ;
  float accelerationX;
  float accelerationY;
  float accelerationZ;
  float panelAVolt;
  float panelBVolt;
};

dataFormat data;


// Barometer and Temperature sensor
Adafruit_BMP3XX bmpSensor; // I2C var for Barometer

// Accelometer and Rotation sensor
Adafruit_BNO055 bnoSensor = Adafruit_BNO055(55, 0x28, &Wire);

// GPS
SFE_UBLOX_GNSS GNSS;

// Other pins
const int operationLight = 18;
const int buzzer = 19;
const int releasePin = 32;
const int panelServoPin = 33;
const int panelAInputPin = 36;
const int panelBInputPin = 39;


// servo information
Servo releaseServo;
const int releaseExtensionAmount = 55;
bool released = 0;
Servo panelServo;
const int panelExtensionAmount = 90;
bool panelExtended = 0;

char teamId[4] = "001";
float altitude = 0;
int velocity;
int packetCount = 0;
int temperature;
float initalPressure;
float batteryVoltage;
float panelAVolt;
float panelBVolt;
int morseUnit = 10;    // in ms
uint8_t mechState = 0x00;
float PEtimeStarted = 0;
float lastTransmission = 0;

float gyroX;
float gyroY;
float gyroZ;

float accelerationX;
float accelerationY;
float accelerationZ;

long lastGPSsample;
long latitude;
long longitude;
int satsUsed;


// ESP-NOW
// teamId, timeElapsed, packetCount, stage, mechState, altitude, temperature, batteryVoltage, latitude, longitude, satsUsed,gyrox,y,z,accelerationX,accelerationY,accelerationZ,panelVolt1,panelVolt2,,extraData
char command[4];
esp_now_peer_info_t groundInfo;
// Ground MAC
uint8_t MAC[] = {
  0x68, 0x09, 0x47, 0x9C, 0xA8, 0xDC
};


void sampleSensors();
void saveTransmitData();
void panelExtend();
void release();

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len);
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

void alert(int num = 99);
void beep(int time);

int xTomV(int bits);

typedef enum {
  launchPad,
  ascent,
  apogee,
  descent,
  landed
} stage;

// String stageNames[5] = {
//   "launchPad",
//   "ascent",
//   "apogee",
//   "descent",
//   "landed"
// }

void flightState();

stage curStage;


void setup() {
  pinMode(buzzer, OUTPUT);

  // Set up ESP-NOW / Wifi
  Serial.begin(115200);
  // start Wifi
  WiFi.mode(WIFI_STA);  
  while(!WiFi.STA.started()){ delay(100); }
  // Print the MAC address
  // init ESP NOW
  if (esp_now_init() != ESP_OK) { alert(1); }

  // register and add ground
  memcpy(groundInfo.peer_addr, MAC, 6);
  groundInfo.channel = 1;
  groundInfo.encrypt = false;
  if (esp_now_add_peer(&groundInfo) != ESP_OK) { alert(2); }
  // register callback function
  esp_now_register_recv_cb(esp_now_recv_cb_t(onDataRecv));

  // start Serial2 for the OpenLog
  Serial2.begin(115200);

  Wire.begin();

  // Set up bmp Sensor (Barometer and temperature)
  if (!bmpSensor.begin_I2C()) { alert(3); }

  // Set up oversampling and filter initialization
  bmpSensor.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmpSensor.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmpSensor.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmpSensor.setOutputDataRate(BMP3_ODR_50_HZ);

  // First reading is inaccurate; throw away values
  sampleSensors();

  // Set pressure of current (lowest) altitude in Hpa
  sampleSensors();
  initalPressure = (bmpSensor.pressure / 100);

  // Set up bno Sensor (orintation and accelometer)
  if (!bnoSensor.begin()) { alert(4); }

  // Start GPS
  if (!GNSS.begin()) { alert(5); }

  GNSS.setI2COutput(COM_TYPE_UBX); // Sets output to UBX only instead of the standard NMEA
  GNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);

  // set up servos
  releaseServo.attach(releasePin);
  panelServo.attach(panelServoPin);

	releaseServo.write(0);
	panelServo.write(0);

  // set up panel voltage readers
  analogSetAttenuation(ADC_0db);

  beep(10);

  curStage = launchPad;
} 


void loop() {

  // run at 4 Hz (0.25 seconds rate)
  if ((millis() - lastTransmission) > 250) {

    lastTransmission = millis(); // having it up here keeps the scheduling on time marginally better
    sampleSensors();
    saveTransmitData();

  }

  flightState();

}


void flightState(){
  switch (curStage) {
    case (launchPad):

      if (altitude >= 10) {
        curStage = ascent;
      }
      break;

    case (ascent):

      // when at desired height
      if (altitude >= 530) {
        curStage = apogee;
      }
      break;

    case (apogee):

      release();
      PEtimeStarted = millis();

      curStage = descent;
      break;

    case (descent):

      // when stops falling
      if (altitude <= 10 || velocity < 1.8) {

        digitalWrite(buzzer, HIGH);
        curStage = landed;

      }

      if (!panelExtended && (millis()-PEtimeStarted) > 5000) {
        panelExtend();
      }
      break;

    case (landed):
      if (!panelExtended && (millis()-PEtimeStarted) > 5000) {
        panelExtend();
      }
      break;
  }

}


void sampleSensors() {

  // Sample solar cell voltages
  panelAVolt = xTomV(analogRead(panelAInputPin));
  panelBVolt = xTomV(analogRead(panelBInputPin));

  // Sample pressure and temperature sensors
  bmpSensor.performReading();
  temperature = bmpSensor.temperature;
  altitude = bmpSensor.readAltitude(initalPressure);


  // Sample rotation and acceleration sensors
  sensors_event_t gyroData, accelerometerData;
  bnoSensor.getEvent(&gyroData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bnoSensor.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);

  gyroX = gyroData.gyro.x;
  gyroY = gyroData.gyro.y;
  gyroZ = gyroData.gyro.z;

  accelerationX = accelerometerData.acceleration.x;
  accelerationY = accelerometerData.acceleration.y;
  accelerationZ = accelerometerData.acceleration.z;

  // Only sample GPS again if it has been 1 second
  if (millis() - lastGPSsample > 1000) {

    // sample GPS
    longitude = GNSS.getLongitude();
    latitude = GNSS.getLatitude();
    satsUsed = GNSS.getSIV();
    
    lastGPSsample = millis();
  
  }

}


void release() {
  if (released) { return; }

  releaseServo.write(releaseExtensionAmount);
}


void panelExtend() {
  if (panelExtended) { return; }

  panelServo.write(panelExtensionAmount);
  mechState = 0x11;
  panelExtended = true;
}

// r: release | e: extend | a: alert | s#: swap stage
void onDataRecv(
  const esp_now_recv_info_t *info,
  const uint8_t *incomingData,
  int len
) {
  strcpy(command,(char*) incomingData);

  switch (command[0]) {
    case 'r':
      release();
      break;
    case 'e':
      panelExtend();
      break;
    case 'a':
      alert();
      break;
    case 's':
      curStage = static_cast<stage>((int)command[1]-48);
      break;
  }
}


void saveTransmitData() {

  strcpy(data.teamId,"001");
  data.timeElapsed = millis();
  data.packetCount = packetCount;
  data.curStage = curStage;
  data.mechState = mechState;
  data.altitude = altitude;
  data.temperature = temperature;
  data.batteryVoltage = batteryVoltage;
  data.latitude = latitude;
  data.longitude = longitude;
  data.satsUsed = satsUsed;
  data.gyroX = gyroX;
  data.gyroY = gyroY;
  data.gyroZ = gyroZ;
  data.accelerationX = accelerationX;
  data.accelerationY = accelerationY;
  data.accelerationZ = accelerationZ;
  data.panelAVolt = panelAVolt;
  data.panelBVolt = panelBVolt;

  Serial.print(data.teamId); Serial.print(',');
  Serial.print(data.timeElapsed); Serial.print(',');
  Serial.print(data.packetCount); Serial.print(',');
  Serial.print(data.curStage); Serial.print(',');
  Serial.print(data.mechState); Serial.print(',');
  Serial.print(data.altitude); Serial.print(',');
  Serial.print(data.temperature); Serial.print(',');
  Serial.print(data.batteryVoltage); Serial.print(',');
  Serial.print(data.latitude); Serial.print(',');
  Serial.print(data.longitude); Serial.print(',');
  Serial.print(data.satsUsed); Serial.print(',');
  Serial.print(data.gyroX); Serial.print(',');
  Serial.print(data.gyroY); Serial.print(',');
  Serial.print(data.gyroZ); Serial.print(',');
  Serial.print(data.accelerationX); Serial.print(',');
  Serial.print(data.accelerationY); Serial.print(',');
  Serial.print(data.accelerationZ); Serial.print(',');
  Serial.print(data.panelAVolt); Serial.print(',');
  Serial.println(data.panelBVolt);
  
  if (esp_now_send(MAC, (uint8_t *) &data, sizeof(data)) == ESP_OK) {
    // if sent correctly
    packetCount++;
  }
  
  
  
  // teamId, timeElapsed, packetCount, stage, mechState, altitude, temperature, batteryVoltage,
  // latitude, longitude, satsUsed,gyrox,y,z,accelerationX,accelerationY,accelerationZ,panelVolt1,panelVolt2,,extraData
}

// ONLY PLAYS IN SETUP plays an error code based on input (default num = 99)
void alert(int num) {
  // if (num == 99) { // buzzer on
  //   digitalWrite(buzzer, HIGH);
  // }
  // else if (num > 5 && num < 10) {

  //   for (int i = 0; i < 5; i++) {
      
  //     if (num > 0) {
  //       beep(morseUnit);
  //       num--;
  //     }
  //     else {
  //       beep(morseUnit*3);
  //     }
  //     delay(morseUnit);
      
  //   }
  // }
  // else {
  //   for (int i = 0; i < 5; i++) {
      
  //     if (num > 0) {
  //       beep(morseUnit*3);
  //       num--;
  //     }
  //     else {
  //       beep(morseUnit);
  //     }
  //     delay(morseUnit);
      
  //   }
  // }
}

// ONLY PLAYS IN SETUP beeps for length
void beep(int length) {
  digitalWrite(buzzer, HIGH);
  delay(length);
  digitalWrite(buzzer, LOW);
}

// converts the analog input to mV
int xTomV(int bits) {
  return (((bits / 4095) * 850) + 100);
}