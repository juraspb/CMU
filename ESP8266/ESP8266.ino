#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266FtpServer.h>
#include <WebSocketsServer.h> 
#include <Hash.h>
#include <FS.h>
#include <ArduinoOTA.h>

// Update to contain your network information :)
const char *ssid = "smurf2G";
//const char *ssid = "netis_evs";
const char *password = "9811279128";
const int led = 2;
const int qr_enable = 4;
const int qr_start = 5;
bool qr_active = false;

// A few variables which we'll need throughout the program
String qr_string = "qr";
String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

// Initialise websockets, web server and servo
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial

void setup(void){
  ArduinoOTA.setHostname("QR-Reader");// Имя хоста
  //ArduinoOTA.setPassword((const char *)"4011");// Пароль для подключения к хосту. Если не нужен — комментируем эту строку
  ArduinoOTA.begin();// Инициализация

  // Wait a second before we jump into starting everything
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  pinMode(qr_start, OUTPUT);
  digitalWrite(qr_start, 1);
  pinMode(qr_enable, OUTPUT);
  digitalWrite(qr_enable, 0);
  inputString.reserve(500);

  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring wifi...");

  // If you'd like to use the ESP as a wifi access point instead of a client, 
  // comment out everything between HERE...

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  IPAddress myIP = WiFi.localIP();

  delay(1500);

  ftpSrv.begin("root","root");    

  // AND HERE... Then uncomment these 2 lines:
  // WiFi.softAP(ssid);
  // IPAddress myIP = WiFi.softAPIP();

  // Print out the IP address
  Serial.print("IP address: ");
  Serial.println(myIP);

  SPIFFS.begin();      // Begin access to our file system (which stores the HTML)

  webSocket.begin();   // Start the websockets and link the events function (defined below, under the loop)
  webSocket.onEvent(webSocketEvent);

  // Configure web server to host HTML files
  server.on("/qr_read_on", [](){                                        // При HTTP запросе вида http://192.168.4.1/qr_read
      server.send(200, "text/plain", qr_read_on());                     // Отдаём клиенту код успешной обработки запроса, сообщаем, что формат ответа текстовый и возвращаем результат выполнения функции qr_read 
  });
  server.on("/qr_read_off", [](){                                        // При HTTP запросе вида http://192.168.4.1/qr_read
      server.send(200, "text/plain", qr_read_off());                     // Отдаём клиенту код успешной обработки запроса, сообщаем, что формат ответа текстовый и возвращаем результат выполнения функции qr_read 
  });
  server.on("/qr_status", [](){                                        // При HTTP запросе вида http://192.168.4.1/qr_status
      server.send(200, "text/plain", qr_status());                     // Отдаём клиенту код успешной обработки запроса, сообщаем, что формат ответа текстовый и возвращаем результат выполнения функции qr_status 
  });
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });
  server.begin();
  Serial.println("HTTP server started");
  delay(500);
  Serial.swap();
  Serial.flush();
}

void loop(void) {
  // Process any incoming HTTP or WebSockets requests
  webSocket.loop();
  server.handleClient();
  ftpSrv.handleFTP(); 
  ArduinoOTA.handle();

  if (stringComplete) {
    digitalWrite(qr_start, 1);
    //digitalWrite(led,1);
    qr_string = inputString;
    webSocket.broadcastTXT(qr_string.c_str());
    Serial.println(qr_string);
    inputString = "";
    stringComplete = false;
    if (qr_active) {
      delay(500);
      digitalWrite(qr_start, 0);
      //digitalWrite(led,0);
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') stringComplete = true;
  }
}

String qr_read_on() {                                                 // Функция переключения реле 
  qr_active = true;
  digitalWrite(qr_start, 0);
  digitalWrite(led,0);
  qr_string="send = ON";
  stringComplete = true;
  return String("1");
}

String qr_read_off() {                                                 // Функция переключения реле 
  qr_active = false;
  digitalWrite(qr_start, 1);
  digitalWrite(led,1);
  qr_string="send = OFF";
  stringComplete = true;
  return String("0");
}

String qr_status() {                                                 // Функция для определения текущего статуса реле 
  byte state;
  if (digitalRead(qr_start))                                             // Если на пине реле высокий уровень   
    state = 1;                                                          //  то запоминаем его как единицу
  else                                                                  // иначе
    state = 0;                                                          //  запоминаем его как ноль
  Serial.println("stat");
  return String(state);                                                 // возвращаем результат, преобразовав число в строку
}

// A function to handle our incoming sockets messages
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch(type) {
    // Runs when a user disconnects
    case WStype_DISCONNECTED: {
      //Serial.printf("User #%u - Disconnected!\n", num);
      break;
    }
    // Runs when a user connects
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("--- Connection. IP: %d.%d.%d.%d Namespace: %s UserID: %u\n", ip[0], ip[1], ip[2], ip[3], payload, num);
      // Send last pot value on connect
      webSocket.broadcastTXT(qr_string);
      break;
    }
    // Runs when a user sends us a message
    case WStype_TEXT: {
      String incoming = "";
      for (int i = 0; i < lenght; i++) {
        incoming.concat((char)payload[i]);
      }
      uint8_t deg = incoming.toInt();
      //if (deg>128) digitalWrite(led,1);
      //        else digitalWrite(led,0);
      break;
    }
  }
}

// A function we use to get the content type for our HTTP responses
String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

// Takes a URL (for example /index.html) and looks up the file in our file system,
// Then sends it off via the HTTP server!
bool handleFileRead(String path){
  #ifdef DEBUG
    Serial.println("handleFileRead: " + path);
  #endif
  if(path.endsWith("/")) path += "index.html";
  if(SPIFFS.exists(path)){
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, getContentType(path));
    file.close();
    return true;
  }
  return false;
}
