// Signal K application template file.
//
// This application demonstrates core SensESP concepts in a very
// concise manner. You can build and upload the application as is
// and observe the value changes on the serial port monitor.
//
// You can use this source file as a basis for your own projects.
// Remove the parts that are not relevant to you, and add your own code
// for external hardware libraries.

// Boilerplate #includes:
#include "sensesp_app_builder.h"
#include "sensesp/signalk/signalk_output.h"

// Sensor-specific #includes:
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define BMP_SCK 18
#define BMP_MISO 19
#define BMP_MOSI 23
#define BMP_CS 5

#define SEALEVELPRESSURE_HPA (1013.25)

// For RepeatSensor:
#include "sensesp/sensors/sensor.h"

using namespace sensesp;

// SensESP builds upon the ReactESP framework. Every ReactESP application
// must instantiate the "app" object.
reactesp::ReactESP app;

// (Specific to the BMP280, and I2C. Replace this with similar code to create an instance
// of whatever sensor you may be using in your project.)
// Create an instance of the sensor using its I2C interface.
Adafruit_BMP3XX bmp;

// (Replace this with whatever function you need to read whatever value you want
// to read from any other sensor you're using in your project.)
// Define the function that will be called every time we want
// an updated temperature value from the sensor. The sensor reads degrees
// Celsius, but all temps in Signal K are in Kelvin, so add 273.15.
float read_temp_callback() { return (bmp.temperature + 273.15); }
float read_press_callback() { return (bmp.pressure); }

// The setup function performs one-time application initialization
void setup() {

// Some initialization boilerplate when in debug mode
#ifndef SERIAL_DEBUG_DISABLED
  SetupSerialDebug(115200);
#endif

  // Create the global SensESPApp() object
  SensESPAppBuilder builder;
  sensesp_app = builder.set_hostname("Pressione")
->set_sk_server("10.10.10.1", 3000)
->set_wifi("sagittario", "sbr0d3774")
->get_app();
  

  // (Do whatever is required to "start" your project's sensor here)
  // Initialize the BMP280 using the default address
  bmp.begin_I2C();
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  // Read the sensor every 2 seconds
  unsigned int read_interval = 2000;

  // Create a RepeatSensor with float output that reads the temperature
  // using the function defined above.
  auto* air_press =
      new RepeatSensor<float>(read_interval, read_press_callback);

  // Set the Signal K Path for the output
  const char* sk_path = "environment.outside.pressure";

  // Send the temperature to the Signal K server as a Float
  air_press->connect_to(new SKOutputFloat(sk_path));

  // Start the SensESP application running
  sensesp_app->start();

}

// loop simply calls `app.tick()` which will then execute all reactions as needed
void loop() {
  bmp.performReading();
  app.tick();
  
}
