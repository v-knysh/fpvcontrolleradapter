#include <Usb.h>
#include <usbhub.h>
#include <hiduniversal.h>

// Create USB and hub objects
USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);


#define PACKET_SIZE 16
uint8_t dataBuffer[PACKET_SIZE];  // Buffer for the complete packet
uint8_t packetIndex = 0;           // Index to track the number of bytes received


// Custom class to parse HID data
class CustomHIDParser : public HIDReportParser {
public:
    // Override the Parse method to handle incoming HID data
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) override {
        // Serial.print("Data received: ");
        // Serial.print(len);
        for (uint8_t i = 0; i < len; i++) {
          // Serial.print(buf[i], HEX);
          // Serial.print(" ");
          uint8_t b = buf[i];
          char hexString[4];
          sprintf(hexString, "%02X", (int)b); // Format with leading zeros
          // Serial.print(hexString);


          dataBuffer[packetIndex+i] = buf[i];
        }
        packetIndex += len;
        if (packetIndex >= PACKET_SIZE) {
          packetIndex = 0;
        }
        // Serial.println();
    }
};

// Create an instance of the custom parser
CustomHIDParser myParser;

void setup() {
    for (uint8_t i=0; i < PACKET_SIZE; i++){
      dataBuffer[i] = 0;
    }
    Serial.begin(115200);
    if (Usb.Init() == -1) {
        Serial.println("USB Host Shield Initialization failed");
        while (1); // Halt if initialization fails
    }
    Serial.println("USB Host Shield Initialized");

    // Set the custom parser for HIDUniversal
    Hid.SetReportParser(0, &myParser);
}

void loop() {
    Usb.Task(); // Continuously process USB tasks
    if (packetIndex == 0) {
      for (uint8_t i=0; i < PACKET_SIZE; i++){
        // Serial.print(dataBuffer[i], HEX);
        char hexString[4];
        sprintf(hexString, "%02X", (int)dataBuffer[PACKET_SIZE-1-i]); // Format with leading zeros
        Serial.print(hexString);
      }
      delay(50);
      Serial.println();
    } 
}
