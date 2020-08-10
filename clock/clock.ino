// An array that stores each digit as it's pin state (on/off) representation
// The pin represented by each bit is: (A-G, DP)
byte PIN_BYTE_MAP[10] = {
  B00000011, // 0
  B10011111, // 1
  B00100101, // 2
  B00001101, // 3
  B10011001, // 4
  B01001001, // 5
  B01000001, // 6
  B00011111, // 7
  B00000001, // 8
  B00001001  // 9
};

// Each index corresponds to the arduino pin of a digit (1-4)
const int NUM_OF_DIGITS = 4;
int DIGIT_PINS[NUM_OF_DIGITS] = {5, 6, 7, 8};

// Each index corresponds to the arduino pin of a digit segment (A-G, DP)
const int NUM_OF_SEGMENTS = 8;
const int DP_SEGMENT = 4;
int SEGMENT_PINS[NUM_OF_SEGMENTS] = {9, 2, 3, 5, 6, 8, 7, 4};

const int SER_PIN = 2;   // Serial/data pin
const int SRCLK_PIN = 4; // Clock pin
const int RCLK_PIN = 3;  // Latch pin

// Global time variables that are referenced while using the display
// The time (HR:MN:SC) is stored in indices 0-5 in sequential order.
int CURR_TIME[6] = {0, 0, 0, 0, 0, 0};

// Initialize all the pin connections
void initialize_pins() {
  // Initialize digit pins
  for (int i = 0; i < NUM_OF_DIGITS; i++) {
    pinMode(DIGIT_PINS[i], OUTPUT);
  }

  // // Initialize digit segment pins
  // for (int i = 0; i < NUM_OF_SEGMENTS; i++) {
  //   pinMode(SEGMENT_PINS[i], OUTPUT);
  // }
  
  // Initialize shift register pins
  pinMode(SER_PIN, OUTPUT);
  pinMode(SRCLK_PIN, OUTPUT);
  pinMode(RCLK_PIN, OUTPUT);
}

// A setup for shift register and LED
// void setup() {
//   Serial.begin(9600);

//   // 3 Pins must be setup
//   const int SER_PIN = 16;   // A2 // Data pin
//   const int SRCLK_PIN = 14; // A0 // Clock pin
//   const int RCLK_PIN = 15;  // A1 // Latch pin
//   pinMode(SER_PIN, OUTPUT);
//   pinMode(SRCLK_PIN, OUTPUT);
//   pinMode(RCLK_PIN, OUTPUT);

//   // Make Latch pin low so nothing changes while we write bits
//   digitalWrite(RCLK_PIN, LOW);

//   // Write/shift the bits
//   shiftOut(SER_PIN, SRCLK_PIN, MSBFIRST, B00000000);

//   // Make Latch pin high so that we can send out the updated state
//   digitalWrite(RCLK_PIN, HIGH);

//   delay(2000);

//   // Make Latch pin low so we are able to make changes to the bits
//   digitalWrite(RCLK_PIN, LOW);

//   // Write/shift the bits
//   shiftOut(SER_PIN, SRCLK_PIN, MSBFIRST, B11111111);

//   // Make Latch pin high to "save" our changes, allowing the register 
//   // to send out the updated state
//   digitalWrite(RCLK_PIN, HIGH);

//   // Pause for a bit
//   delay(5000);
// }

void setup() {
  initialize_pins();
  Serial.begin(9600);

  while(1) {
    // Set the number that will be displayed
    send_number(0);
    // Display the number on the specified digit
    digitalWrite(DIGIT_PINS[0], HIGH);
    // Wait for digit persistence
    delay(3);
    // Turn off the number display
    digitalWrite(DIGIT_PINS[0], LOW);
    
    // Set the number that will be displayed
    send_number(1);
    // Display the number on the specified digit
    digitalWrite(DIGIT_PINS[1], HIGH);
    // Wait for digit persistence
    delay(3);
    // Turn off the number display
    digitalWrite(DIGIT_PINS[1], LOW);
    
    // Set the number that will be displayed
    send_number(2);
    // Display the number on the specified digit
    digitalWrite(DIGIT_PINS[2], HIGH);
    // Wait for digit persistence
    delay(3);
    // Turn off the number display
    digitalWrite(DIGIT_PINS[2], LOW);
    
    // Set the number that will be displayed
    send_number(3);
    // Display the number on the specified digit
    digitalWrite(DIGIT_PINS[3], HIGH);
    // Wait for digit persistence
    delay(3);
    // Turn off the number display
    digitalWrite(DIGIT_PINS[3], LOW);
    
  }
  

  // dont go to loop
  // delay(300 * 1000);
}

void loop() {
  // Update time (either by incrementing seconds or by re-syncing it using python)
  if (Serial.available() > 0) {
    // Read the incoming time data and split it up into digits so that it can be displayed
    // digit-by-digit.
    int numOfDigitsRead = 0;
    while (numOfDigitsRead < 6) {
      // If we successfully convert the incoming data into a displayable format, 
      // we increment numOfDigitsRead so that we can modify specific digits of the display
      if (convert_to_time(Serial.read(), numOfDigitsRead)) {
        numOfDigitsRead++;
      }
    }
    
    // Clear the buffer now that we have read the time
    while (Serial.available() > 0) { Serial.read(); }
  }

  // Display the time on the 4-digit 7-segment display
  display_time();
}

// Converts an integer stored as a CHAR into an INT. --- i.e. '3' -> 3
int char_to_int(char c) {
  return c - '0';
}

// Returns true if c is a digit from 0-9
bool is_int(char c) {
  return c >= '0' && c <= '9';
}

/* Provided that incoming_data is an integer (stored as a char), we convert it
   into a displayable time.
   Return true if conversion is successful.
   Return false otherwise (If the incoming_data is not an integer stored as a char) */
bool convert_to_time(char incoming_data, int numOfDigitsRead) {
  if (is_int(incoming_data)) {
    CURR_TIME[numOfDigitsRead] = char_to_int(incoming_data);
    return true;
  }
  return false;
}

// Using the global time variables, we display the current time
void display_time() {
  for (int digit = 1; digit <= 4; digit++) {
    set_digit(CURR_TIME[digit-1], digit);
  }
}

// Display the provided INTEGER (1-9) in the designated DIGIT (1-4) spot on the display
void set_digit(int integer, int digit) {
  // Turn on/off the segments for the corresponding INTEGER
  for (int pinNum = 0; pinNum < NUM_OF_SEGMENTS; pinNum++) {
    boolean pinState = bitRead(PIN_BYTE_MAP[integer], 7-pinNum);
    digitalWrite(SEGMENT_PINS[pinNum], pinState);
  }

  // Turn on the decimal point if it's the second digit
  if (digit == 2) {
    digitalWrite(DP_SEGMENT, LOW);
  }

  // Activate the specified DIGIT so we can see the INTEGER
  digitalWrite(DIGIT_PINS[digit-1], HIGH);
  
  // Pause in the on state so there is time to see the displayed INTEGER
  delay(3);

  // Deactivate the specified DIGIT
  digitalWrite(DIGIT_PINS[digit-1], LOW);
}

// Sends the specified number's byte representation through to the shift register
void send_number(int digit) {
  // Make Latch pin low so we are able to make changes to the bits
  digitalWrite(RCLK_PIN, LOW);

  // Write/shift the bits
  shiftOut(SER_PIN, SRCLK_PIN, LSBFIRST, PIN_BYTE_MAP[digit]);

  // Make Latch pin high to "save" our changes, allowing the register 
  // to send out the updated state
  digitalWrite(RCLK_PIN, HIGH);
}