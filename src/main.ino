#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "remeha.h"
#include <util.h>

const int port = 9090;

ESP8266WebServer server(port);


void setup() {
  Serial.begin(9600);

  WiFi.begin("xxx", "xxx");
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      // Serial.println("Wifi connecting...");
      notConnectedCounter++;
      if(notConnectedCounter > 15) { // Reset board if not connected after 15s
          // Serial.println("Resetting due to Wifi not connecting...");
          ESP.restart();
      }
  }

  WiFi.mode(WIFI_STA);

  // Serial.print("Wifi connected, IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.begin();

  server.on("/", HandleRoot);
  server.on("/metrics", HandleRoot);
  server.on("/raw", HandleRaw);
  server.on("/hex", HandleHex);
  server.onNotFound(HandleNotFound);

  server.begin();

}



void clearSerialBuffer(){
  while(Serial.available()){Serial.read();}
}

String request = "\x02\xFE\x01\x05\x08\x02\x01\x69\xAB\x03";

// int GenerateMetrics(String& message) {
void HandleRoot() {
  String message;
  int responseSize = 0;
  int retries = 0;
  remehaData response;
  uint16_t crc = 0;

  // String message = "";

  clearSerialBuffer();

  Serial.print(request);

  while (responseSize != sizeof(remehaData)){
    responseSize = Serial.available();
    retries++;
    if (retries > 1000){
      break;
    }
    delay(1);
  }
  if (responseSize != sizeof(remehaData)){
    server.send(500, "text/plain", "");
    return;
  }

  responseSize = Serial.readBytes((char *)&response, sizeof(remehaData));

  remehaPacket *packet = (struct remehaPacket * )((char *)&response);

  String idString = "mac=\"" + String(WiFi.macAddress().c_str()) + "\"} ";
  crc = CRC16_Modbus((uint8_t*)&packet->payload, sizeof(remehaPayload));
  message += "# packet->crc: " + String(packet->crc, HEX) + " crc: " + String(crc, HEX) + "\n";
  if (crc == packet->crc){
    message += "remeha_crc{" + idString + "1\n";
  } else {
    message += "remeha_crc{" + idString + "0\n";
    server.send(200, "text/plain", message);
    return;
  }

  // message += "# data: " + "\n";
  message += "remeha_response_size_bytes{" + idString + String(responseSize) + "\n";

  message += "remeha_temperature_c{name=\"aanvoer\", " + idString + String(response.aanvoer_temperature_c * 0.01) + "\n";
  message += "remeha_temperature_c{name=\"retour\", " + idString + String(response.retour_temperature_c * 0.01) + "\n";
  message += "remeha_temperature_c{name=\"zonneboiler\", " + idString + String(response.zonneboiler_temperature_c * 0.01) + "\n";
  message += "remeha_temperature_c{name=\"buiten\", " + idString + String(response.buiten_temperature_c * 0.01) + "\n";
  message += "remeha_temperature_c{name=\"boiler\", " + idString + String(response.boiler_temperature_c * 0.01) + "\n";

  message += "remeha_temperature_c{name=\"automaat\", " + idString + String(response.automaat_temperature_c * 0.01) + "\n";
  message += "remeha_temperature_c{name=\"ruimte\", " + idString + String(response.ruimte_temperature_c * 0.01) + "\n";

  message += "remeha_temperature_setpoint_c{name=\"cv\", " + idString + String(response.cv_setpoint_c * 0.01) + "\n";
  message += "remeha_temperature_setpoint_c{name=\"ww\", " + idString + String(response.ww_setpoint_c * 0.01) + "\n";
  message += "remeha_temperature_setpoint_c{name=\"ruimte\", " + idString + String(response.ruimte_setpoint_c * 0.01) + "\n";

  message += "remeha_speed_setpoint_rpm{name=\"ventilator\", " + idString + String(response.ventilator_setpoint_rpm) + "\n";
  message += "remeha_speed_rpm{name=\"ventilator\", " + idString + String(response.ventilator_touren_rpm) + "\n";

  message += "remeha_ionazation_current_ma{" + idString + String(response.ionizatie_current_a * 0.1) + "\n";

  message += "remeha_temperature_setpoint_c{name=\"intern\", " + idString + String(response.intern_setpoint_c * 0.01) + "\n";

  message += "remeha_power_percent{name=\"beschikbaar\", " + idString + String(response.vermogen_beschikbaar_percent) + "\n";

  message += "remeha_pump_percent{" + idString + String(response.pomp_percent) + "\n";

  message += "remeha_power_percent{name=\"gewenst\", " + idString + String(response.vermogen_gewenst_percent) + "\n";
  message += "remeha_power_percent{name=\"geleverd\", " + idString + String(response.vermogen_geleverd_percent) + "\n";

  message += "remeha_pressure_bar{ " + idString + String(response.druk_bar * 0.1) + "\n";

  message += "remeha_flowrate_lm{name=\"ww\" " + idString + String(response.ww_flowrate * 0.01) + "\n";

  message += "remeha_state_number{" + idString + String(response.state) + "\n";
  if (response.substate != 9){
    message += "remeha_substate_number{" + idString + String(response.substate) + "\n";
  }
  message += "remeha_lockout_number{" + idString + String(response.lockout) + "\n";
  message += "remeha_blocking_number{" + idString + String(response.blocking) + "\n";

  for (int i=0; i<=17; i++){
    String state = remeha_state[i];
    if (state == "-"){continue;}
    message += "remeha_state{name=\"" + state + "\", state=\"" + String(i) + "\", " + idString;
    if (response.state == i){
       message += "1\n";
    } else {
      message += "0\n";
    }
  }

  for (int i=0; i<=63; i++){
    String substate = remeha_substate[i];
    if (substate == "-"){continue;}
    message += "remeha_substate{name=\"" + substate + "\", substate=\"" + String(i) + "\", " + idString;
    if (response.substate == i){
       message += "1\n";
    } else {
      message += "0\n";
    }
  }

  for (uint i=0; i<8; i++){
    if (String(remeha_bits_demand_source[i]) == ""){continue;}
    message += "remeha_bit{name=\"" + String(remeha_bits_demand_source[i]) + "\", " + idString + String(0 + ((response.demand_source_bits >> i) & 0x01)) + "\n";
  }

  for (uint i=0; i<8; i++){
    if (String(remeha_bits_input[i]) == ""){continue;}
    message += "remeha_bit{name=\"" + String(remeha_bits_input[i]) + "\", " + idString + String(0 + ((response.input_bits >> i) & 0x01)) + "\n";
  }

  for (uint i=0; i<8; i++){
    if (String(remeha_bits_valve[i]) == ""){continue;}
    message += "remeha_bit{name=\"" + String(remeha_bits_valve[i]) + "\", " + idString + String(0 + ((response.valve_bits >> i) & 0x01)) + "\n";
  }

  for (uint i=0; i<8; i++){
    if (String(remeha_bits_pump[i]) == ""){continue;}
    message += "remeha_bit{name=\"" + String(remeha_bits_pump[i]) + "\", " + idString + String(0 + ((response.pump_bits >> i) & 0x01)) + "\n";
  }


  // if (response.substate == 9){
  //   for (int i=0; i<=40; i++){
  //     String blocking = remeha_blocking[i];
  //     if (blocking == "-"){continue;}
  //     message += "remeha_blocking{name=\"" + blocking + "\", blocking=\"" + String(i) + "\", " + idString;
  //     if (response.substate == i){
  //        message += "1\n";
  //     } else {
  //       message += "0\n";
  //     }
  //   }
  // }

  // return 200;
  server.send(200, "text/plain", message);

}

