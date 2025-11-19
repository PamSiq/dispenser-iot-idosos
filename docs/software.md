# Documentação do Software

## 📋 Descrição
Código desenvolvido para o ESP32 que controla o dispenser inteligente de medicamentos.

## 🔧 Funcionalidades Implementadas
- Controle de horários via RTC DS3231
- Acionamento do servomotor SG90
- Detecção de tampa via reed switch
- Sistema de alertas (buzzer + LEDs)
- Comunicação MQTT com broker remoto

## 📚 Bibliotecas Utilizadas
- `WiFi.h` - Conexão WiFi
- `PubSubClient.h` - Cliente MQTT
- `Wire.h` - Comunicação I2C
- `RTClib.h` - Controle do RTC
- `Servo.h` - Controle do servomotor

## ⚙️ Configuração
1. Instale as bibliotecas na IDE Arduino
2. Configure as credenciais WiFi
3. Ajuste os horários de medicação no código
4. Faça upload para o ESP32

## 🎮 Controles
- Horários programados: 09:00, 14:00, 19:00
- Alertas: 5 segundos de buzzer + LED vermelho
- Confirmação: LED azul ao abrir tampa
