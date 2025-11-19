# Descrição do Hardware

## Componentes Utilizados

| Componente | Descrição |
|------------|-----------|
| ESP32 | Microcontrolador com WiFi e Bluetooth |
| DS3231 | Módulo RTC (Real Time Clock) |
| Servomotor SG90 | Atuador para liberar medicamentos |
| Reed Switch | Sensor magnético para detectar abertura da tampa |
| Buzzer | Alerta sonoro |
| LEDs | Alertas visuais (verde, vermelho, azul) |
| Resistores | 220Ω para os LEDs |
| Protoboard | Para montagem do circuito |
| Fios e cabos | Conexões |

## Diagrama Esquemático

![Diagrama Esquemático](../images/diagrama_esquematico.png)

## Montagem

1. Conecte o ESP32 à protoboard.
2. Conecte o DS3231 via I2C (SDA: pino 21, SCL: pino 22).
3. Conecte o servomotor ao pino 13.
4. Conecte o reed switch ao pino 12 (com pull-up interno).
5. Conecte o buzzer ao pino 14.
6. Conecte os LEDs:
   - Verde: pino 2
   - Vermelho: pino 4
   - Azul: pino 5
   - Cada LED com resistor de 220Ω no terra.

## Estrutura Mecânica

O dispenser possui uma estrutura impressa em 3D para organizar os componentes e o compartimento de medicamentos. O desenho da estrutura está disponível em [../hardware/estrutura_3d.stl](../hardware/estrutura_3d.stl).
