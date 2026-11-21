#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Servo.h>

// --- PIN ASSIGNMENTS ---
#define RST_PIN         9
#define SS_PIN          10
#define SERVO_PIN       A1
#define BUZZER_PIN      A2
#define BLUE_LED_PIN    A3
#define RED_LED_PIN     A4

// --- HARDWARE OBJECTS ---
MFRC522 rfid(SS_PIN, RST_PIN);
Servo lockServo;

// --- AUTHORIZED RFID KEYS ---
byte authorizedUIDs[][4] = {
  {0xF3, 0x34, 0x1B, 0x39}, // Key 1
  {0xD7, 0x6E, 0x10, 0x60}  // Key 2
};
const int numAuthorizedCards = sizeof(authorizedUIDs) / sizeof(authorizedUIDs[0]);

// --- PASSCODE SETTINGS ---
const String masterPIN = "2003";
String enteredPIN = "";

// --- 4x4 KEYPAD SETUP ---
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Row pins connect to D2, D3, D4, D5
byte rowPins[ROWS] = {2, 3, 4, 5}; 

// Column pins connect to D6, D7, D8, A0
byte colPins[COLS] = {6, 7, 8, A0};

Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Function declarations
void playTone(unsigned int freq, unsigned int durationMs);
void playKeySound(char key);
void lockDoor();
void unlockSequence();
void deniedSequence();
void doubleBeep();

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  lockServo.attach(SERVO_PIN);

  // Active-LOW buzzer configuration:
  // HIGH keeps the buzzer completely OFF & COOL while idle
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, HIGH);

  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  // Set initial locked state
  lockDoor();
  Serial.println("System Ready. Scan Card or Enter PIN.");
}

void loop() {
  handleKeypad();
  handleRFID();
}

// --- RFID LOGIC ---
void handleRFID() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  bool accessGranted = false;

  for (int i = 0; i < numAuthorizedCards; i++) {
    bool match = true;
    for (byte b = 0; b < 4; b++) {
      if (rfid.uid.uidByte[b] != authorizedUIDs[i][b]) {
        match = false;
        break;
      }
    }
    if (match) {
      accessGranted = true;
      break;
    }
  }

  if (accessGranted) {
    Serial.println("RFID Matched: Access Granted!");
    unlockSequence();
  } else {
    Serial.println("Unauthorized Card: Access Denied!");
    deniedSequence();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// --- KEYPAD LOGIC ---
void handleKeypad() {
  char key = customKeypad.getKey();
  if (!key) return;

  // Plays a distinct musical tone corresponding to the specific key pressed
  playKeySound(key);

  if (key == '#') {
    if (enteredPIN == masterPIN) {
      Serial.println("PIN Correct: Access Granted!");
      unlockSequence();
    } else {
      Serial.println("Incorrect PIN: Access Denied!");
      deniedSequence();
    }
    enteredPIN = "";
  } else if (key == '*') {
    enteredPIN = "";
    Serial.println("PIN Cleared.");
    doubleBeep();
  } else {
    enteredPIN += key;
  }
}

// --- SOUND EMITTER ---
void playTone(unsigned int freq, unsigned int durationMs) {
  tone(BUZZER_PIN, freq, durationMs);
  delay(durationMs);
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, HIGH); // Re-arm high state so module doesn't draw idle current or heat up
}

// --- INDIVIDUAL KEY SOUND MAPPING ---
void playKeySound(char key) {
  unsigned int freq = 2000;

  switch (key) {
    case '1': freq = 1047; break; // C6
    case '2': freq = 1175; break; // D6
    case '3': freq = 1319; break; // E6
    case 'A': freq = 1397; break; // F6

    case '4': freq = 1568; break; // G6
    case '5': freq = 1760; break; // A6
    case '6': freq = 1976; break; // B6
    case 'B': freq = 2093; break; // C7

    case '7': freq = 2349; break; // D7
    case '8': freq = 2637; break; // E7
    case '9': freq = 2794; break; // F7
    case 'C': freq = 3136; break; // G7

    case '*': freq = 3520; break; // A7
    case '0': freq = 3951; break; // B7
    case '#': freq = 4186; break; // C8
    case 'D': freq = 4435; break; // C#8
  }

  playTone(freq, 70); // 70 ms clear tone
}

// --- HARDWARE ACTIONS ---
void lockDoor() {
  lockServo.write(0);          // 0° = Locked position
  digitalWrite(BLUE_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, HIGH);
}

void unlockSequence() {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, HIGH);

  // Sci-Fi "Laser Chirp" (Rapid ascending frequency sweep)
  for (int freq = 1800; freq <= 3200; freq += 140) {
    playTone(freq, 15);
  }

  lockServo.write(90);         // 90° = Unlocked position
  delay(4000);                 // Door stays open for 4 seconds

  lockDoor();                  // Auto re-lock
}

void deniedSequence() {
  // Descending "Failure / Sad Wah" with Red LED blink
  digitalWrite(RED_LED_PIN, LOW);
  playTone(1800, 160);
  delay(40);
  playTone(1400, 180);
  delay(40);
  playTone(950, 320);          // Low dragging final tone
  digitalWrite(RED_LED_PIN, HIGH);
}

void doubleBeep() {
  playTone(2200, 50);
  delay(60);
  playTone(2200, 50);
}
