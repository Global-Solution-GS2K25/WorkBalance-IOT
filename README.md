# WorkBalance Hub – Módulo IoT

## 1. Visão geral

O **WorkBalance Hub** é uma solução focada em bem-estar no ambiente de trabalho,
ajudando empresas a monitorar as condições físicas de cada estação (temperatura,
ruído e luminosidade) e identificar situações de desconforto que podem impactar
a saúde e a produtividade das pessoas.

Este repositório traz o **módulo IoT** desenvolvido para a disciplina  
**Disruptive Architectures: IoT, IoB & Generative IA (FIAP)**.

## 2. Arquitetura IoT

Fluxo geral da solução:

ESP32 (Wokwi) → Broker MQTT → Node-RED (Gateway) → Dashboard em tempo real  
(opcional) → API/BD do WorkBalance Hub

**Componentes principais**

- **Hardware / Simulação**
  - ESP32 DevKit v1 (simulado no Wokwi)
  - Sensor DHT22 – leitura de temperatura
  - Módulo LDR – leitura de luminosidade (entrada analógica AO)
  - Ruído ambiente simulado via código C++ (valores aleatórios em dB)

- **Comunicação**
  - Protocolo **MQTT**
  - Broker público: `test.mosquitto.org`
  - Tópico de publicação:
    - `workbalance/estacoes/desk-01/ambiente`

- **Gateway**
  - **Node-RED** responsável por:
    - Receber as mensagens MQTT
    - Converter o payload JSON em objeto
    - Calcular o **índice de conforto** do ambiente
    - Enviar os dados para o **dashboard**

- **Dashboard**
  - Construído com **Node-RED Dashboard**, exibe:
    - Gráfico da temperatura ao longo do tempo
    - Gráfico do nível de ruído
    - Gráfico da luminosidade
    - Texto com o status do ambiente (Confortável / Desconfortável + motivos)

## 3. Payload e tópicos MQTT

### Tópico de publicação

```text
workbalance/estacoes/desk-01/ambiente
