#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUIDs personalizados para o serviço e característica BLE
#define UUID_SERVICO        "12345678-1234-1234-1234-123456789abc"
#define UUID_CARACTERISTICA "87654321-4321-4321-4321-abcdefabcdef"

// Pinos do driver de motor
#define IN1 2
#define IN2 4
#define ENA 5
#define IN3 18
#define IN4 19
#define ENB 21

BLECharacteristic *caracteristica;
bool dispositivoConectado = false;

// Funções de movimento
void frente() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(ENA, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); digitalWrite(ENB, HIGH);
}

void tras() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(ENA, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); digitalWrite(ENB, HIGH);
}

void girarDireita() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(ENA, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); digitalWrite(ENB, HIGH);
}

void girarEsquerda() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(ENA, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); digitalWrite(ENB, HIGH);
}

void parar() {
  digitalWrite(ENA, LOW); digitalWrite(ENB, LOW);
}

// Callback de conexão BLE
class MeusCallbacksDeServidor : public BLEServerCallbacks {
  void onConnect(BLEServer* servidor) {
    dispositivoConectado = true;
    Serial.println("Cliente conectado");
  }

  void onDisconnect(BLEServer* servidor) {
    dispositivoConectado = false;
    Serial.println("Cliente desconectado");
  }
};

// Callback de escrita BLE
class MeusCallbacksDeCaracteristica : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *caracteristica) {
    String valor = caracteristica->getValue();
    if (valor.length() > 0) {
      Serial.print("Comando recebido: ");
      Serial.println(valor);

      char comando = valor.charAt(0);
      switch (comando) {
        case 'F': frente(); break;
        case 'B': tras(); break;
        case 'L': girarEsquerda(); break;
        case 'R': girarDireita(); break;
        case 'S': parar(); break;
        default: parar(); break;
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Bluetooth...");

  // Configura os pinos dos motores
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  parar();

  // Inicializa BLE
  BLEDevice::init("victor");
  BLEServer *servidor = BLEDevice::createServer();
  servidor->setCallbacks(new MeusCallbacksDeServidor());

  BLEService *servico = servidor->createService(UUID_SERVICO);
  caracteristica = servico->createCharacteristic(
    UUID_CARACTERISTICA,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  caracteristica->setCallbacks(new MeusCallbacksDeCaracteristica());
  servico->start();

 BLEAdvertising *propaganda = servidor->getAdvertising();
  propaganda->addServiceUUID(UUID_SERVICO);
  propaganda->setScanResponse(true);
  propaganda->setMinPreferred(0x06);
  propaganda->setMinPreferred(0x12);
  propaganda->start();
  Serial.println("Esperando por cliente...");
}

void loop() {
  delay(1000); // BLE é controlado por eventos
}

