#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_MLX90614.h>

// Konfigurasi WiFi
const char* ssid = "YOUR SSID";        // Ganti dengan SSID WiFi Anda
const char* password = "YOUR PASSWORD"; // Ganti dengan password WiFi Anda

// Konfigurasi MQTT
const char* mqttServer = "BROKER IP ADDRESS / DOMAIN"; // Ganti dengan broker MQTT Anda
const int mqttPort = 1883;

// Topik untuk suhu
const char* topicAmbientTemp = "sensor/ambient_temp";
const char* topicObjectTemp = "sensor/object_temp";

// Inisialisasi WiFi dan MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Inisialisasi sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() 
{
  Serial.begin(9600);
  while (!Serial);

  // Inisialisasi sensor
  if (!mlx.begin()) 
  {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };

  // Menghubungkan ke WiFi
  connectToWiFi();

  // Konfigurasi MQTT client
  client.setServer(mqttServer, mqttPort);
  connectToMQTT();
}

void loop() 
{
  // Pastikan tetap terhubung ke MQTT
  if (!client.connected()) 
  {
    connectToMQTT();
  }
  client.loop();

  // Membaca suhu dari sensor
  float ambientTempC = mlx.readAmbientTempC();
  float objectTempC = mlx.readObjectTempC();

  // Mengirim data suhu lingkungan ke topik MQTT
  if (client.publish(topicAmbientTemp, String(ambientTempC, 2).c_str())) {
    Serial.println("Ambient temperature sent: " + String(ambientTempC, 2));
  } else {
    Serial.println("Failed to send ambient temperature.");
  }

  // Mengirim data suhu objek ke topik MQTT
  if (client.publish(topicObjectTemp, String(objectTempC, 2).c_str())) {
    Serial.println("Object temperature sent: " + String(objectTempC, 2));
  } else {
    Serial.println("Failed to send object temperature.");
  }

  delay(2000); // Delay 2 detik sebelum pengukuran berikutnya
}

void connectToWiFi() 
{
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void connectToMQTT() 
{
  Serial.print("Connecting to MQTT...");
  while (!client.connected()) 
  {
    if (client.connect("ESP32Client")) // Ganti "ESP32Client" dengan ID unik jika diperlukan
    {
      Serial.println("\nConnected to MQTT!");
    } 
    else 
    {
      Serial.print(".");
      delay(1000);
    }
  }
}