#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WebSocketsServer.h> 
#include <SimpleFTPServer.h>
#include <SPIFFS.h>
#include <FS.h>
#include <BluetoothSerial.h>
//#include <HardwareSerial.h>


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define FORMAT_SPIFFS_IF_FAILED true
#define RXD2 16
#define TXD2 17

// Update to contain your network information :)
const char *ssid = "smurf2G";
//const char *ssid = "netis_evs";
const char *password = "9811279128";
const int qr_enable = 4;
const int qr_start = 5;
bool qr_active = false;
bool wifiConnected = false;
int webSocketConnected = 0;
int timerCount = 0;

// A few variables which we'll need throughout the program
String qr_string = "qr";
String inputString = "";      // a String to hold incoming data from Serial
String inputQRString = "";    // a String to hold incoming data from Serial2

// Initialise BTSerial, WebServer, WebSockets, FtpServer
BluetoothSerial SerialBT;
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial
//HardwareSerial qrSerial(2);

void setup(void){
  // Wait a second before we jump into starting everything
  pinMode(qr_start, OUTPUT);
  digitalWrite(qr_start, 1);
  pinMode(qr_enable, OUTPUT);
  digitalWrite(qr_enable, 0);
  inputString.reserve(100);
  inputQRString.reserve(100);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring Serial2...");
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println();
  Serial.println("Configuring BTSerial...");
  SerialBT.begin("QR Reader"); //Bluetooth device name
  Serial.println();
  Serial.println("Configuring wifi...");

  // If you'd like to use the ESP as a wifi access point instead of a client, 
  // comment out everything between HERE...

  WiFi.begin(ssid, password);
  while ((WiFi.status() != WL_CONNECTED)&(timerCount<50)) {
    delay(100);
    Serial.print(".");
    timerCount++;
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected=true;
    IPAddress myIP = WiFi.localIP();
    delay(1500);

    // AND HERE... Then uncomment these 2 lines:
    // WiFi.softAP(ssid);
    // IPAddress myIP = WiFi.softAPIP();

    // Print out the IP address
    Serial.print("QR reader IP address: ");
    Serial.println(myIP);
  }
  else {
    Serial.println("WIFI not found");
  }

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return;
  }
  else {
      Serial.println("SPIFFS Mounted");
  }
  
  if (wifiConnected) {
    ftpSrv.begin("root","root");    
    ftpSrv.setCallback(FTPcallback);
    ftpSrv.setTransferCallback(transferCallback);
    Serial.println("FTP server started");
    
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
  }
}

void loop(void) {
  // Process any incoming HTTP or WebSockets requests
  if (wifiConnected) {
    webSocket.loop();
    server.handleClient();
    ftpSrv.handleFTP(); 
  }  

  if (SerialBT.available()) {
    char inChar = SerialBT.read();
    switch (inChar){
      case '0': Serial.println(qr_read_off()); break;
      case '1': Serial.println(qr_read_on()); break;
    }
  }

  while (Serial2.available()) {
    char inChar = (char)Serial2.read();
    inputQRString += inChar;
    if (inChar == '\n') {
      digitalWrite(qr_start, 1);
      qr_string = inputQRString;
      SerialBT.print(qr_string);
      Serial.println(qr_string);
      if (webSocketConnected!=0) webSocket.broadcastTXT(qr_string.c_str());
      inputQRString = "";
      if (qr_active) {
        delay(1000);
        digitalWrite(qr_start, 0);
      }
    }
  }

  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      // 
      Serial.println(inputString);
      inputString = "";
    }
  }
}

//  Serial.println("---cp2---");

String qr_read_on() {  
  qr_active = true;
  digitalWrite(qr_start, 0);
  return String("1");
}

String qr_read_off() {   
  qr_active = false;
  digitalWrite(qr_start, 1);
  return String("0");
}

String qr_status() { 
  byte state;
  if (digitalRead(qr_start))  
    state = 1;
  else 
    state = 0; 
  Serial.println("stat");
  return String(state);   
}

// A function to handle our incoming sockets messages
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch(type) {
    // Runs when a user disconnects
    case WStype_DISCONNECTED: {
      Serial.printf("User #%u - Disconnected!\n", num);
      if (webSocketConnected>0) webSocketConnected--;
      Serial.println(webSocketConnected);
      break;
    }
    // Runs when a user connects
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("--- Connection. IP: %d.%d.%d.%d Namespace: %s UserID: %u\n", ip[0], ip[1], ip[2], ip[3], payload, num);
      Serial.println();
      // Send last pot value on connect
      webSocket.broadcastTXT(qr_string);
      webSocketConnected++;
      Serial.println(webSocketConnected);
      break;
    }
    // Runs when a user sends us a message
    case WStype_TEXT: {
      String incoming = "";
      for (int i = 0; i < lenght; i++) {
        incoming.concat((char)payload[i]);
      }
      uint8_t deg = incoming.toInt();
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

void FTPcallback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace){
  /* FTP_CONNECT,
   * FTP_DISCONNECT,
   * FTP_FREE_SPACE_CHANGE
   */
  Serial.printf("Connecting to ftp server >>>>> %d, freeSpace=%d, totalSpace=%d",ftpOperation,freeSpace,totalSpace);
  Serial.println();
  // freeSpace : totalSpace = x : 360
  if (ftpOperation == FTP_CONNECT) Serial.println(F("CONNECTED"));
  if (ftpOperation == FTP_DISCONNECT) Serial.println(F("DISCONNECTED"));
};

void transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize){
  Serial.printf(">>>>>>>> transferCallback %d, %s, %d",ftpOperation,name,transferredSize);
  /* FTP_UPLOAD_START = 0,
   * FTP_UPLOAD = 1,
   *
   * FTP_DOWNLOAD_START = 2,
   * FTP_DOWNLOAD = 3,
   *
   * FTP_TRANSFER_STOP = 4,
   * FTP_DOWNLOAD_STOP = 4,
   * FTP_UPLOAD_STOP = 4,
   *
   * FTP_TRANSFER_ERROR = 5,
   * FTP_DOWNLOAD_ERROR = 5,
   * FTP_UPLOAD_ERROR = 5
   */
  Serial.println();
};
