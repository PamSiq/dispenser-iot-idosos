# Documentação das Interfaces

## 🔌 Protocolos de Comunicação

### MQTT (Message Queuing Telemetry Transport)
- **Broker:** broker.hivemq.com
- **Porta:** 1883
- **Tópicos:**
  - `dispenser/status` - Status do sistema
  - `dispenser/alert` - Alertas de medicação
  - `dispenser/medication` - Confirmações

### WiFi
- **Protocolo:** 802.11 b/g/n
- **Segurança:** WPA/WPA2
- **Configuração:** Credenciais no código

## 📡 Módulos de Comunicação

### ESP32 WiFi
- Interface nativa com stack TCP/IP
- Suporte a SSL/TLS
- Configuração via biblioteca WiFi.h

### I2C
- Comunicação com módulo RTC DS3231
- Pinos: SDA (21), SCL (22)
- Velocidade: 100kHz
