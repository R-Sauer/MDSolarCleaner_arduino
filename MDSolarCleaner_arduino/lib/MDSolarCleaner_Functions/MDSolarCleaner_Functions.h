#include <DHT.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <MPU6050.h>

// Functions used for MDSolarCleaner_arduino project

float read_US(int num);

float read_temp(float temperature, DHT &dht);

float read_humidity(float humidity, DHT &dht);

int read_air_dust_density();

float read_brush_speed(int num, unsigned int counter_brush1, unsigned int counter_brush2);

float read_flow_velocity();

float read_acceleration(MPU6050 &mpu);

bool send_data_to_raspy(float distance_up, float distance_down, float distance_right,
                         float distance_left, float temperature, float surface_temp,
                         float humidity, int air_dust_density, float brush1_speed,
                         float brush2_speed, float flow_velocity, float acceleration);

void writeSensorvaluesToSerial(float sensorValues[], size_t len);
