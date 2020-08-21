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

/* If there is serial input available, then we assume it is time data (and therefore contains 6 numbers).
   We look at incoming bytes until we find a number.
   If it is a number, we process it and update CURR_TIME
   Processing is done by:
   - Update a specific digit of CURR_TIME, depending on how many numbers have been read.
     - The digits that will be changed go from 0-5, and correspond in the following way:
       - (HR:MN:SC)
       - (01:23:45)
     - To keep track of which digit we're changing, each sucessful processing job will increment
       the tracking variable by one */
void update_time() {
  // If there is time input waiting, we process it so that the arduino can display it
  int timeDigitToChange = 0;
  while (Serial.available() > 0) {
    // Process each incoming byte individually, 
    // If processing is successful, we set which time digit will be changed next
    if (process_incoming_byte(Serial.read(), timeDigitToChange)) {
      timeDigitToChange++;
    }
  }
}

/* Update a specific digit of CURR_TIME, depending on how many numbers have been read.
     - The digits that will be changed go from 0-5, and correspond in the following way:
       - (HR:MN:SC)
       - (01:23:45)
     - If processing is successful, return true
     - Otherwise, return false */
bool process_incoming_byte(char incoming_byte, int timeDigitToChange) {
  // If the incoming byte is actually number
  if (is_int(incoming_byte)) {
    // We update CURR_TIME, which tracks the current time
    CURR_TIME[timeDigitToChange] = char_to_int(incoming_byte);
    return true;
  } else {
    return false;
  }
}

// Converts an integer stored as a CHAR into an INT. --- i.e. '3' -> 3
int char_to_int(char c) {
  return c - '0';
}

// Returns true if c is a digit from 0-9
bool is_int(char c) {
  return c >= '0' && c <= '9';
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