void HandleRaw() {
  String message;
  int responseSize = 0;
  int retries = 0;
  char response[sizeof(remehaData)];

  clearSerialBuffer();

  Serial.print(request);

  while (responseSize != sizeof(remehaData)){
    responseSize = Serial.available();
    retries++;
    if (retries > 1000){
      break;
    }
    delay(1);
  }
  if (responseSize != sizeof(remehaData)){
    server.send(500, "text/plain", "");
    return;
  }

  responseSize = Serial.readBytes((char *)&response, sizeof(remehaData));

  server.send_P(200, "application/octet-stream", response, responseSize);
}

void HandleHex() {
  char message[sizeof(remehaStatus)*2];
  int responseSize = 0;
  int retries = 0;
  remehaPacket response[sizeof(remehaPacket)];
  uint16_t crc = 0;

  clearSerialBuffer();

  Serial.print(request);

  while (responseSize != sizeof(remehaPacket)){
    responseSize = Serial.available();
    retries++;
    if (retries > 1000){
      break;
    }
    delay(1);
  }

  if (responseSize != sizeof(remehaPacket)){
    server.send(500, "text/plain", "timeout");
    return;
  }

  responseSize = Serial.readBytes((char*)&response, sizeof(remehaPacket));

  if (responseSize != sizeof(remehaPacket)){
    server.send(500, "text/plain", "read_fail");
    return;
  }

  crc = CRC16_Modbus((uint8_t*)&response->payload, sizeof(remehaPayload));

  if (crc != response->crc){
    server.send(500, "text/plain", "crc_fail");
    return;
  }

  for (int i=0; i < responseSize; i++){
    sprintf(&message[i*2],"%02X", ((char *)&(&response->payload)->status)[i]);
  }

  server.send(200, "text/plain", message);

}


// void HandleRoot() {
//   String message;
//   int status = GenerateMetrics(message);
//   server.send(status, "text/plain", message);
// }

void HandleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (int i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/html", message);
}

void loop() {
  // Check for over the air update request and (if present) flash it
  ArduinoOTA.handle();

  server.handleClient();
}