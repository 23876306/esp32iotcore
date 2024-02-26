#include <Arduino_BuiltIn.h>   // 引入 Arduino 內建庫
#include "utils.h"             // 引入自定義的 utils.h 標頭檔
#include <PubSubClient.h>      // 引入 MQTT 客戶端庫
#include <WiFi.h>
#include <time.h>

const char* ssid = ""; // 更改為你的 WiFi SSID
const char* password = ""; // 更改為你的 WiFi 密碼
const char* ntpServer = "pool.ntp.org";
const long utcOffset = 28800; // 28800秒 = 8小时
const int daylightOffset = 0;

WiFiClient espClient;

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    configTime(utcOffset, daylightOffset, ntpServer);
    connectAWS();
}

void loop() {
    struct tm now;
    if (!getLocalTime(&now)) {
        Serial.println("Failed to obtain time");
        return;
    }

    char timeString[20];
    strftime(timeString, sizeof(timeString), "%Y/%m/%d %H:%M:%S", &now); 
    Serial.println(timeString);

    time_t timeStamp = time(NULL); // 獲取時間戳記

    int temperatureValue = random(10, 40);   
    int humidityValue = random(30, 70);       

    Serial.print(F("Temperature: "));        
    Serial.println(temperatureValue);        

    Serial.print(F("Humidity: "));           
    Serial.println(humidityValue);           
    
    publishMessage(temperatureValue, humidityValue, timeString, timeStamp); // 傳遞時間戳記

    client.loop();                            
    delay(3000);                             
}
