// RYLR998 LoRa Duplex Test for ESP32
// Written for IoT Bhai

#include <HardwareSerial.h>

// --- CONFIGURATION ---
// Change this for each board!
// Board A: localAddress = 1, destinationAddress = 2
// Board B: localAddress = 2, destinationAddress = 1
const int localAddress = 1;       
const int destinationAddress = 2; 

const int networkID = 5;          // Must be the same for both
const int BAND = 915000000;       // Frequency (868000000 or 915000000)

// --- PINS ---
#define RX_PIN 16 // Connect to RYLR998 TX
#define TX_PIN 17 // Connect to RYLR998 RX

// Use Hardware Serial 2
HardwareSerial LoRaSerial(2);

long lastSendTime = 0;
int interval = 2000; // Send message every 2 seconds

void setup() {
  // Start Serial Monitor (PC)
  Serial.begin(115200);
  while (!Serial);

  // Start LoRa Serial
  // RYLR998 default is usually 115200
  LoRaSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  
  delay(1000);
  Serial.println("--- IoT Bhai LoRa Test Starting ---");

  // Initialize Module
  setupLoRa();
}

void loop() {
  // 1. READ INCOMING DATA
  while (LoRaSerial.available()) {
    String incoming = LoRaSerial.readStringUntil('\n');
    if (incoming.length() > 0) {
      Serial.print("RECEIVED: ");
      Serial.println(incoming); // Print raw response to Serial Monitor
      
      // If it's a message, it looks like: +RCV=1,5,HELLO,-99,40
      if (incoming.indexOf("+RCV") != -1) {
         parseMessage(incoming);
      }
    }
  }

  // 2. SEND DATA (Only if we are Address 1 for this test)
  if (localAddress == 1) {
    if (millis() - lastSendTime > interval) {
      String message = "Hello from Tipu!";
      sendMessage(destinationAddress, message);
      lastSendTime = millis();
    }
  }
}

void setupLoRa() {
  // Test Connection
  sendAT("AT"); 
  
  // Set Factory Defaults (Optional, good for fresh start)
  // sendAT("AT+FACTORY"); 
  // delay(1000);

  // Set Address
  sendAT("AT+ADDRESS=" + String(localAddress));
  
  // Set Network ID
  sendAT("AT+NETWORKID=" + String(networkID));
  
  // Set Frequency (Optional)
  // sendAT("AT+BAND=" + String(BAND));
  
  // Set Parameter (Spreading Factor, Bandwidth, Coding Rate, Preamble)
  // Default is usually sufficient: 9,7,1,12
  // sendAT("AT+PARAMETER=9,7,1,12");
}

void sendMessage(int address, String msg) {
  // Syntax: AT+SEND=<Address>,<PayloadLength>,<Data>
  String cmd = "AT+SEND=" + String(address) + "," + String(msg.length()) + "," + msg;
  sendAT(cmd);
}

void sendAT(String cmd) {
  Serial.print("CMD SENT: ");
  Serial.println(cmd);
  LoRaSerial.print(cmd + "\r\n"); // RYLR998 requires CR+LF
  delay(100); // Give module time to process
}

void parseMessage(String response) {
  // Basic parsing to make it readable
  // Expected: +RCV=SenderID,Length,Data,RSSI,SNR
  
  int firstComma = response.indexOf(',');
  int secondComma = response.indexOf(',', firstComma + 1);
  int thirdComma = response.indexOf(',', secondComma + 1);
  
  if (thirdComma > 0) {
    String senderID = response.substring(5, firstComma);
    String data = response.substring(secondComma + 1, thirdComma);
    
    Serial.println("-----------------------------");
    Serial.println("Message Content: " + data);
    Serial.println("From ID: " + senderID);
    Serial.println("-----------------------------");
  }
}