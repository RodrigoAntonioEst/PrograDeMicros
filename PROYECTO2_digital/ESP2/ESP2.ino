#include <WiFi.h>
#include <esp_now.h>

// ==========================
// CONFIGURACION
// ==========================
#define BAUD_DEBUG 115200

HardwareSerial STM32_UART(2);   // RX=16, TX=17

// ==========================
// PAQUETE CRUDO
// DEBE SER IGUAL AL DEL ESP1
// ==========================
typedef struct {
  char control;   // '1' o '2'
  char comando;   // 'U', 'D', 'L', 'R', 'N', 'A', 'a', 'B', 'b'
} PaqueteCrudo;

PaqueteCrudo paqueteRecibido;

// ==========================
// VALIDACION
// ==========================
bool controlValido(char c) {
  return (c == '1' || c == '2');
}

bool comandoValido(char c) {
  switch (c) {
    case 'U':
    case 'D':
    case 'L':
    case 'R':
    case 'N':
    case 'A':
    case 'a':
    case 'B':
    case 'b':
      return true;
    default:
      return false;
  }
}

// Solo estos se mandan al STM32 por G17
bool comandoReenviableSTM32(char c) {
  switch (c) {
    case 'U':
    case 'D':
    case 'L':
    case 'R':
    case 'A':
    case 'B':
      return true;
    default:
      return false;
  }
}

bool esComandoStop(char c) {
  return (c == 'N' || c == 'a' || c == 'b');
}

// ==========================
// DEBUG
// ==========================
void imprimirMAC(const uint8_t *mac) {
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
}

// ==========================
// CALLBACK DE RECEPCION
// compatible con ESP32 core viejo
// ==========================
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (len != sizeof(PaqueteCrudo)) {
    return;
  }

  memcpy(&paqueteRecibido, incomingData, sizeof(paqueteRecibido));

  if (!controlValido(paqueteRecibido.control) || !comandoValido(paqueteRecibido.comando)) {
    return;
  }

  // DEBUG: mostrar todo lo que llega
  Serial.print("RX -> ");
  Serial.print(paqueteRecibido.control);
  Serial.println(paqueteRecibido.comando);

  // SOLO reenviar al STM32 los comandos utiles
  if (comandoReenviableSTM32(paqueteRecibido.comando)) {
    STM32_UART.write(paqueteRecibido.control);
    STM32_UART.write(paqueteRecibido.comando);

    Serial.print("TX G17 -> ");
    Serial.print(paqueteRecibido.control);
    Serial.println(paqueteRecibido.comando);
  }
  else if (esComandoStop(paqueteRecibido.comando)) {
    Serial.print("NO UART (stop interno) -> ");
    Serial.print(paqueteRecibido.control);
    Serial.println(paqueteRecibido.comando);
  }
}

// ==========================
// CONFIGURAR ESP-NOW
// ==========================
void configurarESPNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    while (true) {
      delay(1000);
    }
  }

  esp_now_register_recv_cb(onDataRecv);

  Serial.println("ESP-NOW receptor listo");
}

// ==========================
// SETUP
// ==========================
void setup() {
  Serial.begin(BAUD_DEBUG);

  // TX = GPIO17
  STM32_UART.begin(115200, SERIAL_8N1, 16, 17);

  delay(1000);

  Serial.println();
  Serial.println("=== ESP2 INICIANDO ===");

  configurarESPNow();

  Serial.print("MAC ESP2: ");
  Serial.println(WiFi.macAddress());

  Serial.println("Esperando paquetes...");
}

// ==========================
// LOOP
// ==========================
void loop() {
}