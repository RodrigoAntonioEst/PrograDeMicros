#include <WiFi.h>
#include <esp_now.h>

// ==========================
// CONFIGURACION
// ==========================
#define BAUD_DEBUG   115200
#define BAUD_NANO    9600

// Nano 1
#define RX_NANO1     4
#define TX_NANO1     -1

// Nano 2
#define RX_NANO2     16
#define TX_NANO2     -1

// MAC DEL ESP2
uint8_t macESP2[] = {0x80, 0xF3, 0xDA, 0x63, 0xA4, 0x18};

// Cada cuanto reenviar por ESP-NOW
#define TX_INTERVAL_MS  6

// ==========================
// PAQUETE CRUDO 2 BYTES
// ==========================
typedef struct {
  char control;
  char comando;
} PaqueteCrudo;

PaqueteCrudo paqueteTx;

// ==========================
// ESTADO POR CONTROL
// ==========================
typedef struct {
  char dirActual;          // U D L R o N

  bool aActiva;
  bool bActiva;

  bool aPendienteOn;       // enviar A una vez
  bool bPendienteOn;       // enviar B una vez

  bool nPendiente;         // enviar N una vez
  bool aPendienteOff;      // enviar a una vez
  bool bPendienteOff;      // enviar b una vez
} EstadoControl;

EstadoControl mando1 = {'N', false, false, false, false, false, false, false};
EstadoControl mando2 = {'N', false, false, false, false, false, false, false};

// ==========================
// PARSER POR UART
// ==========================
typedef struct {
  bool esperandoComando;
  char controlTemp;
} ParserUART;

ParserUART parser1 = {false, 0};
ParserUART parser2 = {false, 0};

// ==========================
// CONTROL DE ENVIO
// ==========================
volatile bool txBusy = false;
unsigned long ultimoTxMs = 0;
uint8_t turnoEnvio = 0;

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

bool esIgnorable(char c) {
  return (c == '\n' || c == '\r' || c == ' ' || c == '\t');
}

// ==========================
// CALLBACK ESP-NOW
// ==========================
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  txBusy = false;

  Serial.print("ESP-NOW envio: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("OK");
  } else {
    Serial.println("FALLO");
  }
}

// ==========================
// ENVIO CRUDO
// ==========================
bool enviarTrama(char control, char comando) {
  if (txBusy) {
    return false;
  }

  paqueteTx.control = control;
  paqueteTx.comando = comando;

  txBusy = true;
  esp_err_t resultado = esp_now_send(macESP2, (uint8_t *)&paqueteTx, sizeof(paqueteTx));

  Serial.print("ENVIADO -> ");
  Serial.print(control);
  Serial.println(comando);

  if (resultado == ESP_OK) {
    return true;
  } else {
    Serial.print("Error esp_now_send: ");
    Serial.println(resultado);
    txBusy = false;
    return false;
  }
}

// ==========================
// ACTUALIZAR ESTADO
// ==========================
void actualizarEstado(char control, char comando) {
  EstadoControl *m = (control == '1') ? &mando1 : &mando2;

  switch (comando) {
    // Direccion
    case 'U':
    case 'D':
    case 'L':
    case 'R':
      m->dirActual = comando;
      break;

    case 'N':
      if (m->dirActual != 'N') {
        m->dirActual = 'N';
        m->nPendiente = true;
      }
      break;

    // A
    case 'A':
      if (!m->aActiva) {
        m->aActiva = true;
        m->aPendienteOn = true;
      }
      break;

    case 'a':
      if (m->aActiva) {
        m->aActiva = false;
        m->aPendienteOff = true;
      }
      break;

    // B
    case 'B':
      if (!m->bActiva) {
        m->bActiva = true;
        m->bPendienteOn = true;
      }
      break;

    case 'b':
      if (m->bActiva) {
        m->bActiva = false;
        m->bPendienteOff = true;
      }
      break;
  }

  Serial.print("Estado ");
  Serial.print(control);
  Serial.print(" -> dir=");
  Serial.print(m->dirActual);
  Serial.print(" | A=");
  Serial.print(m->aActiva ? "ON" : "OFF");
  Serial.print(" | B=");
  Serial.print(m->bActiva ? "ON" : "OFF");
  Serial.print(" | nPend=");
  Serial.print(m->nPendiente ? "SI" : "NO");
  Serial.print(" | aOn=");
  Serial.print(m->aPendienteOn ? "SI" : "NO");
  Serial.print(" | aOff=");
  Serial.print(m->aPendienteOff ? "SI" : "NO");
  Serial.print(" | bOn=");
  Serial.print(m->bPendienteOn ? "SI" : "NO");
  Serial.print(" | bOff=");
  Serial.println(m->bPendienteOff ? "SI" : "NO");
}

// ==========================
// PROCESAR BYTE UART
// Espera pares tipo 1U, 1N, 1A, 1a, 1B, 1b...
// ==========================
void procesarByteUART(uint8_t uartOrigen, char c) {
  ParserUART *p = (uartOrigen == 1) ? &parser1 : &parser2;

  if (esIgnorable(c)) {
    return;
  }

  if (!p->esperandoComando) {
    if (controlValido(c)) {
      p->controlTemp = c;
      p->esperandoComando = true;
    }
    return;
  }

  if (comandoValido(c)) {
    actualizarEstado(p->controlTemp, c);
    p->esperandoComando = false;
    p->controlTemp = 0;
    return;
  }

  if (controlValido(c)) {
    p->controlTemp = c;
    p->esperandoComando = true;
    return;
  }

  p->esperandoComando = false;
  p->controlTemp = 0;
}

