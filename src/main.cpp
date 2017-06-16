/*------------------------------------------------------------------------------

  LIDARLite Arduino Library
  GetDistanceI2c

  This example shows how to initialize, configure, and read distance from a
  LIDAR-Lite connected over the I2C interface.

  Connections:
  LIDAR-Lite 5 Vdc (red) to Arduino 5v
  LIDAR-Lite I2C SCL (green) to Arduino SCL
  LIDAR-Lite I2C SDA (blue) to Arduino SDA
  LIDAR-Lite Ground (black) to Arduino GND

  (Capacitor recommended to mitigate inrush current when device is enabled)
  680uF capacitor (+) to Arduino 5v
  680uF capacitor (-) to Arduino GND

  See the Operation Manual for wiring diagrams and more information:
  http://static.garmin.com/pumac/LIDAR_Lite_v3_Operation_Manual_and_Technical_Specifications.pdf

------------------------------------------------------------------------------*/
#include <Wire.h>
#include <LIDARLite.h>
#include <Servo.h>
#include <math.h>

struct point{
  float x;
  float y;
  float z;
};

Servo myservo1;  // create servo object to control a servo
Servo myservo2;  // create servo object to control a servo

int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

LIDARLite myLidarLite;

void setup()
{
  Serial.begin(115200); // Initialize serial connection to display distance readings
  myservo2.attach(9);// vertikal
  myservo1.attach(10);// horizontal

  myservo1.write(90);
  myservo2.write(90);
  /*
    begin(int configuration, bool fasti2c, char lidarliteAddress)

    Starts the sensor and I2C.Servo myservo1;  // create servo object to control a servo

    Parameters
    ----------------------------------------------------------------------------
    configuration: Default 0. Selects one of several preset configurations.
    fasti2c: Default 100 kHz. I2C base frequency.
      If true I2C frequency is set to 400kHz.
    lidarliteAddress: Default 0x62. Fill in new address here if changed. See
      operating manual for instructions.
  */
  myLidarLite.begin(0, true); // Set configuration to default and I2C to 400 kHz

  /*
    configure(int configuration, char lidarliteAddress)
hori
    Selects one of several preset configurations.

    Parameters
    ----------------------------------------------------------------------------
    configuration:  Default 0.
      0: Default mode, balanced performance.
      1: Short range, high speed. Uses 0x1d maximum acquisition count.
      2: Default range, higher speed short range. Turns on quick termination
          detection for faster measurements at short range (with decreased
          accuracy)
      3: Maximum range. Uses 0xff maximum acquisition count.
      4: High sensitivity detection. Overrides default valid measurement detection
          algorithm, and uses a threshold value for high sensitivity and noise.
      5: Low sensitivity detection. Overrides default valid measurement detection
          algorithm, and uses a threshold value for low sensitivity and noise.
    lidarliteAddress: Default 0x62. Fill in new address here if changed. See
      operating manual for instructions.
  */
  myLidarLite.configure(0); // Change this number to try out alternate configurations
}

int messure(int count)
{
  int distance = 0;
  if(count%100 == 0)
  {
    //Serial.println("cal");
    distance = myLidarLite.distance();
  }
  else
  {
    distance = myLidarLite.distance(false);
  }
  //  Serial.println(distance);
  return distance;
}

point calcPoint(int distance, int ang_vert, int ang_hori)
{
  struct point p;
  float pi = 3.14159265359;
  p.x = (float) distance * sin((float) ang_vert/180*pi/2) * cos((float) ang_hori/180*pi);
  p.y = (float) distance * sin((float) ang_vert/180*pi/2) * sin((float) ang_hori/180*pi);
  p.z = (float) distance * cos((float) ang_vert/180*pi/2);

  return p;
}

void printPoint(point p)
{
  Serial.print(String(p.x) + ",");
  Serial.print(String(p.y)  + ",");
  Serial.print(String(p.x));
}

void loop()
{
  //myservo1.write(90);
  //myservo2.write(90);

  int max = 180;
  int min = 20;
  int del = 1;
  int count = 0;

  for(int i = 0; i <= max; i++)
  {
    myservo1.write(i);
    for(int j = max; j >= min; j--)
    {
      myservo2.write(j);
      delay(del);
      int dist = messure(count);
      printPoint(calcPoint(dist, j, i));
      Serial.println(","+String(dist)+","+String(j)+","+String(i));
      count++;
    }
    i++;
    myservo1.write(i);
    for(int j = min; j <= max; j++)
    {
      myservo2.write(j);
      delay(del);
      int dist = messure(count);
      printPoint(calcPoint(dist, j, i));
      Serial.println(","+String(dist)+","+String(j)+","+String(i));
      count++;
    }
  }
}
