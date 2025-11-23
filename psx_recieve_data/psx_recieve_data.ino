// Include the library for PS2 controller communication (likely used on the transmitter side).
#include <Psx.h> 
// Include the standard SPI communication library for the nRF24L01 module.
#include <SPI.h>
// Include the main library for the nRF24L01 module.
#include <nRF24L01.h>
// Include the RF24 wrapper library for easier control of the nRF24L01 module.
#include <RF24.h>

// Define the motor driver control pins. 
// Assuming an H-bridge or L298N driver wired in an IN1/IN2 and IN3/IN4 configuration.
// These pins should be connected to the inputs of the motor driver.
#define IN1 3 // Left Motor Forward/Input A
#define IN2 5 // Left Motor Backward/Input B
#define IN3 6 // Right Motor Forward/Input C
#define IN4 9 // Right Motor Backward/Input D


// Define the unique 6-byte address (pipe) for communication.
// Both the transmitter and receiver must use the same address.
const uint64_t pipeIn = 0xE8E8F0F0E1LL;

// Initialize the nRF24L01 radio object.
// The constructor takes the CE (Chip Enable) and CSN (Chip Select Not) pin numbers.
// CE is connected to pin 9, CSN is connected to pin 10.
RF24 radio(9, 10); 

// Structure to hold the data received from the transmitter (e.g., PS2 controller states).
// The data is typically an 8-byte array matching the PS2 controller output.
struct Data_to_be_recived {
  byte data[8];
};
Data_to_be_recived r_data; // Global variable to store the received data.

/**
 * @brief Resets the data structure to a known, typically neutral, state.
 * This is useful for clearing previous commands or ensuring a safe startup.
 */
void resetData() {
    for (int i = 0; i < 8; i++) {
        r_data.data[i] = 0;
    }
}

// --- Motor Control Functions ---

/**
 * @brief Sets the motors to move the robot backward.
 * Uses LOW/HIGH pattern for reversal, assuming active-HIGH logic on motor driver inputs.
 */
void back() {
  // Left Motor (Reverse)
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Right Motor (Reverse)
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

/**
 * @brief Sets the motors to move the robot forward.
 * Uses HIGH/LOW pattern for forward movement.
 */
void forward() {
  // Left Motor (Forward)
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Right Motor (Forward)
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

/**
 * @brief Stops all motors.
 */
void stopp() {
  // All inputs LOW to brake or coast (depending on the motor driver type).
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

/**
 * @brief Turns the robot left (e.g., Left motor backward, Right motor forward).
 */
void left() {
  // Left Motor (Backward)
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Right Motor (Forward)
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

/**
 * @brief Turns the robot right (e.g., Left motor forward, Right motor backward).
 */
void right() {
  // Left Motor (Forward)
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Right Motor (Backward)
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// --- Setup Function ---

void setup() {
  // Initialize Serial communication for debugging and viewing received data.
  Serial.begin(115200); 

  // Set the motor control pins as OUTPUTs.
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Call the reset function to clear the data buffer.
  resetData(); 

  // Initialize the nRF24L01 radio module.
  radio.begin();
  // Disable automatic acknowledgments for slightly faster transmission (if not needed).
  radio.setAutoAck(false); 
  // Set the data rate to 250KBPS for maximum range and reliability.
  radio.setDataRate(RF24_250KBPS);
  // Open the receiving pipe on address 1 with the defined pipe address.
  radio.openReadingPipe(1, pipeIn);

  // Switch the radio to receiver mode and start listening for data.
  radio.startListening();
  
  // Ensure the robot is stopped at startup.
  stopp(); 
}

// --- Main Loop Function ---

void loop() {
  // Check if there is data available to be read from the radio.
  if (radio.available()){
    // Read the incoming data into the r_data structure.
    radio.read(&r_data, sizeof(Data_to_be_recived));
    
    // --- Debugging: Print received data in HEX format ---
    for(int index = 0; index<8; index++)
    {
      Serial.print(r_data.data[index], HEX); // Print the byte value in hexadecimal
      Serial.print(" ");
    }
    Serial.println();
    
    // --- Robot Movement Control Logic (Based on PS2 Controller Data) ---
    // The PS2 controller typically sends button states (bits) in r_data.data[3] or r_data.data[4].
    // Assuming r_data.data[3] contains direction buttons (e.g., Up/Down/Left/Right):
    
    // Check for specific button presses in the received data.
    // NOTE: Specific bit masks (0x80, 0x40, etc.) depend on the transmitter's PS2 library implementation.
    
    // Example: If 'Up' button is pressed (assuming 'Up' is bit 6 of r_data.data[3]):
    if (!(r_data.data[3] & 0x10)) { // Assuming 'Up' is mapped to D-pad Up (a common bitmask)
      forward();
    } 
    // Example: If 'Down' button is pressed:
    else if (!(r_data.data[3] & 0x40)) { // Assuming 'Down' is mapped to D-pad Down
      back();
    }
    // Example: If 'Left' button is pressed:
    else if (!(r_data.data[3] & 0x80)) { // Assuming 'Left' is mapped to D-pad Left
      left();
    }
    // Example: If 'Right' button is pressed:
    else if (!(r_data.data[3] & 0x20)) { // Assuming 'Right' is mapped to D-pad Right
      right();
    }
    // If no directional buttons are pressed, stop the robot.
    else {
      stopp();
    }
    
    // For Analogue Joystick Control:
    // If you were using analog sticks, you would read r_data.data[5] to r_data.data[8] 
    // and use analogWrite() instead of digitalWrite() in the movement functions.
  }
  
  // Small delay to prevent the loop from running too fast.
  delay(50);
}