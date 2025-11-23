// Include the standard SPI communication library for the nRF24L01 module.
#include <SPI.h>
// Include the library for the PS2 controller (PlayStation 2) communication.
#include <Psx.h>  
// Include the main library for the nRF24L01 module's register map.
#include <nRF24L01.h>
// Include the RF24 wrapper library for easier control of the nRF24L01 module.
#include <RF24.h>

// --- PS2 Controller Pin Definitions (Wired to the Arduino) ---
// Define the digital pins connected to the PS2 controller adapter.
#define dataPin 4  // Controller Data pin (PS2 D)
#define cmndPin 3  // Controller Command pin (PS2 C)
#define attPin 2   // Controller Attention pin (PS2 A/Select)
#define clockPin 5 // Controller Clock pin (PS2 K)

// Initialize the Psx controller object.
Psx Psx;

// Define the unique 6-byte address (pipe) for communication.
// This must match the address used by the receiver code.
const uint64_t my_radio_pipe = 0xE8E8F0F0E1LL;

// Initialize the nRF24L01 radio object.
// The constructor takes the CE (Chip Enable) and CSN (Chip Select Not) pin numbers.
// CE is connected to pin 9, CSN is connected to pin 10.
RF24 radio(9, 10);

// Structure to define the data packet that will be sent wirelessly.
// These 8 bytes are used to transmit the controller's state (buttons, joysticks, etc.).
struct Data_to_be_sent {
  byte ch1; // PSx ID (Controller Type)
  byte ch2; // PSx Status (Error/Mode)
  byte ch3; // PSx Thumb Left (Button states for D-pad/Left Stick click/L1/L2/etc.)
  byte ch4; // PSx Thumb Right (Button states for R1/R2/Square/Triangle/etc.)        
  byte ch5; // PSx Right Joystick X-axis (0-255)
  byte ch6; // PSx Right Joystick Y-axis (0-255)
  byte ch7; // PSx Left Joystick X-axis (0-255)
  byte ch8; // PSx Left Joystick Y-axis (0-255)
};
Data_to_be_sent sent_data; // Global variable to hold the data ready for transmission.

void setup()
{
  // Initialize Serial communication for debugging and viewing data.
  Serial.begin(115200); 

  // Initialize the PS2 controller communication pins and set a read delay (10 µs).
  Psx.setupPins(dataPin, cmndPin, attPin, clockPin, 10);
  
  // Initialize the nRF24L01 radio module.
  radio.begin();
  // Disable automatic acknowledgments (allows faster, one-way communication).
  radio.setAutoAck(false);
  // Set the data rate to 250KBPS for maximum range and reliability.
  radio.setDataRate(RF24_250KBPS);
  // Open the transmission pipe using the defined address.
  radio.openWritingPipe(my_radio_pipe);
  // Set the radio to transmitter mode.
  radio.stopListening();
}


void loop() {
  
    // 1. Read the current state from the PS2 controller.
    Psx.read();  
    
    // 2. Map the controller data bytes to the data structure for transmission.
    sent_data.ch1 = Psx.psxID;
    sent_data.ch2 = Psx.psxStatus;
    sent_data.ch3 = Psx.psxThumbL;
    sent_data.ch4 = Psx.psxThumbR;
    sent_data.ch5 = Psx.psxJoyRX;
    sent_data.ch6 = Psx.psxJoyRY;
    sent_data.ch7 = Psx.psxJoyLX;
    sent_data.ch8 = Psx.psxJoyLY;
    
    // Optional: Print the received data to the Serial Monitor for debugging.
    // Serial.print("TX Data: ");
    // Serial.print(sent_data.ch5); // Right X-axis
    // Serial.print(" ");
    // Serial.println(sent_data.ch6); // Right Y-axis

    // 3. Transmit the entire data structure wirelessly to the receiver.
    // The sizeof() operator ensures the exact size of the structure is sent.
    radio.write(&sent_data, sizeof(Data_to_be_sent));

    // 4. Introduce a delay to prevent flooding the radio link. 
    // This allows the receiver time to process the command and reduces power consumption.
    delay(20); 
}