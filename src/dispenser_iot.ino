// DISPENSER INTELIGENTE IoT COM MQTT
// Autor: Pamela Bispo Siqueira
// Universidade Presbiteriana Mackenzie

#include <WiFi.h>
#include <PubSubClient.h>

// ==================== CONFIGURAÇÕES MQTT ====================
// CONFIGURAÇÕES OBRIGATÓRIAS PARA O PROTOCOLO MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_client_id = "dispenser_mackenzie_pamela";

// TÓPICOS MQTT - DEFINIDOS CONFORME EXIGÊNCIA
const char* topic_status = "dispenser/status";
const char* topic_alert = "dispenser/alert";
const char* topic_medication = "dispenser/medication";
const char* topic_times = "dispenser/times";

// ==================== CONFIGURAÇÕES WIFI ====================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ==================== CONFIGURAÇÕES DOS PINOS ====================
#define SERVO_PIN 13
#define SENSOR_PIN 12
#define BUZZER_PIN 14
#define LED_VERDE 2
#define LED_VERMELHO 4
#define LED_AZUL 5

// ==================== VARIÁVEIS GLOBAIS ====================
WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long temposAtuador[4] = {0, 0, 0, 0};
unsigned long temposSensor[4] = {0, 0, 0, 0};
int contadorAtuador = 0;
int contadorSensor = 0;

unsigned long tempoInicio;
bool sistemaAtivo = false;
bool mqttConectado = false;

// ==================== FUNÇÃO: CONEXÃO WiFi ====================
void conectarWiFi() {
  Serial.println();
  Serial.println("=== CONEXÃO WiFi ===");
  Serial.print("Conectando a: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
    digitalWrite(LED_VERDE, !digitalRead(LED_VERDE)); // Piscar LED durante conexão
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ WiFi CONECTADO!");
    Serial.print("📡 IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_VERDE, HIGH);
  } else {
    Serial.println();
    Serial.println("❌ WiFi FALHOU!");
    digitalWrite(LED_VERDE, LOW);
  }
}

// ==================== FUNÇÃO: CONEXÃO MQTT ====================
void conectarMQTT() {
  Serial.println();
  Serial.println("=== CONEXÃO MQTT ===");
  Serial.print("Conectando ao broker: ");
  Serial.println(mqtt_server);
  
  mqttClient.setServer(mqtt_server, mqtt_port);
  
  int tentativas = 0;
  while (!mqttClient.connected() && tentativas < 10) {
    Serial.print("Tentativa MQTT ");
    Serial.println(tentativas + 1);
    
    if (mqttClient.connect(mqtt_client_id)) {
      mqttConectado = true;
      Serial.println("✅ MQTT CONECTADO!");
      Serial.print("🔗 Client ID: ");
      Serial.println(mqtt_client_id);
      
      // Publicar mensagem de status inicial
      publicarMensagemMQTT(topic_status, "Sistema iniciado - MQTT conectado");
      publicarMensagemMQTT(topic_status, "Pronto para dispensar medicamentos");
      
    } else {
      Serial.print("❌ Falha MQTT, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" - Tentando novamente em 5s");
      delay(5000);
    }
    tentativas++;
    digitalWrite(LED_AZUL, !digitalRead(LED_AZUL)); // Piscar LED azul durante conexão MQTT
  }
  
  if (mqttConectado) {
    digitalWrite(LED_AZUL, HIGH);
  } else {
    digitalWrite(LED_AZUL, LOW);
    Serial.println("⚠️  Modo offline - MQTT não conectado");
  }
}

// ==================== FUNÇÃO: PUBLICAR MENSAGEM MQTT ====================
void publicarMensagemMQTT(const char* topico, const char* mensagem) {
  if (mqttClient.connected()) {
    bool sucesso = mqttClient.publish(topico, mensagem);
    if (sucesso) {
      Serial.print("📤 MQTT ENVIADO: ");
      Serial.print(topico);
      Serial.print(" -> ");
      Serial.println(mensagem);
    } else {
      Serial.println("❌ Falha ao publicar MQTT");
    }
  } else {
    Serial.print("⚠️  MQTT OFFLINE: ");
    Serial.print(topico);
    Serial.print(" -> ");
    Serial.println(mensagem);
  }
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.println("==========================================");
  Serial.println("🏥 DISPENSER INTELIGENTE IoT - MACKENZIE");
  Serial.println("==========================================");
  
  // Configurar pinos
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);
  
  // Inicializar LEDs
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(LED_AZUL, LOW);
  
  // Conectar redes
  conectarWiFi();
  conectarMQTT();
  
  tempoInicio = millis();
  sistemaAtivo = true;
  
  // Mensagem inicial do sistema
  Serial.println();
  Serial.println("=== SISTEMA INICIADO ===");
  Serial.println("📍 Componentes:");
  Serial.println("   - Servo: D13 | Sensor: D12 | Buzzer: D14");
  Serial.println("   - LEDs: D2(Verde), D4(Verm), D5(Azul)");
  Serial.println("⏰ Horarios: 15s, 25s, 35s, 45s");
  Serial.println("📡 MQTT: broker.hivemq.com:1883");
  Serial.println("==========================================");
  Serial.println();
}

// ==================== SIMULAÇÃO DO SERVO ====================
void simularServo(int angulo) {
  Serial.print("🔄 Servo movendo para ");
  Serial.print(angulo);
  Serial.println(" graus");
  
  // Simular movimento com delay proporcional
  int tempoMovimento = map(angulo, 0, 90, 200, 800);
  delay(tempoMovimento);
}

// ==================== SIMULAÇÃO DO BUZZER ====================
void simularBuzzer() {
  Serial.println("🔊 Buzzer ativado");
  publicarMensagemMQTT(topic_alert, "Alerta sonoro ativado");
  
  // Simular buzzer com piscadas
  for (int i = 0; i < 8; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_PIN, LOW);
    delay(150);
  }
}

