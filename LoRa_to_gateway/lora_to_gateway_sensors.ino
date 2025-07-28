//UART pins for RAK3172
#define RAK_TX_PIN 17  // ESP32 TX pin connected to RAK3172 RX
#define RAK_RX_PIN 16  // ESP32 RX pin connected to RAK3172 TX

// Create a HardwareSerial object for RAK3172
HardwareSerial RAK3172(2); //creates a serial port object on UART2
void configureP2PReceiver() {
  delay(3000);
  Serial.println("--------------------------------------------");
  sendATCommand("AT", 4000);
  sendATCommand("AT+VER=?", 2000);  // Get device information
  sendATCommand("AT+DEVEUI=?", 2000);  // Get device EUI
  sendATCommand("AT+NWM=0", 1000);
  delay(1500);
  sendATCommand("AT+P2P=868250000:7:125:0:10:14", 1000);
  delay(500);
  sendATCommand("AT+PRECV=0", 1000);
  delay(500);
  sendATCommand("AT+PRECV=65534", 1000);
  delay(800); // Wait before next try
  // Maybe set other parameters
}


void configureLoRaWAN() {
  Serial.println("Testing RAK3172 connection to the LORAWAN");
  sendATCommand("AT", 4000);
  sendATCommand("AT+VER=?", 2000);  // Get device information
  sendATCommand("AT+DEVEUI=?", 2000);  // Get device EUI

  sendATCommand("AT+NWM=1", 1000);              // LoRaWAN mode NOT THE P2P
  sendATCommand("AT+NJM=0", 1000);              // ACTİVATİON TO ABP
  sendATCommand("AT+CLASS=A", 1000);            // CLASS A B C 
  sendATCommand("AT+BAND=4", 1000);             //  4 is EU
  sendATCommand("AT+DEVADDR=006f0ad6", 1000);   // CLASS A B C 
  sendATCommand("AT+APPSKEY=8c378871993b6556d3139d1c9e21e3f8", 1000);        //  Network session key
  sendATCommand("AT+NWKSKEY=8c378871993b6556d3139d1c9e21e3f8", 1000);        //  Network session key
  sendATCommand("AT+JOIN=1:0:10:8", 1000);    // Some firmware requires a join to start uplinks

  Serial.println("LORA Configuration Completed");
}


void sendLoRaWAN(const String& hexPayload) {
  // Send the payload as HEX 
  String cmd = "AT+SEND=2:" + hexPayload;
  Serial.print("Forwarding to LoRaWAN: "); Serial.println(cmd);
  sendATCommand(cmd, 5000);
  delay(1000);
}



void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 to RAK3172 AT Command Interface");
  

  RAK3172.begin(9600, SERIAL_8N1, RAK_RX_PIN, RAK_TX_PIN);// Initialize RAK3172 serial communication
  //SERIAL_8N1 means: 8 data bits, No parity, 1 stop bit (standard UART 
  sendATCommand("ATZ", 2000);
  delay(2000);

}

const int MAX_DEVICES = 10;
String deviceIDs[MAX_DEVICES];
String deviceMessages[MAX_DEVICES];
int deviceCount = 0;
unsigned long listenStartTime = 0;
const unsigned long LISTEN_DURATION = 30000;
bool isListening = false;

// Add this new function
int findOrAddDevice(String deviceID) {
    for (int i = 0; i < deviceCount; i++) {
        if (deviceIDs[i] == deviceID) {
            return i;
        }
    }
    if (deviceCount < MAX_DEVICES) {
        deviceIDs[deviceCount] = deviceID;
        deviceCount++;
        return deviceCount - 1;
    }
    return -1;
}

