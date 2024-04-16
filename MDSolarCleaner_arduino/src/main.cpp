#include <DHT.h>  // heißt DHT sensor library by Adafruit
#include <Wire.h> //fuer Gyro und Surface Temperature
#include <Adafruit_MLX90614.h> //MLX90614 von Adafruit
#include <MPU6050.h> //MPU6050 von Electronics Cats


//Air_Dust
//RX vom Sensor an TX (Digital 1) und TX vom Sensor an RX vom Arduino (D0)

//Brush Speed 1 und 2
#define BRUSHSPEED1 18
#define BRUSHSPEED2 19
//VCC jeweils an 3.3V
//GND jeweils an GND
volatile unsigned int counter_brush1 = 0;  // Zähler für die Drehungen
volatile unsigned int counter_brush2 = 0;  // Zähler für die Drehungen

//US Sensoren:
//US-Sensor UP
#define US_UP_TRIG 4
#define US_UP_ECHO 5

//US-Sensor DOWN
#define US_DOWN_TRIG 6
#define US_DOWN_ECHO 7

//US-Sensor RIGHT
#define US_RIGHT_TRIG 8
#define US_RIGHT_ECHO 9

//US-Sensor LEFT
#define US_LEFT_TRIG 10
#define US_LEFT_ECHO 11

//Air Temperatur
#define TEMPSENSPIN 12
const int DHTPIN = TEMPSENSPIN;       // Pin, an dem der DHT22 angeschlossen ist
const int DHTTYPE = DHT22;    // Typ des DHT-Sensors (DHT22 für den DHT22-Sensor)
DHT dht(DHTPIN, DHTTYPE);

//Acceleration MPU6050 und IR Temperatursensor Panel (beide nutzen I2C Bus)
#define SDA 20
#define SCL 21
//SDA-D20(SDA)
//SCL-D21(SCL)
//VCC-5V
//GND-GND
MPU6050 mpu;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

//Flow Velocity (Strömungssensor)
//Rh=3.3V Rs=A0 GND=GND
#define STROEMUNG_RS A0


// Sensorvariablen
float distance_up = 0.0;            // [cm]
float distance_down = 0.0;          // [cm]
float distance_right = 0.0;         // [cm]
float distance_left = 0.0;          // [cm]
float temperature = 0.0;            // [°C]
float surface_temp = 0.0;           // [°C]
float humidity = 0.0;               // [%]
int air_dust_density = 0;           // [µg/m3] Durchmesser von 10 Mikrometern
float brush1_speed = 0.0;           // [U/min]
float brush2_speed = 0.0;           // [U/min]
float flow_velocity = 0.0;          // [L/min]
float acceleration = 0.0;           // [g]

// Variablen für Programmablauf
int takt100 = 0;
float takt1000 = 0;
unsigned long pasttime = 0;

// Funktionsdeklarationen
float read_US(int num);
float read_temp();
float read_humidity();
int read_air_dust_density();
float read_brush_speed(int num);
float read_flow_velocity();
float read_acceleration();
bool send_data_to_raspy(float distance_up, float distance_down, float distance_right,
                         float distance_left, float temperature, float surface_temp,
                         float humidity, int air_dust_density, float brush1_speed,
                         float brush2_speed, float flow_velocity, float acceleration);

