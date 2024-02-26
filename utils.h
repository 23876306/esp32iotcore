#include "certs.h"               // 引入 AWS IoT 相關的證書設定

#include <Arduino_BuiltIn.h>     // 引入 Arduino 內建庫
#include <WiFiClientSecure.h>    // 引入安全的 WiFi 用戶端庫
#include <PubSubClient.h>        // 引入 MQTT 用戶端庫
#include <ArduinoJson.h>         // 引入 Arduino JSON 库
#include "WiFi.h"                // 引入 WiFi 相關的庫

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub/weather"  // 定義 AWS IoT 發佈主題
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"        // 定義 AWS IoT 訂閱主題


WiFiClientSecure net = WiFiClientSecure();   // 建立安全的 WiFi 用戶端
PubSubClient client(net);                    // 建立 MQTT 用戶端


void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);              // 反序列化收到的 JSON 資料
  const char* message = doc["message"];       // 提取 JSON 中的訊息欄位
  Serial.println(message);
}

void connectAWS() {
  WiFi.mode(WIFI_STA);                        // 設置 WiFi 模式為站點模式
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);       // 連接 WiFi

  Serial.println("Connecting to Wi-Fi");

  // 等待 WiFi 連接成功
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // 設置 WiFiClientSecure 使用 AWS IoT 裝置證書
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);



  // 連接到之前定義的 AWS 端點的 MQTT 經紀人
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // 設置訊息處理程序
  client.setCallback(messageHandler);

  Serial.println("Connecting to AWS IOT");

  // 嘗試連接到 AWS IoT
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  // 檢查是否成功連接
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // 訂閱訂閱主題
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage(float temperature, float humidity, const char* timeString, unsigned long timeStamp) {
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;     // 將溫度值添加到 JSON 文檔
  doc["humidity"] = humidity;           // 將濕度值添加到 JSON 文檔
  doc["timeString"] = timeString;       // 將時間字串添加到 JSON 文檔
  doc["timeStamp"] = timeStamp;         // 將時間戳記添加到 JSON 文檔

  char jsonBuffer[512];                      // 創建緩衝區以保存 JSON 文檔
  serializeJson(doc, jsonBuffer);            // 序列化 JSON 文檔

  // 發佈 JSON 文檔到指定主題
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