void handleRakLine(const String &ln) {
    // Only process if it matches "+EVT:HEXDATA" (HEX only, no spaces, no text)
    if (ln.startsWith("+EVT:")) {
        String hexPayload = ln.substring(5);

        // Only process if the string is a valid hex (all chars 0-9, a-f, A-F, even length)
        bool validHex = true;
        if (hexPayload.length() % 2 == 0 && hexPayload.length() > 0) {
            for (int i = 0; i < hexPayload.length(); ++i) {
                char c = hexPayload.charAt(i);
                if (!isxdigit(c)) {
                    validHex = false;
                    break;
                }
            }
        } else {
            validHex = false;
        }

        if (validHex) {
            // Normal decode and store
            String asciiPayload = "";
            for (int i = 0; i < hexPayload.length(); i += 2) {
                String byteString = hexPayload.substring(i, i + 2);
                char c = (char) strtol(byteString.c_str(), NULL, 16);
                asciiPayload += c;
            }
            Serial.print("[P2P Received] ASCII: ");
            Serial.println(asciiPayload);

            // Extract device ID from ASCII (before first $)
            int dollarIndex = asciiPayload.indexOf('$');
            String deviceID = "";
            if (dollarIndex > 0) {
                deviceID = asciiPayload.substring(0, dollarIndex);
            } else {
                deviceID = "UNKNOWN";
            }

            int index = findOrAddDevice(deviceID);
            if (index >= 0) {
                deviceMessages[index] = hexPayload;
                Serial.print("Device ");
                Serial.print(deviceID);
                Serial.print(" message stored. Total devices: ");
                Serial.println(deviceCount);
            }
        } else {
            // Ignore non-hex, just print as info
            Serial.print("[P2P Event] Not a hex payload, skipping: ");
            Serial.println(ln);
        }
    }
}


String buildAggregatePayload() {
    String agg = "";
    for (int i = 0; i < deviceCount; i++) {
        agg += deviceMessages[i];  // <-- No separator!
    }
    return agg;
}


bool rakReadLine(String &outLine) {
    static String buf;
    while (RAK3172.available()) {
        char c = RAK3172.read();
        if (c == '\r') continue;
        if (c == '\n') {
            outLine = buf;
            buf = "";
            if (outLine.length()) {
                Serial.print("[RAK->line] "); Serial.println(outLine);
                return true;
            }
        } else {
            buf += c;
        }
    }
    return false;
}

void loop() {
    String ln;

    // Start listening cycle if not already
    if (!isListening) {
        Serial.println("Configuring P2P and starting listening window...");
        configureP2PReceiver();             // Configure P2P ONLY here
        listenStartTime = millis();
        isListening = true;
        // Optionally clear device buffers here
        deviceCount = 0;
        for (int i = 0; i < MAX_DEVICES; i++) {
            deviceIDs[i] = "";
            deviceMessages[i] = "";
        }
    }

    // While listening, process messages
    if (isListening) {
        while (rakReadLine(ln)) {
            handleRakLine(ln);
        }
        //Serial.println("[DEBUG] Exited rakReadLine() while loop.");

        // Show time left every 5s (optional)
        static unsigned long lastShow = 0;
        if (millis() - lastShow > 5000) {
            lastShow = millis();
            unsigned long left = LISTEN_DURATION - (millis() - listenStartTime);
            Serial.print("Listening... ");
            Serial.print(left / 1000);
            Serial.println(" sec left");
        }

        // End listening after 30 seconds
        if (millis() - listenStartTime >= LISTEN_DURATION) {
          Serial.println("[DEBUG] Listen window duration reached!");
            isListening = false;
            Serial.println("Listen window ended.");

            // Stop P2P receive right here
            sendATCommand("AT+PRECV=0", 500);

            if (deviceCount > 0) {
                configureLoRaWAN();
                delay(2000);
                String aggPayload = buildAggregatePayload();
                sendLoRaWAN(aggPayload);
                delay(3000);
            } else {
                Serial.println("No devices heard; skipping send.");
            }
            // At next loop, !isListening triggers new P2P cycle again
        }
    }

    // Manual AT command handler
    if (Serial.available()) {
        String command = Serial.readString();
        command.trim();
        Serial.println("Sending command: " + command);
        sendATCommand(command, 3000);
    }
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