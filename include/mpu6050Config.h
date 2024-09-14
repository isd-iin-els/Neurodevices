#ifndef mpu6050config_h
#define mpu6050config_h

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "SistemasdeControle/headers/primitiveLibs/LinAlg/matrix.h"
LinAlg::Matrix<double> mpuData(1,3);

Adafruit_MPU6050 mpu;  sensors_event_t a, g, temp;
#define M_PI    3.14159265358979323846
double pitch, roll, rad2degree = 180/M_PI;

double get_pitch( double ax, double ay, double az){
    return atan2(-1*ax, sqrt(ay*ay + az*az ))*rad2degree;
}
		
double get_roll(double ax, double ay, double az){
    return atan2(ay, az + 0.05*ax)*rad2degree;
}

double get_yaw(double magx, double magy, double magz, double pitch, double roll){
		return atan2(sin(roll)*magz - cos(roll)*magy,	cos(pitch)*magx + sin(roll)*sin(pitch)*magy + cos(roll)*sin(pitch)*magz)*rad2degree;
    //return atan2(magy,magx); 
}

bool mpuInit()
{
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        return false;
    }
    Serial.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    // mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    return true;
}

void getEulerAngles(){
    mpu.getEvent(&a, &g, &temp);
    mpuData(0,0) = get_pitch(a.acceleration.x, a.acceleration.y, a.acceleration.z);
    mpuData(0,1) = get_roll(a.acceleration.x, a.acceleration.y, a.acceleration.z);
    // if(mpuData(0,1) < 0) mpuData(0,1) = mpuData(0,1) +360;
}

#endif