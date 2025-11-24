// =======================================================================
// DISPENSER INTELIGENTE IoT COM MQTT - Versao Wokwi
// Autor: Pamela Bispo Siqueira
// =======================================================================

#include <WiFi.h>
#include <PubSubClient.h>

// ==================== CONFIGURACOES WIFI ====================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ==================== CONFIGURACOES MQTT ====================
const char* mqtt_server = "broker.mqtt.cool";
const int mqtt_port = 1883;
const char* mqtt_client_id = "dispenser_mackenzie_pamela";

// TOPICOS MQTT
const char* topic_status     = "dispenser/status";
const char* topic_alert      = "dispenser/alert";
const char* topic_medication = "dispenser/medication";
const char* topic_times      = "dispenser/times";

// ==================== PINOS DO SISTEMA ====================
#define SENSOR_PIN 12
#define BUZZER_PIN 14
#define LED_VERDE 2
#define LED_VERMELHO 4
#define LED_AZUL 5

// ==================== OBJETOS ====================
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ==================== VARIAVEIS IMPORTANTES ====================
unsigned long tempos_simulados[] = {15000, 25000, 35000, 45000};
int indiceTempo = 0;

unsigned long inicioCiclo = 0;
bool alertaDisparado = false;
bool medicamentoRetirado = false;

// ==================== CALLBACK MQTT ====================
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("\n[MQTT] ");
  Serial.print(topic);
  Serial.print(" -> ");
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)message[i];
  Serial.println(msg);
}

// ==================== WIFI ====================
void conectarWiFi() {
  Serial.println("\nCONECTANDO WIFI");
  WiFi.begin(ssid, password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    Serial.print(".");
    delay(400);
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWIFI OK");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_VERDE, HIGH);
  } else {
    Serial.println("\nERRO WIFI");
  }
}

// ==================== MQTT ====================
void conectarMQTT() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);

  Serial.println("Conectando MQTT...");

  while (!mqttClient.connected()) {
    if (mqttClient.connect(mqtt_client_id)) {
      Serial.println("MQTT OK");
      digitalWrite(LED_AZUL, HIGH);

      mqttClient.subscribe(topic_status);
      mqttClient.subscribe(topic_alert);
      mqttClient.subscribe(topic_medication);

      mqttClient.publish(topic_status, "Sistema conectado MQTT");
    } else {
      Serial.print("Erro rc=");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}

// ==================== PUBLICAR ====================
void publicar(const char* topico, const char* mensagem) {
  if (mqttClient.connected()) {
    mqttClient.publish(topico, mensagem);
    Serial.print("MQTT -> ");
    Serial.print(topico);
    Serial.print(": ");
    Serial.println(mensagem);

    digitalWrite(LED_VERMELHO, HIGH);
    delay(60);
    digitalWrite(LED_VERMELHO, LOW);
  }
}

// ==================== ALERTA ====================
void emitirAlerta() {
  publicar(topic_alert, "ALERTA: Hora do remedio");

  Serial.println("=== ALERTA DISPARADO ===");

  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_VERDE, LOW);
    delay(250);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_VERDE, HIGH);
    delay(250);
  }

  alertaDisparado = true;       // agora pode retirar
  medicamentoRetirado = false;  // aguardando retirada
}

// ==================== DETECTAR RETIRADA AUTOMATICA ====================
void verificarRetirada() {
  if (alertaDisparado && !medicamentoRetirado) {
    medicamentoRetirado = true;

    publicar(topic_medication, "Medicamento retirado");

    Serial.println("=== REMEDIO RETIRADO AUTOMATICAMENTE ===");

    // Proximo horario
    indiceTempo++;
    if (indiceTempo >= 4) indiceTempo = 0;

    // Reinicia a contagem
    inicioCiclo = millis();
    alertaDisparado = false;
  }
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);

  conectarWiFi();
  conectarMQTT();

  publicar(topic_status, "Sistema iniciado");

  inicioCiclo = millis();
}

// ==================== LOOP ====================
void loop() {
  if (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
    conectarMQTT();
  }

  mqttClient.loop();

  unsigned long tempoDecorrido = millis() - inicioCiclo;

  // Dispara alerta no tempo correto
  if (!alertaDisparado && tempoDecorrido >= tempos_simulados[indiceTempo]) {
    emitirAlerta();
  }

  // Retirada automática após alerta
  verificarRetirada();

  delay(50);
}
