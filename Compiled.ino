#include <board.h>
#include <BreakoutSDK.h>
#include <stdio.h>
#include <Ultrasonic.h>
#include <DHT.h>

static const char *device_purpose = "Dev-Kit";
static const char *psk_key = "e9d43386a0184c02292c6b849bbabd5d";

Breakout *breakout = &Breakout::getInstance();

#define LOOP_INTERVAL (5 * 1000)
//#define SEND_INTERVAL (1 * 60 * 1000)
#define SEND_INTERVAL (3 * 60 * 1000)
#define ULTRASONIC_PIN  (20)
#define INTERVAL        (1000)
#define SENSOR_PIN (D38)
#define DHTTYPE DHT11   // DHT 11 

Ultrasonic UltrasonicRanger(ULTRASONIC_PIN);

DHT dht(SENSOR_PIN, DHTTYPE);

void setup() {
  // Feel free to change the log verbosity. E.g. from most critical to most verbose:
  //   - errors: L_ALERT, L_CRIT, L_ERR, L_ISSUE
  //   - warnings: L_WARN, L_NOTICE
  //   - information & debug: L_INFO, L_DB, L_DBG, L_MEM
  // When logging, the additional L_CLI level ensure that the output will always be visible, no matter the set level.
  owl_log_set_level(L_INFO);
  LOG(L_WARN, "Arduino setup() starting up\r\n");

  // Set the Breakout SDK parameters
  breakout->setPurpose(device_purpose);
  breakout->setPSKKey(psk_key);
  breakout->setPollingInterval(1 * 60);  // Optional, by default set to 10 minutes

  // Powering the modem and starting up the SDK
  LOG(L_WARN, "Powering on module and registering...");
  breakout->powerModuleOn();

  LOG(L_WARN, "... done powering on and registering.\r\n");
  LOG(L_WARN, "Arduino loop() starting up\r\n");
}

/**
 * This is just a simple example to send a command and write out the status to the console.
 */

void sendCommand(const char * command) {
  if (breakout->sendTextCommand(command) == COMMAND_STATUS_OK) {
    LOG(L_INFO, "Tx-Command [%s]\r\n", command);
  } else {
    LOG(L_INFO, "Tx-Command ERROR\r\n");
  }
}

int hexadecimal_to_decimal(int x)
{
      int decimal_number, remainder, count = 0;
      while(x > 0)
      {
            remainder = x % 10;
            decimal_number = decimal_number + remainder * pow(16, count);
            x = x / 10;
            count++;
      }
      return decimal_number;
}



void loop()
{
  static unsigned long last_send = 0;

  gnss_data_t data;
  breakout->getGNSSData(&data);

  if ((last_send == 0) || (millis() - last_send >= SEND_INTERVAL)) {
    last_send = millis();

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
  
    //LOG(L_INFO, "Current temperature [%f] degrees celcius\r\n", temperature);
    //SerialUSB.println("Current temperature " temperature " degrees celcius\r\n");
    //LOG(L_INFO, "Current humidity [%f]\r\n", humidity);
    //SerialUSB.println("Current humidity [%f]\r\n", humidity);
    char thcommandText[512];
    //SerialUSB.println("Current humidity [%4.2f] and current temp [%4.2f]", humidity, temperature)
    snprintf(thcommandText, 512, "Current humidity [%4.2f] and current temp [%4.2f]", humidity, temperature);
    SerialUSB.println(thcommandText);
    sendCommand(thcommandText);

    delay(100);

    char distancecommandText[512];
    long distance;
    distance = UltrasonicRanger.MeasureInCentimeters();
  
    snprintf(distancecommandText, 512, "Object is within [%ld] cm from sensor", distance);
    sendCommand(distancecommandText);
    SerialUSB.println(distancecommandText);
    //LOG(L_INFO, "Object is [%ld] CM from the sensor\r\n", distance );

    char gpscommandText[512];

    //LOG(L_INFO, "Current Latitude [%d] degrees\r\n", data.position.latitude_degrees);
    //SerialUSB.println("Current Latitude [%d] degrees\r\n", data.position.latitude_degrees);
    //LOG(L_INFO, "Current Latitude [%d] minutes\r\n", data.position.latitude_minutes);
    //SerialUSB.println("Current Latitude [%d] minutes\r\n", data.position.latitude_minutes);
    //LOG(L_INFO, "Current Longitude [%d] degrees\r\n", data.position.longitude_degrees);
    //SerialUSB.println("Current Longitude [%d] degrees\r\n", data.position.longitude_degrees);
    //LOG(L_INFO, "Current Latitude [%d] minutes\r\n", data.position.longitude_minutes);
    //SerialUSB.println("Current Latitude [%d] minutes\r\n", data.position.longitude_minutes);
    
    snprintf(gpscommandText, 512, "Current Position:  %d %7.5f %s  %d %7.5f %s\r\n", data.position.latitude_degrees,
        data.position.latitude_minutes, data.position.is_north ? "N" : "S", data.position.longitude_degrees,
        data.position.longitude_minutes, data.position.is_west ? "W" : "E");
    SerialUSB.println(gpscommandText);    if (data.valid) {
      sendCommand(gpscommandText);
    }
  }

  breakout->spin();

  delay(INTERVAL);
}
