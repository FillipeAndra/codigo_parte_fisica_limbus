#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>

#define SERIAL_PORT    1  // Porta Serial do ESP32 (pode ser UART1 ou outra)
#define BAUD_RATE      9600  // Taxa de comunicação serial
#define API_URL        "http://18.231.161.105:8000/home/"  // URL da sua API

const char* ssid = "NET_2GB984B2";       // Substitua pelo nome da sua rede Wi-Fi
const char* password = "19B984B2";  // Substitua pela sua senha de Wi-Fi

HardwareSerial mySerial(1);  // Use o UART1 (pode ser UART0, UART1, ou UART2 dependendo da sua configuração)

void setup() {
  // Inicializa o Monitor Serial
  Serial.begin(115200);
  mySerial.begin(BAUD_RATE, SERIAL_8N1, 16, 17);  // Pinos RX e TX para o UART1

  // Conexão com Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Conectando ao Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    if (millis() > 15000) { // Timeout de 15 segundos
      Serial.println("Falha ao conectar ao Wi-Fi!");
      return;
    }
  }
  Serial.println("\nConectado ao Wi-Fi!");
}

bool isValidJson(String data) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, data);
  if (error) {
    Serial.print("Erro na validação JSON: ");
    Serial.println(error.c_str());
    return false;
  }
  return true;
}

void loop() {
  // Verifica se há dados na porta serial
  if (mySerial.available() > 0) {
    String data = mySerial.readStringUntil('\n');  // Lê a linha da porta serial
    data.trim();  // Remove espaços em branco extras

    Serial.print("Dados recebidos: ");
    Serial.println(data);

    if (isValidJson(data)) {  // Verifica se os dados são JSON válidos
      Serial.println("Dados em formato JSON válido!");

      // Preparando o JSON para enviar à API
      HTTPClient http;
      http.begin(API_URL);  // URL da API
      http.addHeader("Content-Type", "application/json");

      // Envia a requisição POST
      int httpCode = http.POST(data);

      if (httpCode == 201) {
        Serial.println("Dados enviados com sucesso!");
        String payload = http.getString();  // Resposta da API
        Serial.println("Resposta da API: " + payload);
      } else {
        Serial.printf("Erro ao enviar dados: %d - %s\n", httpCode, http.errorToString(httpCode).c_str());
      }

      http.end();  // Finaliza a conexão HTTP
    } else {
      Serial.println("Dados recebidos não estão em formato JSON válido.");
    }
  }

  delay(1000);  // Pequeno atraso para não sobrecarregar o loop
}
