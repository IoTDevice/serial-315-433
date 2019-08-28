#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const int httpPort = 80;
String deviceName = "家用卷帘门";
String version = "1.0";
ESP8266WebServer server(httpPort);

unsigned char  up[7] = {0xfd,0x03,0x1a,0xd9,0x11,0x36,0xdf};
unsigned char  down[7] = {0xfd,0x03,0x1a,0xd9,0x18,0x36,0xdf};

// web服务器的根目录
void handleRoot() {
  server.send(200, "text/html", "<h1>this is index page from esp8266!</h1>");
}
// 操作LED开关状态的API
void handleLEDStatusChange(){
  String message = "{\"code\":0,\"message\":\"success\"}";
  for (uint8_t i=0; i<server.args(); i++){
    if (server.argName(i)=="status")
    {
      if (server.arg(i)=="up")
      {
        Serial.write(up,7);
      }else if (server.arg(i)=="down")
      {
        Serial.write(down,7);
      }
    }
  }
  server.send(200, "application/json", message);
}
// 设备改名的API
void handleDeviceRename(){
  String message = "{\"code\":0,\"message\":\"success\"}";
  for (uint8_t i=0; i<server.args(); i++){
    if (server.argName(i)=="name")
    {
      deviceName = server.arg(i);
    }
  }
  server.send(200, "application/json", message);
}
// 设备信息
void handleDeviceInfo(){
  String message;
  message = "{\n";
  message += "\"name\":\""+deviceName +"\",\n";
  message += "\"model\":\"com.iotserv.devices.serial-315-433\",\n";
  message += "\"mac\":\""+WiFi.macAddress()+"\",\n";
  message += "\"id\":\""+String(ESP.getFlashChipId())+"\",\n";
  message += "\"ui-support\":[\"web\",\"native\"],\n";
  message += "\"ui-first\":\"native\",\n";
  message += "\"author\":\"Farry\",\n";
  message += "\"email\":\"newfarry@126.com\",\n";
  message += "\"home-page\":\"https://github.com/iotdevice\",\n";
  message += "\"firmware-respository\":\"https://github.com/iotdevice/serial-315-433\",\n";
  message += "\"firmware-version\":\""+version+"\"\n";
  message +="}";
  server.send(200, "application/json", message);
}

// 页面或者api没有找到
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
  WiFi.mode(WIFI_STA);
  // 选取一种连接路由器的方式 
  // WiFi.begin(ssid, password);
  WiFi.beginSmartConfig();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }

  if (MDNS.begin("esp8266"+String(ESP.getFlashChipId()))) {
    // Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/botton", handleLEDStatusChange);
  server.on("/rename", handleDeviceRename);
  // about this device
  server.on("/info", handleDeviceInfo);
  server.onNotFound(handleNotFound);

  server.begin();
  // 软串口
  Serial.begin(9600);

  MDNS.addService("iotdevice", "tcp", httpPort);
}

void loop(void){
  MDNS.update();
  server.handleClient();
}