//Setup----------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);  // Starte die serielle Kommunikation mit einer Baudrate von 9600
  pinMode(BRUSHSPEED1, INPUT);
  pinMode(BRUSHSPEED2, INPUT);
  pinMode(US_UP_TRIG, OUTPUT);
  pinMode(US_UP_ECHO, INPUT);
  pinMode(US_DOWN_TRIG, OUTPUT);
  pinMode(US_DOWN_ECHO, INPUT);
  pinMode(US_RIGHT_TRIG, OUTPUT);
  pinMode(US_RIGHT_ECHO, INPUT);
  pinMode(US_LEFT_TRIG, OUTPUT);
  pinMode(US_LEFT_ECHO, INPUT);
  pinMode(BRUSHSPEED1, INPUT);
  pinMode(BRUSHSPEED2, INPUT);
  pinMode(STROEMUNG_RS, INPUT); 
  Wire.begin(); // Fuer Acceleration mit I2C
  dht.begin(); //nutzt PIN 10
  mpu.initialize();
  mlx.begin(); // Initialisierung des Sensors
  Serial.println("Setup abgeschlossen");

  //Brush_speed 1 und 2
  attachInterrupt(digitalPinToInterrupt(BRUSHSPEED1), countRotation_brush1, RISING); //Brush1
  attachInterrupt(digitalPinToInterrupt(BRUSHSPEED2), countRotation_brush2, RISING); //Brush2

}

//Loop----------------------------------------------------------------------------------------------------
void loop() {
  unsigned long currentTime = millis();  // Aktuelle Zeit in Millisekunden seit dem Start des Programms

  if (currentTime - pasttime >= 100) {
    takt100 = 1;
    pasttime = currentTime;
  }

  if (takt100 == 1) {
    //Serial.println("Beginne 100ms Block");
    //temperature = read_temp(temperature);
    //humidity = read_humidity(humidity);
    //flow_velocity = read_flow_velocity();
    //acceleration = read_acceleration();
    send_data_to_raspy(air_dust_density,distance_up,distance_down,distance_right,distance_left,temperature,humidity,brush1_speed,brush2_speed,acceleration,surface_temp,flow_velocity);
    takt1000 += 0.1;  //Für Takt 1000
    takt100 = 0;      //zurücksetzen
  }

  if (takt1000 >= 1) {
    //Serial.println("Beginne 1S Block");  // WENN DIESER TEIL LANGE BRAUCHT LIEGT ES DARAN; DASS KEIN US SENSOR ANGESCHLOSSEN IST UND ER LANGE WARTET (PulseIn Funktion)
    //brush1_speed = read_brush_speed(1); //oben interrupts einkommentieren
    //brush2_speed = read_brush_speed(2); //oben interrupts einkommentieren
    distance_up = read_US(1);
    //distance_down = read_US(2);
    //distance_right = read_US(3);
    //distance_left = read_US(4);
    //air_dust_density = read_air_dust_density();
    //surface_temp = read_surface_temp();
    takt1000 = 0;  //zurücksetzen
  }
}
//Loop Ende----------------------------------------------------------------------------------------------------

// Funktion zum Auslesen der Sensoren
// Ultraschallsensor
float read_US(int num) {
  float dist = 0;
  int trig = 0;
  int echo = 0;
  // Festlegen der Pins durch Nummer des Sensors
  switch (num) {
    case 1:
      trig = US_UP_TRIG;
      echo = US_UP_ECHO;
      break;
    case 2:
      trig = US_DOWN_TRIG;
      echo = US_DOWN_ECHO;
      break;
    case 3:
      trig = US_RIGHT_TRIG;
      echo = US_RIGHT_ECHO;
      break;
    case 4:
      trig = US_LEFT_TRIG;
      echo = US_LEFT_ECHO;
      break;
    default:
      Serial.println("Wert liegt außerhalb des Bereichs von 1 bis 4");
      break;
  }

  long duration;  // Variablen zur Speicherung der Dauer und der gemessenen Entfernung

  // Sende ein Ultraschall-Puls-Signal aus
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  // Messen der Zeit, die der Ultraschall benötigt, um hin und her zu reflektieren
  duration = pulseIn(echo, HIGH);

  // Berechne die Entfernung basierend auf der gemessenen Zeit und der Schallgeschwindigkeit
  dist = (duration / 2) / 29.1;  // Formel für die Berechnung der Entfernung in Zentimetern

  //Serial.print("Sensor");
  //Serial.print(num);
  //Serial.print("distanz = ");
  //Serial.println(dist);
  return dist;
}

// Temperatur
float read_temp(float temperature) {
  temperature = dht.readTemperature();  // Temperatur auslesen in Celsius
  return temperature;
}

