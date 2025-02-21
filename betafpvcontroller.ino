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


#define LX_high 7
#define LX_low 6
#define LY_high 5
#define LY_low 4
#define RX_high 1
#define RX_low 0
#define RY_high 3
#define RY_low 2



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
			  // sprintf(hexString, "%02X", (int)b); // Format with leading zeros
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

void setup() {
	for (uint8_t i=0; i < PACKET_SIZE; i++){
		dataBuffer[i] = 0;
	}
	// Serial.begin(115200);
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

  XInput.press(BUTTON_A);
	delay(100);

	XInput.release(BUTTON_A);
	delay(100);

  XInput.press(BUTTON_A);
	delay(100);

	XInput.release(BUTTON_A);
	delay(100);

  XInput.press(BUTTON_A);
	delay(100);

	XInput.release(BUTTON_A);
	delay(100);

}

void loop() {	
  if (digitalRead(SafetyPin) == LOW) {
		return;
	}

	// unsigned long t = millis();  // Get timestamp for comparison
	int LY = 0;
  int LX = 0;
	int RY = 0;
	int RX = 0;

	Usb.Task(); // Continuously process USB tasks
  // if (packetIndex == 0) {
  //   for (uint8_t i=0; i < PACKET_SIZE; i++){
  //     if (i == LY_high){
  //       LY = (int)dataBuffer[i] * 0x1000;
  //     }
      
  //     // Serial.print(dataBuffer[i], HEX);
  //     // char hexString[4];
  //     // sprintf(hexString, "%02X", (int)dataBuffer[PACKET_SIZE-1-i]); // Format with leading zeros
  //     // Serial.print(hexString);
  //   }
  //   // delay(50);
  //   // Serial.println();
  // } 

	// controller gives values from 0 to 0x7FF, and 0x3FF as zero position. 
	// XInput takes data from -32768 to 32767 signed. or 0x7FFF as max value and 0x8000 (signed) as minimum.   

	
  LY = (((int)(dataBuffer[LY_high]) << 13) | ((int)dataBuffer[LY_low]) << 5) - 0x7FF0;
	LX = (((int)(dataBuffer[LX_high]) << 13) | ((int)dataBuffer[LX_low]) << 5) - 0x7FF0;
	RY = (((int)(dataBuffer[RY_high]) << 13) | ((int)dataBuffer[RY_low]) << 5) - 0x7FF0;
	RX = (((int)(dataBuffer[RX_high]) << 13) | ((int)dataBuffer[RX_low]) << 5) - 0x7FF0;



	XInput.setJoystick(JOY_LEFT, LX, LY); 
	XInput.setJoystick(JOY_RIGHT, RX, RY); 

	// Send values to PC
	XInput.send();
}
