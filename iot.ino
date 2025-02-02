#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

//PARA VISUALIZAR O DISPOSITIVO VIA AZURE DA PARA RODAR ESSE COMANDO VIA CLI: az iot hub monitor-events --hub-name <SEU HUB EVENTO> --device-id calu, ONDE AINDA ADICIONANDO UM >a.txt da pra persistir o dado em um txt se for o caso

// Configurações de Wi-Fi
const char* ssid = "rot_fundo";
const char* password = "<SENHA>";

// Configurações do Azure IoT Hub
const char* mqttServer = "<SEU AZURE HOST>.azure-devices.net";
const int mqttPort = 8883;
const char* deviceID = "calu";
const char* sasToken =  RODAR ISSO NO AZURE CLI E PREENCHER O RESTO ENTRE ASPAS<az iot hub generate-sas-token --device-id SEU ID DE DISPOSIIVO --hub-name NOME DO SEU HUB>;


WiFiClientSecure espClient;
PubSubClient client(espClient);

void connectWiFi() {
    Serial.print("Conectando ao Wi-Fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Conectado ao Wi-Fi!");
}

void connectMQTT() {
    client.setServer(mqttServer, mqttPort);
    espClient.setInsecure(); // Para evitar problemas com certificados SSL

    String username = String(mqttServer) + "/" + deviceID + "/?api-version=2021-04-12";

    Serial.print("Conectando ao Azure IoT Hub...");
    while (!client.connected()) {
        if (client.connect(deviceID, username.c_str(), sasToken)) {
            Serial.println("Conectado!");
        } else {
            Serial.print("Falha, rc=");
            Serial.print(client.state());
            Serial.println(" tentando novamente em 5 segundos...");
            delay(5000);
        }
    }
}


void sendMessage() {
    String payload = "{\"temperature\": 25.5, \"humidity\": 60}";
    String topic = String("devices/") + deviceID + "/messages/events/";
    
    if (client.publish(topic.c_str(), payload.c_str())) {
        Serial.println("Mensagem enviada: " + payload);
    } else {
        Serial.println("Falha ao enviar mensagem");
    }
}

void setup() {
    Serial.begin(115200);
    connectWiFi();
    connectMQTT();
}

void loop() {
    if (!client.connected()) {
        connectMQTT();
    }
    client.loop();
    sendMessage();
    delay(60000); // Envia a cada 10 segundos
}
