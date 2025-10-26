#include <Arduino.h>

const int numSensores = 7;
const int pinosSensores[numSensores] = {2, 3, 4, 5, 6, 7, 8};
int estadoAnterior[numSensores];
unsigned long ultimoAcionamentoGeral = 0;
unsigned long ultimoAcionamentoSensor[numSensores];
bool primeiroEvento = true;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < numSensores; i++) {
    pinMode(pinosSensores[i], INPUT);
    estadoAnterior[i] = digitalRead(pinosSensores[i]);
    ultimoAcionamentoSensor[i] = 0;
  }
}

void loop() {
  for (int i = 0; i < numSensores; i++) {
    int estadoAtual = digitalRead(pinosSensores[i]);
    // Transição LOW -> HIGH (detecção pelo sensor PNP)
    if (estadoAnterior[i] == LOW && estadoAtual == HIGH) {
      unsigned long agora = millis();
      unsigned long intervaloGeral = primeiroEvento ? 0 : (agora - ultimoAcionamentoGeral);
      unsigned long intervaloSensor =
          ultimoAcionamentoSensor[i] == 0 ? 0 : (agora - ultimoAcionamentoSensor[i]);

      Serial.print("sensor");
      Serial.print(i + 1);
      Serial.print(";intervalo_entre_sensores=");
      Serial.print(intervaloGeral);
      Serial.print("ms;intervalo_mesmo_sensor=");
      Serial.print(intervaloSensor);
      Serial.println("ms");

      ultimoAcionamentoGeral = agora;
      ultimoAcionamentoSensor[i] = agora;
      primeiroEvento = false;
      delay(50); // Debounce
    }
    estadoAnterior[i] = estadoAtual;
  }
}

// const int numSensores = 7;
// int sensorAtual = 1;
// unsigned long ultimoEnvio = 0;
// const unsigned long intervalo = 1000; // 2 segundos em milissegundos

// void setup() {
//   Serial.begin(115200);
// }

// void loop() {
//   unsigned long agora = millis();

//   if (agora - ultimoEnvio >= intervalo) {
//     Serial.print("sensor");
//     Serial.println(sensorAtual);
//     sensorAtual++;
//     if (sensorAtual > numSensores) {
//       sensorAtual = 1;
//     }
//     ultimoEnvio = agora;
//   }
// }
