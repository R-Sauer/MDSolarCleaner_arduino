#include "MDSolarCleaner_Functions.h"

float read_temp(float temperature, DHT &dht) 
{
  temperature = dht.readTemperature();  // Temperatur auslesen in Celsius
  return temperature;
}

float read_surface_temp(Adafruit_MLX90614 &mlx) 
{
  float temp = 999.0;
  temp = mlx.readObjectTempC();
  return temp;
}

float read_humidity(float humidity, DHT &dht) 
{
  humidity = dht.readHumidity();
  return humidity;
}

int runAve(int x, int n) 
{
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

int read_air_dust_density() 
{
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
  if (Serial.available() > 0) 
  {
    // Lies das eingehende Byte:
    incomingByte = Serial.read();
    if (incomingByte == 0xAA) 
    {
      Serial.readBytes(buf, LEN);
      if ((buf[0] == 0xC0) && (buf[8] == 0xAB)) 
      {
        for (i = 1; i <= 6; i++) 
        {
          checksum = checksum + buf[i];
        }
        PM10Val = ((buf[4] << 8) + buf[3]) / 10;
        PM10ave = runAve(PM10Val, 10);

        //Serial.print("PM10 Average: ");
        //Serial.print(PM10ave);
        //Serial.println(" ug/m3");
        //Serial.println();

        loopcnt++;
      } else 
        {
          Serial.println("Frame Error");
        }
    }
  }
  return PM10ave;
}

float read_brush_speed(int num, unsigned int counter_brush1, unsigned int counter_brush2) 
{
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

float read_flow_velocity() 
{
  float flow_velocity = 1000.0; 
  int sensorValue = analogRead(A0);
  flow_velocity = map(sensorValue, 0, 1023, 0, 100);
  return flow_velocity;
}

float read_acceleration(MPU6050 &mpu) 
{
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  float ax_in_g = ax/16384.0;
  float ay_in_g = ay/16384.0;
  float az_in_g = (az/16384.0)+0.2; 

  return az_in_g;
}

bool send_data_to_raspy(float distance_up, float distance_down, float distance_right,
                         float distance_left, float temperature, float surface_temp,
                         float humidity, int air_dust_density, float brush1_speed,
                         float brush2_speed, float flow_velocity, float acceleration) 
{
  String sensorString = String(distance_up) + ";" + String(distance_down) + ";" +
                        String(distance_right) + ";" + String(distance_left) + ";" +
                        String(temperature) + ";" + String(surface_temp) + ";" +
                        String(humidity) + ";" + String(air_dust_density) + ";" +
                        String(brush1_speed) + ";" + String(brush2_speed) + ";" +
                        String(flow_velocity) + ";" + String(acceleration);

  Serial.println(sensorString);
  return 1;  // Wenn erfolgreich sende 1 sonst 0
}