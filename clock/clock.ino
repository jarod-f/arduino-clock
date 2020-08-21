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
};  // Invert a byte using ~ (rather than !)

// Each index corresponds to the arduino pin of a digit (1-4)
const int NUM_OF_DIGITS = 4;
int DIGIT_PINS[NUM_OF_DIGITS] = {5, 6, 7, 8};

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
  
  // Initialize shift register pins
  pinMode(SER_PIN, OUTPUT);
  pinMode(SRCLK_PIN, OUTPUT);
  pinMode(RCLK_PIN, OUTPUT);
}

void setup() {
  initialize_pins();
  Serial.begin(9600);
}

void loop() {
  update_display();
  update_time();
}

void update_display() {
  // alrite, here's the new plan:
  // 1) function to set digits based on 4-digit time, this is always called (running) unless we are receiving information
  // 2) function inside above, this will set the first display, second display, third dispaly  (when setting 2nd and 3rd display, you don't actually have to give it permanance, the shift register will sagve the state)
}

void update_time() {
  // get serial input
  // set the global time variables
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
  shiftOut(SER_PIN, SRCLK_PIN, LSBFIRST, ~PIN_BYTE_MAP[digit]);

  // Make Latch pin high to "save" our changes, allowing the register 
  // to send out the updated state
  digitalWrite(RCLK_PIN, HIGH);
}