// ==================== MEDIÇÃO DO ATUADOR ====================
void medirAtuador() {
  Serial.println();
  Serial.println("💊 INICIANDO DISPENSAÇÃO...");
  
  // Publicar MQTT - início da ação
  publicarMensagemMQTT(topic_medication, "Iniciando dispensação de medicamento");
  
  // ⏰ INICIAR MEDIÇÃO DE TEMPO
  unsigned long inicio = micros();
  
  // Executar ação do atuador
  simularServo(90); // Abrir
  digitalWrite(LED_VERMELHO, HIGH);
  simularBuzzer();
  
  // ⏰ FINALIZAR MEDIÇÃO
  unsigned long fim = micros();
  
  // Calcular e armazenar tempo
  if (contadorAtuador < 4) {
    temposAtuador[contadorAtuador] = (fim - inicio) / 1000;
    
    Serial.print("⏱️ TEMPO ATUADOR [");
    Serial.print(contadorAtuador + 1);
    Serial.print("]: ");
    Serial.print(temposAtuador[contadorAtuador]);
    Serial.println(" ms");
    
    // Publicar tempo via MQTT
    char msgTempo[60];
    sprintf(msgTempo, "Atuador_%d: %lu ms", contadorAtuador + 1, temposAtuador[contadorAtuador]);
    publicarMensagemMQTT(topic_times, msgTempo);
    
    contadorAtuador++;
  }
  
  // Finalizar ação
  delay(1000);
  simularServo(0); // Fechar
  digitalWrite(LED_VERMELHO, LOW);
  
  // Publicar confirmação MQTT
  publicarMensagemMQTT(topic_medication, "Medicamento dispensado com sucesso");
  Serial.println("✅ DISPENSAÇÃO CONCLUÍDA!");
}

