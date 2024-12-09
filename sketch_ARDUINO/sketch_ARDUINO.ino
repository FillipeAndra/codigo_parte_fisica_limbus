float vazao;      // Variável para armazenar o valor em L/min
float mediaLm = 0;  // Variável para tirar a média em L/min a cada 1 minuto
float mediaPSI = 0;  // Variável para tirar a média em PSI a cada 1 minuto
int contaPulso;   // Variável para a quantidade de pulsos
int i = 0;        // Variável para contagem


void setup() {
  Serial.begin(9600); // Comunicação serial com o ESP32

  pinMode(2, INPUT);      // Sensor de vazão
  pinMode(3, OUTPUT);     // Configura o pino 3 para o sensor de pressão (SCK)
  pinMode(4, INPUT);      // Configura o pino 4 para o sensor de pressão (OUT)
  attachInterrupt(0, incpulso, RISING); // Configura o pino 2 como interrupção
  Serial.println("\n\nInicio\n\n"); // Indica início no monitor serial
}

void loop() {
  contaPulso = 0;   // Zera a contagem dos pulsos
  sei();            // Habilita interrupções
  delay(1000);      // Aguarda 1 segundo
  cli();            // Desabilita interrupções

  // Cálculo da vazão
  vazao = contaPulso / 5.5; // Converte para L/min
  mediaLm = mediaLm + vazao;    // Soma a vazão para a média em L/min
  i++;

  // Leitura do sensor de pressão
  long pressao = lerPressao();  // Lê a pressão do sensor
  float pressaoPSI = (pressao / 100000000.0) / 14.504; // Aplica a fórmula de conversão
  float pressaoPascal = pressaoPSI * 6894.76;
  // Envia dados formatados para o ESP32
  Serial.print("{\"vazao\":");
  Serial.print(vazao, 2); // Vazão com 2 casas decimais
  Serial.print(",\"pressao\":");
  Serial.print(pressaoPSI, 10); // Pressão com 4 casas decimais (PSI)
  Serial.println("}"); // Fecha o JSON e envia os dados

  mediaPSI = mediaPSI + pressaoPSI;

  // Exibe média a cada 60 segundos
  if (i == 60) {
    mediaLm = mediaLm / 60; // Calcula a média
    Serial.print("{\"mediaVazao\":");
    Serial.print(mediaLm, 4); // Média com 2 casas decimais
    Serial.println("}");    // Envia a média como JSON
    
    mediaPSI = mediaPSI / 60; // Calcula a média
    Serial.print("{\"mediaPSI\":");
    Serial.print(mediaPSI, 10); // Média com 2 casas decimais
    Serial.println("}");    // Envia a média como JSON

    mediaLm = 0; // Reseta média
    mediaPSI = 0; // Reseta média
    i = 0;     // Reseta contador
    Serial.println("\n\nInicio\n\n"); // Reinicia a contagem
  }
}

void incpulso() {
  contaPulso++; // Incrementa a contagem dos pulsos
}

// Função para leitura do sensor de pressão
long lerPressao() {
  long result = 0;

  // Aguarda leitura do sensor
  while (digitalRead(4)) {}

  for (int j = 0; j < 24; j++) {
    digitalWrite(3, HIGH); // Pulso do clock
    digitalWrite(3, LOW);
    result = result << 1;
    if (digitalRead(4)) {
      result++; // Lê os bits do sensor
    }
  }

  result = result ^ 0x800000; // Ajuste para complemento de 2

  // Pulsa o clock 3 vezes para iniciar nova leitura
  for (char j = 0; j < 3; j++) {
    digitalWrite(3, HIGH);
    digitalWrite(3, LOW);
  }

  return result; // Retorna o valor lido
}