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

// This display will turn on/update each display in a way that ensures the numbers are visible
void update_display() {
  // Set each of the 4 digits on the 4-digit 7-segment display in a way that lets the human eye see it.
  // Also sets the 7-segment displays so that the seconds are displayed correctly
  for (int i = 0; i < 4; i++) {
    set_display_digits(CURR_TIME[i], i, CURR_TIME[4], CURR_TIME[5]);
  }
}

// Sends the specified number's byte representation through to the shift register
void set_display_digits(int displayOneNumber, int digitSlot, int displayTwoNumber, int displayThreeNumber) {
  // Make Latch pin low so we are able to make changes to the bits
  digitalWrite(RCLK_PIN, LOW);

  // Set 7-segment display digits.
  // - They are common cathode, so we invert the bits
  shiftOut(SER_PIN, SRCLK_PIN, LSBFIRST, ~PIN_BYTE_MAP[displayThreeNumber]);  // Last 7-segment display
  shiftOut(SER_PIN, SRCLK_PIN, LSBFIRST, ~PIN_BYTE_MAP[displayTwoNumber]);  // Second last 7-segment display

  // Set the 4-digit 7-segment display digits.
  // - They are common anode, so we don't invert the bits.
  shiftOut(SER_PIN, SRCLK_PIN, LSBFIRST, PIN_BYTE_MAP[displayOneNumber]);

  // Make Latch pin high to "save" our changes, allowing the register to send out the updated state
  // to send out the updated state
  digitalWrite(RCLK_PIN, HIGH);

  // Turn on one digit on the 4-digit 7-segment display
  digitalWrite(DIGIT_PINS[digitSlot], HIGH);

  // Wait 5ms so that our eyes see a constant digit being emitted from the display
  delay(5);

  // Turn off the 4-digit 7-segment display so we can set the next digit
  digitalWrite(DIGIT_PINS[digitSlot], LOW);
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
  // If there is input available, we read it...
  if (Serial.available() > 0) {
    int timeDigitToChange = 0;
    // We don't stop reading the serial port until we get all 6 digits of data we need
    while (timeDigitToChange < 6) {
      // While we are looking for data, we only want to draw from the Serial port if there is data to pull
      if (Serial.available() > 0 && process_incoming_byte(Serial.read(), timeDigitToChange)) {
        timeDigitToChange++;
      }
    }

    // Clear the buffer now that we have read all the time digits
    while (Serial.available() > 0) { Serial.read(); }
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