float read_surface_temp() {
  float temp = 999.0;
  temp = mlx.readObjectTempC();
  return temp;
}

float read_humidity(float humidity) {
  humidity = dht.readHumidity();
  return humidity;
}

int runAve(int x, int n) {
  static int aveArr[10];
  int sum = 0;
  int i;

  for (i = 0; i < n - 1; i++) {
    aveArr[i] = aveArr[i + 1];
  }
  aveArr[n - 1] = x;
  for (i = 0; i < n; i++) {
    sum = sum + aveArr[i];
  }
  return sum / n;
}

int read_air_dust_density() {
  //RX vom Sensor an TX (Digital 1) und TX vom Sensor an RX vom Arduino (D0)
  #define LEN 9
  int PM10Val = 0;
  int PM10ave = 1000;
  int loopcnt = 0;
  unsigned char incomingByte = 0;
  unsigned char buf[LEN];
  int i;
  unsigned char checksum;

  // Sende Daten nur, wenn Daten empfangen werden:
  if (Serial.available() > 0) {
    // Lies das eingehende Byte:
    incomingByte = Serial.read();
    if (incomingByte == 0xAA) {
      Serial.readBytes(buf, LEN);
      if ((buf[0] == 0xC0) && (buf[8] == 0xAB)) {
        for (i = 1; i <= 6; i++) {
          checksum = checksum + buf[i];
        }
        PM10Val = ((buf[4] << 8) + buf[3]) / 10;
        PM10ave = runAve(PM10Val, 10);

        //Serial.print("PM10 Average: ");
        //Serial.print(PM10ave);
        //Serial.println(" ug/m3");
        //Serial.println();

        loopcnt++;
      } else {
        Serial.println("Frame Error");
      }
    }
  }
  return PM10ave;
}

float read_brush_speed(int num) {
  // Berechnung der Geschwindigkeit alle 1000 Millisekunden
  unsigned long currentTime = millis();
  float brush_rotations= 0.0;
  float brush_speed= 0.0;
  if(num == 1){ //Brush 1
    brush_rotations = float(counter_brush1)/20;
    brush_speed = brush_rotations * 60; //Umdrehungen pro Minute
    counter_brush1 = 0;
  } else { //Brush 2
    brush_rotations = float(counter_brush2)/20;
    brush_speed = brush_rotations * 60; //Umdrehungen pro Minute
    counter_brush2 = 0;
  }
  return brush_speed;
}

float read_flow_velocity() {
  float flow_velocity = 1000.0; 
  int sensorValue = analogRead(A0);
  flow_velocity = map(sensorValue, 0, 1023, 0, 100);
  return flow_velocity;
}

float read_acceleration() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  float ax_in_g = ax/16384.0;
  float ay_in_g = ay/16384.0;
  float az_in_g = (az/16384.0)+0.2; 

  return az_in_g;
}

void countRotation_brush1() {
  // Die Funktion wird bei jeder Drehung des Encoders aufgerufen
  counter_brush1++;
}

void countRotation_brush2() {
  // Die Funktion wird bei jeder Drehung des Encoders aufgerufen
  counter_brush2++;
}

bool send_data_to_raspy(float distance_up, float distance_down, float distance_right,
                         float distance_left, float temperature, float surface_temp,
                         float humidity, int air_dust_density, float brush1_speed,
                         float brush2_speed, float flow_velocity, float acceleration) {
  String sensorString = String(distance_up) + ";" + String(distance_down) + ";" +
                        String(distance_right) + ";" + String(distance_left) + ";" +
                        String(temperature) + ";" + String(surface_temp) + ";" +
                        String(humidity) + ";" + String(air_dust_density) + ";" +
                        String(brush1_speed) + ";" + String(brush2_speed) + ";" +
                        String(flow_velocity) + ";" + String(acceleration);

  Serial.println(sensorString);
  return 1;  // Wenn erfolgreich sende 1 sonst 0
}