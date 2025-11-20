#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"


// ====== CONFIG WI-FI (Wokwi) ======
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ====== CONFIG MQTT ======
const char* mqtt_server = "test.mosquitto.org";
const int   mqtt_port   = 1883;
const char* mqtt_topic  = "workbalance/estacoes/desk-01/ambiente";

// ====== OBJETOS WI-FI / MQTT ======
WiFiClient espClient;
PubSubClient client(espClient);

// ====== SENSORES ======
DHTesp dht;
const int DHT_PIN = 15;   // DATA do DHT22 no GPIO 15
const int LDR_PIN = 34;   // AO do LDR no GPIO 34

// ====== CONTROLE DE ENVIO ======
unsigned long lastMsg = 0;
const long interval = 5000; // 5 segundos

void setupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop até conectar
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    // clientId único
    String clientId = "workbalance-desk-01-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("conectado!");
      // Se quiser assinar algo depois, faz aqui (client.subscribe...)
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Inicializa DHT22
  dht.setup(DHT_PIN, DHTesp::DHT22);

  // LDR como entrada analógica
  pinMode(LDR_PIN, INPUT);

  // Conexão WiFi
  setupWifi();

  // Config MQTT
  client.setServer(mqtt_server, mqtt_port);

  // Semente pro random (usa ruído do ADC)
  randomSeed(analogRead(0));
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;

    // ===== Leitura sensores =====
    TempAndHumidity data = dht.getTempAndHumidity();
    float temperatura = data.temperature; // °C

    int ldrValue = analogRead(LDR_PIN); // 0–4095
    // Converte pra um "nível de luminosidade" genérico (0–1000)
    float luminosidade = (ldrValue / 4095.0) * 1000.0;

    // "Sensor" de ruído simulado (40–90 dB)
    float ruido = random(400, 900) / 10.0;

    // ===== Monta JSON =====
    // Exemplo:
    // {
    //   "estacaoId": "desk-01",
    //   "temperatura": 25.3,
    //   "ruido": 62.1,
    //   "luminosidade": 430,
    //   "timestamp": 123456
    // }

    unsigned long timestamp = now; // usando millis como timestamp

    String payload = "{";
    payload += "\"estacaoId\":\"desk-01\",";
    payload += "\"temperatura\":" + String(temperatura, 1) + ",";
    payload += "\"ruido\":" + String(ruido, 1) + ",";
    payload += "\"luminosidade\":" + String(luminosidade, 0) + ",";
    payload += "\"timestamp\":" + String(timestamp);
    payload += "}";

    // ===== Publica no MQTT =====
    Serial.print("Publicando em ");
    Serial.print(mqtt_topic);
    Serial.print(": ");
    Serial.println(payload);

    client.publish(mqtt_topic, payload.c_str());
  }
}
