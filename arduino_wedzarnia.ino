#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define I2C_SDA 17
#define I2C_SCL 18

TwoWire I2CBME = TwoWire(0);
Adafruit_BME280 bme; // I2C

float temperature, humidity, pressure, altitude;

/*Put your SSID & Password*/
const char* ssid = "admin";  // Enter SSID here
const char* password = "admin";  //Enter Password here

WebServer server(80);             

void handle_OnConnect() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  server.send(200, "text/html", SendHTML(temperature,humidity,pressure,altitude)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float temperature,float humidity,float pressure,float altitude){
  String ptr = "<!DOCTYPE html> <html lang=\"pl-PL\">\n";
  ptr +="<head><meta name=\"viewport\" charset=\"UTF-8\" http-equiv=\"refresh\" content=\"10\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\" >\n";
  ptr +="<title>Wędzarnia - parametry</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>Wędzarnia - parametry:</h1>\n";
  ptr +="<p>Temperatura: ";
  ptr +=temperature;
  ptr +="&deg;C</p>";
  ptr +="<p>Wilgotność: ";
  ptr +=humidity;
  ptr +="%</p>";
  ptr +="<p>Ciśnienie: ";
  ptr +=pressure;
  ptr +="hPa</p>";
  ptr +="<p>Wysokość n.p.m.: ";
  ptr +=altitude;
  ptr +="m</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Wire.begin(I2C_SDA, I2C_SCL);
  I2CBME.begin(I2C_SDA, I2C_SCL, 100000); 

  bme.begin(0x76, &I2CBME);
  
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

}
void loop() {
  server.handleClient();
}
