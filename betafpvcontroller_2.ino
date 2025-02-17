#include <XInput.h>
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

// Config Settings
const int SafetyPin = 0;  // Ground this pin to prevent inputs


#define LX_high = 0
#define LX_low = 1
#define LY_high = 2
#define LY_low = 3
#define RX_high = 4
#define RX_low = 5
#define RY_high = 6
#define RY_low = 7



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

// Joystick Setup
const int JoyMax = 32767;  // int16_t max
const double angle_precision = (2 * PI) / (CycleTime / 4);  // 4 because 250 Hz update rate
double angle = 0.0;

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
	
	pinMode(SafetyPin, INPUT_PULLUP);
	XInput.setAutoSend(false);  // Wait for all controls before sending

	XInput.begin();
}

void loop() {
	if (digitalRead(SafetyPin) == LOW) {
		return;
	}

	unsigned long t = millis();  // Get timestamp for comparison

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

	// controller gives values from 0 to 0x7FFF, and 0x3FFF as zero position. 
	// XInput takes data from -32768 to 32767 signed. or 0x7FFF as max value and 0x8000 (signed) as minimum.   


	int LX = (((int)(dataBuffer[LX_high] << 8) | (int)dataBuffer[LX_low])) << 1 - 0x7FFE;
	int LY = (((int)(dataBuffer[LY_high] << 8) | (int)dataBuffer[LY_low])) << 1 - 0x7FFE;
	int RX = (((int)(dataBuffer[RX_high] << 8) | (int)dataBuffer[RX_low])) << 1 - 0x7FFE;
	int RY = (((int)(dataBuffer[RY_high] << 8) | (int)dataBuffer[RX_low])) << 1 - 0x7FFE;


	// Calculate joystick x/y values using trig
	int axis_x = sin(angle) * JoyMax;
	int axis_y = cos(angle) * JoyMax;

	XInput.setJoystick(JOY_LEFT, LX, LY);  // Clockwise
	XInput.setJoystick(JOY_RIGHT, RX, RY);  // Counter-clockwise

	// Send values to PC
	XInput.send();
}
