# Liquid Level Detector

This project aims to create a Liquid Level Detector system with two Time of Flight sensors (VL53L0X) a DHT11 Temperature/Humidity sensor and add a Web Server functionality in order to initiate the Time of Flight sensors and return the values in Litres for 2 tanks (one for petrol and one for water) alongside temperature readings. The host microcontroller is an ESP32 NodeMCU 32S with the 38 pins.

Below is the pinout diagramm for the microcontroller:
https://grobotronics.com/images/companies/1/NodeMCU-32S-pinout.jpg?1567071865358

The current version of the code takes as default values the width and length of the each individual tank and measures the height using the VL53L0X sensors. Since the tanks are orthogonal parallelogramms then to calculate the volume simply multiply the 3 dimensions. In order to make the whole system usable for every tank and not messing with the code every time it would be a good idea to make the web server request values for width and length before getting the volume. This can be achieved using the ESPAsyncWebServer.h library alongside some extra lines of code. In this project however, the 2 dimensions of the tanks were given in the code but if anyone is interested in using the above-mentioned library and make the change it is completely fine.


# VL53L0X Time of Flight sensor

For this project the Adafruit VL53L0X breakout board was used. This was done because Adafruit provides its own library to drive the sensors and it provides an example on how to use multiple of the same sensors in a single design. Because the VL53L0X uses the I2C bus to communicate with the host microcontroller the hard coded address of each VL53L0X sensor is the same. In order to work around this issue the XSHUT (SHDN) pin on both sensors have to be connected to the ESP32 in order to manually provide the different addresses for each sensor. In this example the SHDN pins for each sensor were connected to GPIO5 and GPIO4 respectively on the microcontroller.

Below you can find the Adafruit website that discusses how to connect multiple VL53L0X sensors in the same system:
https://learn.adafruit.com/adafruit-vl53l0x-micro-lidar-distance-sensor-breakout/arduino-code?view=all

In order to make our system more efficient and increase the life span of the sensors the Vin pins for each Time of Flight sensor were connected to pins GPIO12 and GPIO13 respectively on the microcontroller. This pins will be set to high when the user presses the button on the web server page on their browser. By doing so the sensors will then take the average value from 50 readings and that is going to be used as the height in calculating the volume. By re-pressing the same button the pins will be set to low and the sensors will turn off. The Vin of the sensor is 3.3-5 VDC the SDA and SCL pins are connected to the GPIO21 and GPIO22 respectivelly and GND to ground. The only limitation for this sensor is its range. According to the manufacturer the sensor can detect ranges from 30mm up to 2m. If your tank has a height greater than 2 m then the next option is to use an ultrasonic proximity sensor. The HC-SR04 sensor can detect objects from 2 cm up to 4 meters with an accuracy of 3 mm. However I have not tested the HC-SR04 in any aqueous environment and do not know how accurate it is. In the industry there are both ultrasonic and Lidar instruments used for liquid level detection so I am guessing that this will not be an issue.

# DHT11 Temperature/Humidity Sensor

This sensor was used because the person I was making the system for, wanted to know the temperature of the basement where the tanks were located. Unfortunately the DHT11 required a 5VDC and the pins of the NodeMCU32s output 3.3 VDC. That means that the temperature/humidity sensor is always active but if upgraded to the DHT22 then a pin on the microcontroller can be used, similarly to the Time of Flight sensor, to control the on/off state. For this sensor simply hook the Vin to 5VDC GND to ground and the SIG to GPIO23 on the microcontroller.

# Web Server Access

In order to access the web server from any device simply turn on the Wi-Fi. Find the Liquid Level Meter SSID and enter the password specified in the code 
const char* password = "";. You can also change the SSID name of the web server by changing the const char* ssid = "Liquid Level Meter";. Then open a web browser (I used Chrome) and in the search bar type 192.168.1.1 . This will open a page where you can find a button located in the middle. By clicking the button once, the microcontroller sets the GPIO12 and GPIO13 to high, the system takes measurements of the height and outputs the volume in Litres alongside the temperature in Celcius. Clicking the button again will turn off the Time of Flight sensors and return you to the previous page. In case there is an error a page will appearing saying page not found.

