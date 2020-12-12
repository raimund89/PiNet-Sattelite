#ifndef LC_WEBSERVER
#define LC_WEBSERVER

#include <ESP8266SSDP.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <ArduinoJson.h>

#include "lc_gpio.h"
#include "lc_templates.h"

#define SSDP_URL String("/")
#define SSDP_SCHEMA_URL "description.xml"
#define SSDP_HTTP_PORT 80
#define SSDP_NAME "PiNet Sattelite"
#define SSDP_SERIAL_NUMBER ESP.getChipId()
#define SSDP_MODEL_NAME "RGB"
#define SSDP_MODEL_NUMBER "01"
#define SSDP_MANUFACTURER "PiNet"

ESP8266WebServer server(80);

void InitWebserver()
{
  server.on("/", []() {
    server.send(200, "text/html", FrontMatter() + HeadMatter() + ButtonMatter("/info", "Information") + BackMatter());
  });
  server.on("/info", []() {
    String str = FrontMatter() + HeadMatter();
    
    str += InfoHeader("General");
    str += InfoRow("Manufacturer", "Espressiff");
    str += InfoRow("Device Type", "RGB");
    str += InfoRow("Friendly Name", "Fractal Lamp");
    str += InfoRow("PiNet version", "0.1");

    str += InfoHeader("Chip Info");
    long mils = millis();
    int mins = int((mils/(1000*60)) % 60);
    int hrs = int((mils/(1000*60*60)) % 24);
    int dys = int((mils/(1000*60*60*24)) % 365);
    str += InfoRow("Uptime", String(dys) + "d " + hrs + "h " + mins + "m");
    //str += InfoRow("Flash writes", "");
    //str += InfoRow("Boot count", "");
    // Flash size
    // Program size
    // Free program space
    // Free memory
    

    str += InfoHeader("WiFi Info");
    str += InfoRow("SSID", WiFi.SSID());
    str += InfoRow("Network IP", WiFi.localIP().toString());
    str += InfoRow("MAC-address", WiFi.macAddress());
    
    str += InfoHeader("SSDP Discovery");
    str += InfoRow("Schema URL", SSDP_URL + SSDP_SCHEMA_URL);
    str += InfoRow("Port", String(SSDP_HTTP_PORT));
    str += InfoRow("Name", SSDP_NAME);
    str += InfoRow("Serial Number", String(SSDP_SERIAL_NUMBER));
    str += InfoRow("Manufacturer", SSDP_MANUFACTURER);
    str += InfoRow("Model Name", SSDP_MODEL_NAME);
    str += InfoRow("Model Number", SSDP_MODEL_NUMBER);
    
    str += BackMatter();

    server.send(200, "text/html", str);
  });
  server.on("/cm", []() {
    if(server.hasArg("cmnd")) {
      String cmnd = server.arg("cmnd");

      if(cmnd == "DeviceName") {
        DynamicJsonDocument doc(50);
        doc["DeviceName"] = "RGB01";
        String output;
        serializeJson(doc, output);
        server.send(200, "text/json", output);
      }
      else if(cmnd == "Color") {
        if(server.hasArg("c")){
          String color = server.arg("c");
          SetColor(Color(strtoul(color.substring(0,2).c_str(), NULL, 16), strtoul(color.substring(2,4).c_str(), NULL, 16), strtoul(color.substring(4,6).c_str(), NULL, 16)), true);
        }
  
        // They want to know the current color.
        DynamicJsonDocument doc(50);
        Color c = GetColor();
        char cs[6];
        sprintf(cs, "#%02X%02X%02X", c.r, c.g, c.b);
        doc["Color"] = cs;
        String output;
        serializeJson(doc, output);
        server.send(200, "text/json", output);
      }
      else if(cmnd == "Program") {
        if(server.hasArg("p")) {
          // Set the program and settings

          // p=-1 means the user wants the program to stop.
        }

        // Return the program
      }
    }
  });
  server.on("/description.xml", HTTP_GET, [](){
    SSDP.schema(server.client());
  });
  server.begin();

  SSDP.setSchemaURL(SSDP_URL + SSDP_SCHEMA_URL);
  SSDP.setHTTPPort(SSDP_HTTP_PORT);
  SSDP.setName(SSDP_NAME);
  SSDP.setSerialNumber(SSDP_SERIAL_NUMBER);
  SSDP.setURL(SSDP_URL);
  SSDP.setModelName(SSDP_MODEL_NAME);
  SSDP.setModelNumber(SSDP_MODEL_NUMBER);
  SSDP.setManufacturer(SSDP_MANUFACTURER);
  SSDP.begin();
  
  Program_Clear();
}

void HandleWebserver()
{
  server.handleClient();
}

#endif
