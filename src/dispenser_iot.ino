/*
 * Dispenser Inteligente de Medicamentos com IoT
 * Autor: Pamela Bispo Siqueira
 * Universidade Presbiteriana Mackenzie
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "RTClib.h"
#include <Servo.h>

// Configurações WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Configurações MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Tópicos MQTT
const char* topic_status = "dispenser/status";
const char* topic_alert = "dispenser/alert";
const char* topic_medication = "dispenser/medication";

// Componentes
RTC_DS3231 rtc;
Servo servo;
const int servoPin = 13;
const int reedSwitch = 12;
const int buzzer = 14;
const int ledGreen = 2;
const int ledRed = 4;
const int ledBlue = 5;

// Horários de medicação
int medicationTimes[][2] = {
  {9, 0},   // 09:00
  {14, 0},  // 14:00  
  {19, 0}   // 19:00
};

WiFiClient espClient;
PubSubClient client(espClient);

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando à rede: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    
    if (client.connect("ESP32_Dispenser")) {
      Serial.println("conectado!");
      client.publish(topic_status, "Sistema iniciado");
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000);
    }
  }
}

void dispenseMedication() {
  Serial.println("💊 Liberando medicamento...");
  
  // Publicar via MQTT
  client.publish(topic_medication, "Medicamento liberado");
  client.publish(topic_alert, "Alerta: horário da medicação");
  
  // Acionar servo
  servo.write(90);
  
  // Alertas visuais e sonoros
  digitalWrite(ledRed, HIGH);
  tone(buzzer, 1000, 2000);
  
  delay(3000);
  
  // Retornar servo
  servo.write(0);
  digitalWrite(ledRed, LOW);
}

void checkMedicationTime() {
  DateTime now = rtc.now();
  
  for (int i = 0; i < 3; i++) {
    if (now.hour() == medicationTimes[i][0] && 
        now.minute() == medicationTimes[i][1] &&
        now.second() == 0) {
      dispenseMedication();
    }
  }
}

void checkLidSensor() {
  if (digitalRead(reedSwitch) == LOW) {
    client.publish(topic_medication, "Tampa aberta - medicamento retirado");
    digitalWrite(ledBlue, HIGH);
    delay(1000);
    digitalWrite(ledBlue, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inicializar componentes
  if (!rtc.begin()) {
    Serial.println("❌ RTC não encontrado!");
    while (1);
  }
  
  servo.attach(servoPin);
  pinMode(reedSwitch, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  
  // Configurar WiFi e MQTT
  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);
  
  // LED verde indica sistema pronto
  digitalWrite(ledGreen, HIGH);
  
  Serial.println("✅ Sistema iniciado com sucesso!");
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
  
  checkMedicationTime();
  checkLidSensor();
  
  delay(1000);
}
