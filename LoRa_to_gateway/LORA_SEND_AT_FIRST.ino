/*
 * ESP32 to RAK3172 AT Command Communication
 * This code demonstrates how to send AT commands from ESP32 to RAK3172
 */

//UART pins for RAK3172
#define RAK_TX_PIN 17  // ESP32 pin connected to RAK3172 RX
#define RAK_RX_PIN 16  // ESP32 pin connected to RAK3172 TX

// Create a HardwareSerial object for RAK3172
HardwareSerial RAK3172(2); //creates a serial port object on UART2

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 to RAK3172 AT Command Interface");
  
  
  RAK3172.begin(9600, SERIAL_8N1, RAK_RX_PIN, RAK_TX_PIN);// Initialize RAK3172 serial communication
  //SERIAL_8N1 means: 8 data bits, No parity, 1 stop bit (standard UART 
  
  delay(1000);
  
  // Test basic communication
  Serial.println("Testing RAK3172 connection...");
  sendATCommand("AT", 2000);
  sendATCommand("AT+VER=?", 2000);  // Get device information
  sendATCommand("AT+DEVEUI=?", 2000);  // Get device EUI
}


void loop() {
  // Check for incoming data from RAK3172
  if (RAK3172.available()) {
    String response = RAK3172.readString();
    Serial.print("RAK3172 Response: ");
    Serial.println(response);
  }
  
  // Check for commands from Serial Monitor
  if (Serial.available()) {
    String command = Serial.readString();
    command.trim(); // Remove whitespace
    
    Serial.println("Sending command: " + command);
    sendATCommand(command, 3000);
  }
  
  // Example: Send periodic commands
  static unsigned long lastCommand = 0;
  if (millis() - lastCommand > 30000) { // Every 30 seconds
    sendATCommand("AT+RSSI=?", 2000); // Get signal strength
    lastCommand = millis();
  }
  //set_timeout cevabın gelmesi gerekn süre
  delay(100);
}

// Function to send AT command and wait for response
void sendATCommand(String command, int timeout) {
  Serial.println("Sending: " + command);
  
  // Clear any existing data
  while (RAK3172.available()) {
    RAK3172.read();
  }
  
  // Send the command
  RAK3172.println(command);
  
  // Wait for response
  unsigned long startTime = millis();
  String response = "";
  
  while (millis() - startTime < timeout) {
    if (RAK3172.available()) {
      response += RAK3172.readString();
      break;
    }
    delay(10);
  }
  
  if (response.length() > 0) {
    Serial.println("Response: " + response);
  } else {
    Serial.println("No response received");
  }
  
  Serial.println("---");
}

// Function to configure RAK3172 for LoRaWAN
void configureLoRaWAN() {
  Serial.println("Configuring RAK3172 for LoRaWAN...");
  
  // Set to LoRaWAN mode
  sendATCommand("AT+NWM=1", 2000);
  
  // Set region (adjust as needed)
  sendATCommand("AT+BAND=4", 2000); // EU868
  
  // Set device class
  sendATCommand("AT+CLASS=A", 2000);
  
  // Set join mode to OTAA
  sendATCommand("AT+NJM=1", 2000);
  
  // You would set your keys here:
  // sendATCommand("AT+APPEUI=your_app_eui", 2000);
  // sendATCommand("AT+APPKEY=your_app_key", 2000);
  
  Serial.println("Configuration complete!");
}

// Function to send LoRaWAN data
void sendLoRaWANData(String data) {
  String command = "AT+SEND=1:" + data; // Port 1
  sendATCommand(command, 5000);
}

// Common RAK3172 AT Commands you can use:
/*
Basic Commands:
- AT - Test command
- AT+VER=? - Get firmware version
- ATZ - Reset module
- AT+DEVEUI=? - Get device EUI

LoRaWAN Commands:
- AT+NWM=1 - Set LoRaWAN mode
- AT+BAND=? - Get/Set frequency band
- AT+CLASS=? - Get/Set device class
- AT+JOIN=1:0:10:8 - Join network
- AT+SEND=port:data - Send data

LoRa P2P Commands:
- AT+NWM=0 - Set LoRa P2P mode
- AT+PFREQ=? - Get/Set frequency
- AT+PSF=? - Get/Set spreading factor
- AT+PSEND=data - Send P2P data
*/