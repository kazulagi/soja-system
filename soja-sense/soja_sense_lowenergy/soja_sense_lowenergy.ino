/*
 
  This code is based on a sample code of Majenko Technologies, and can be used under GPL-v2 licence.
  Followings are the original copywrite.
 */


/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>



const char *ssid = "your_ssid_of_soja_spot";
const char *password = "your_password_of_soja_spot";

const int hr = 3; // wake up every 3 hours
const int voutPin = 34;
const int VOLT = 3.3 ; // 5.0 Vを電源とした場合
const int ANALOG_MAX = 4096; // ESP32の場合


WebServer server(80);
RTC_DATA_ATTR int bootCounter = 0;

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  Serial.print(min);
// R1の電圧を取得
    int reading = analogRead(voutPin)*100 / ANALOG_MAX;

    // AD値をmVに変換
    //float voltage = ((long)reading * VOLT * 1000) / ANALOG_MAX;

    //Serial.print(voltage);
    //Serial.print(" mV, ");

    // 電圧から電流を求める I=E/R (R=1000)
    //float microamp = (voltage * 1000) / 1000;

    //Serial.print(microamp);
    //Serial.println(" uA, ");
    
  snprintf(temp, 400,

           "<html>\
  <body>\
    <h1>soja!sense0001</h1>\
    <p>Uptime: %02d:%02d:%02d  Water content: %04d percent </p>\
  </body>\
</html>",

           hr, min % 60, sec % 60, reading
     
          );
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());



  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  bootCounter ++;
  Serial.printf("Boot counter(%08x) = %d\n", &bootCounter,bootCounter);

}

void loop(void) {
  int i;
  i=0;
  for (i=0; i<100; i++)
  {
  server.handleClient();

  // R1の電圧を取得
  int reading = analogRead(voutPin);

  // AD値をmVに変換
  float voltage = ((long)reading * VOLT * 1000) / ANALOG_MAX;

  //Serial.print(voltage);
  Serial.print(" mV, ");

  // 電圧から電流を求める I=E/R (R=1000)
  float microamp = (voltage * 1000) / 1000;

  Serial.print(microamp);
  Serial.println(" uA, ");

  // 電流をlxに変換
  //float lx = microamp / (290 / 100);

  //Serial.print(lx);
  //Serial.println(" lx");
  delay(1000);
  }
  int j;
  for (j=0; j< hr; j++){
    esp_sleep_enable_timer_wakeup( 60 * 1000 * 1000);  // wakeup every 3 hr s
    esp_deep_sleep_start();  
  }
}
