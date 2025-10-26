#include <Arduino.h>

const int numSensores = 7;
const int pinosSensores[numSensores] = {2, 3, 4, 5, 6, 7, 8};
int estadoAnterior[numSensores];

enum DirecaoSequencia {
  DIRECAO_NENHUMA,
  DIRECAO_ASCENDENTE,
  DIRECAO_DESCENDENTE
};

DirecaoSequencia direcaoAtual = DIRECAO_NENHUMA;
int proximoSensorEsperado = 0;
int sensoresAcionados = 0;

void reiniciarSequencia() {
  direcaoAtual = DIRECAO_NENHUMA;
  proximoSensorEsperado = 0;
  sensoresAcionados = 0;
}

void iniciarSequencia(int sensor) {
  sensoresAcionados = 1;
  if (sensor == 1) {
    direcaoAtual = DIRECAO_ASCENDENTE;
    proximoSensorEsperado = sensor + 1;
  } else if (sensor == numSensores) {
    direcaoAtual = DIRECAO_DESCENDENTE;
    proximoSensorEsperado = sensor - 1;
  } else {
    direcaoAtual = DIRECAO_NENHUMA;
    sensoresAcionados = 0;
  }
}

void processarAcionamento(int sensor) {
  if (direcaoAtual == DIRECAO_NENHUMA) {
    if (sensor == 1 || sensor == numSensores) {
      iniciarSequencia(sensor);
    }
    return;
  }

  if ((direcaoAtual == DIRECAO_ASCENDENTE && sensor == proximoSensorEsperado) ||
      (direcaoAtual == DIRECAO_DESCENDENTE && sensor == proximoSensorEsperado)) {
    sensoresAcionados++;

    if (direcaoAtual == DIRECAO_ASCENDENTE) {
      proximoSensorEsperado++;
    } else {
      proximoSensorEsperado--;
    }

    if (sensoresAcionados == numSensores) {
      if (direcaoAtual == DIRECAO_ASCENDENTE) {
        Serial.println("sentido horario");
      } else {
        Serial.println("sentido anti-horario");
      }
      reiniciarSequencia();
    }
    return;
  }

  // Sequência quebrada; verificar se podemos iniciar uma nova
  reiniciarSequencia();
  if (sensor == 1 || sensor == numSensores) {
    iniciarSequencia(sensor);
  }
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < numSensores; i++) {
    pinMode(pinosSensores[i], INPUT);
    estadoAnterior[i] = digitalRead(pinosSensores[i]);
  }
}

void loop() {
  for (int i = 0; i < numSensores; i++) {
    int estadoAtual = digitalRead(pinosSensores[i]);
    // Transição LOW -> HIGH (detecção pelo sensor PNP)
    if (estadoAnterior[i] == LOW && estadoAtual == HIGH) {
      const int sensorAtual = i + 1;
      Serial.print("sensor");
      Serial.println(sensorAtual);
      processarAcionamento(sensorAtual);
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