// ==========================
// LEER UART
// ==========================
void leerUART(HardwareSerial &puerto, uint8_t uartOrigen) {
  while (puerto.available() > 0) {
    char c = (char)puerto.read();

    Serial.print("UART");
    Serial.print(uartOrigen);
    Serial.print(" -> ");
    Serial.println(c);

    procesarByteUART(uartOrigen, c);
  }
}

// ==========================
// LISTA DE PRIORIDAD
// 0 = 1-N
// 1 = 1-a
// 2 = 1-b
// 3 = 2-N
// 4 = 2-a
// 5 = 2-b
// 6 = 1-dir
// 7 = 1-A
// 8 = 1-B
// 9 = 2-dir
// 10 = 2-A
// 11 = 2-B
// ==========================
int construirActivos(uint8_t activos[]) {
  int n = 0;

  // PRIORIDAD MAXIMA: soltados
  if (mando1.nPendiente)   activos[n++] = 0;
  if (mando1.aPendienteOff) activos[n++] = 1;
  if (mando1.bPendienteOff) activos[n++] = 2;

  if (mando2.nPendiente)   activos[n++] = 3;
  if (mando2.aPendienteOff) activos[n++] = 4;
  if (mando2.bPendienteOff) activos[n++] = 5;

  // Luego movimientos y presionados
  if (mando1.dirActual != 'N') activos[n++] = 6;
  if (mando1.aPendienteOn)     activos[n++] = 7;
  if (mando1.bPendienteOn)     activos[n++] = 8;

  if (mando2.dirActual != 'N') activos[n++] = 9;
  if (mando2.aPendienteOn)     activos[n++] = 10;
  if (mando2.bPendienteOn)     activos[n++] = 11;

  return n;
}

// ==========================
// TRANSMISION
// - N, a, b con prioridad
// - URLD continuas
// - A y B una vez
// ==========================
void transmitirActivos() {
  if (txBusy) {
    return;
  }

  unsigned long ahora = millis();
  if (ahora - ultimoTxMs < TX_INTERVAL_MS) {
    return;
  }

  uint8_t activos[12];
  int total = construirActivos(activos);

  if (total == 0) {
    return;
  }

  if (turnoEnvio >= total) {
    turnoEnvio = 0;
  }

  uint8_t item = activos[turnoEnvio];

  switch (item) {
    // PRIORIDAD: soltados mando 1
    case 0:
      if (enviarTrama('1', 'N')) {
        mando1.nPendiente = false;
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;

    case 1:
      if (enviarTrama('1', 'a')) {
        mando1.aPendienteOff = false;
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;

    case 2:
      if (enviarTrama('1', 'b')) {
        mando1.bPendienteOff = false;
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;

    // PRIORIDAD: soltados mando 2
    case 3:
      if (enviarTrama('2', 'N')) {
        mando2.nPendiente = false;
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;

    case 4:
      if (enviarTrama('2', 'a')) {
        mando2.aPendienteOff = false;
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;

    case 5:
      if (enviarTrama('2', 'b')) {
        mando2.bPendienteOff = false;
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;

    // Direccion continua mando 1
    case 6:
      if (enviarTrama('1', mando1.dirActual)) {
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;

    // A una vez mando 1
    case 7:
      if (enviarTrama('1', 'A')) {
        mando1.aPendienteOn = false;
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;

    // B una vez mando 1
    case 8:
      if (enviarTrama('1', 'B')) {
        mando1.bPendienteOn = false;
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;

    // Direccion continua mando 2
    case 9:
      if (enviarTrama('2', mando2.dirActual)) {
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;

    // A una vez mando 2
    case 10:
      if (enviarTrama('2', 'A')) {
        mando2.aPendienteOn = false;
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;

    // B una vez mando 2
    case 11:
      if (enviarTrama('2', 'B')) {
        mando2.bPendienteOn = false;
        ultimoTxMs = ahora;
        turnoEnvio++;
      }
      break;
  }

  if (turnoEnvio >= total) {
    turnoEnvio = 0;
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

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, macESP2, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error agregando peer ESP2");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("ESP-NOW listo");
}

// ==========================
// SETUP
// ==========================
void setup() {
  Serial.begin(BAUD_DEBUG);
  delay(1000);

  Serial.println();
  Serial.println("=== ESP1 INICIANDO ===");

  Serial1.begin(BAUD_NANO, SERIAL_8N1, RX_NANO1, TX_NANO1);
  Serial2.begin(BAUD_NANO, SERIAL_8N1, RX_NANO2, TX_NANO2);

  Serial.print("Nano1 en RX GPIO ");
  Serial.println(RX_NANO1);

  Serial.print("Nano2 en RX GPIO ");
  Serial.println(RX_NANO2);

  configurarESPNow();

  Serial.print("MAC ESP1: ");
  Serial.println(WiFi.macAddress());

  Serial.println("Esperando comandos...");
}

// ==========================
// LOOP
// ==========================
void loop() {
  leerUART(Serial1, 1);
  leerUART(Serial2, 2);
  transmitirActivos();
}