// ==================== MEDIÇÃO DO SENSOR ====================
void medirSensor() {
  // ⏰ INICIAR MEDIÇÃO
  unsigned long inicio = micros();
  
  // Simular processamento
  digitalWrite(LED_AZUL, HIGH);
  
  // ⏰ FINALIZAR MEDIÇÃO
  unsigned long fim = micros();
  
  // Calcular e armazenar tempo
  if (contadorSensor < 4) {
    temposSensor[contadorSensor] = (fim - inicio) / 1000;
    
    Serial.print("⏱️ TEMPO SENSOR [");
    Serial.print(contadorSensor + 1);
    Serial.print("]: ");
    Serial.print(temposSensor[contadorSensor]);
    Serial.println(" ms");
    
    // Publicar via MQTT
    char msgTempo[60];
    sprintf(msgTempo, "Sensor_%d: %lu ms", contadorSensor + 1, temposSensor[contadorSensor]);
    publicarMensagemMQTT(topic_times, msgTempo);
    
    contadorSensor++;
  }
  
  // Publicar evento MQTT
  publicarMensagemMQTT(topic_medication, "Tampa aberta - medicamento retirado");
  Serial.println("📦 TAMPA ABERTA - Medicamento retirado");
  
  delay(800);
  digitalWrite(LED_AZUL, LOW);
}

// ==================== VERIFICAÇÃO DE HORÁRIOS ====================
void verificarHorarios() {
  if (!sistemaAtivo) return;
  
  unsigned long tempoAtual = (millis() - tempoInicio) / 1000;
  int horarios[] = {15, 25, 35, 45}; // Horários em segundos
  
  for (int i = 0; i < 4; i++) {
    if (tempoAtual == horarios[i] && contadorAtuador == i) {
      // Publicar alerta MQTT
      char msgAlerta[50];
      sprintf(msgAlerta, "Horario de medicamento: %d segundos", horarios[i]);
      publicarMensagemMQTT(topic_alert, msgAlerta);
      
      medirAtuador();
      break;
    }
  }
}

// ==================== EXIBIÇÃO DE RESULTADOS ====================
void exibirResultadosFinais() {
  if (contadorAtuador == 4 && contadorSensor == 4) {
    Serial.println();
    Serial.println("========================================");
    Serial.println("📊 RESULTADOS FINAIS DAS MEDICOES");
    Serial.println("========================================");
    
    // Calcular médias
    float mediaAtuador = (temposAtuador[0] + temposAtuador[1] + 
                         temposAtuador[2] + temposAtuador[3]) / 4.0;
    float mediaSensor = (temposSensor[0] + temposSensor[1] + 
                        temposSensor[2] + temposSensor[3]) / 4.0;
    
    // Publicar médias via MQTT
    char msgMedia[100];
    sprintf(msgMedia, "MEDIAS - Atuador: %.1f ms, Sensor: %.1f ms", mediaAtuador, mediaSensor);
    publicarMensagemMQTT(topic_times, msgMedia);
    
    Serial.println("🎉 MEDICOES CONCLUIDAS COM SUCESSO!");
    Serial.println("💾 Dados disponiveis via MQTT");
    
    // Efeito de conclusão
    for (int i = 0; i < 6; i++) {
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_VERMELHO, HIGH);
      digitalWrite(LED_AZUL, HIGH);
      delay(200);
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_VERMELHO, LOW);
      digitalWrite(LED_AZUL, LOW);
      delay(200);
    }
    
    // Manter LEDs acesos
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_VERMELHO, HIGH);
    digitalWrite(LED_AZUL, HIGH);
    
    // Parar execução
    while(true) {
      delay(1000);
    }
  }
}

// ==================== LOOP PRINCIPAL ====================
void loop() {
  // Manter conexão MQTT ativa
  if (!mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
    mqttConectado = false;
    digitalWrite(LED_AZUL, LOW);
    conectarMQTT();
  }
  
  if (mqttClient.connected()) {
    mqttClient.loop();
  }
  
  // Verificar horários e sensor
  verificarHorarios();
  
  if (digitalRead(SENSOR_PIN) == LOW) {
    medirSensor();
    delay(400); // Debounce
  }
  
  exibirResultadosFinais();
  delay(100);
}
