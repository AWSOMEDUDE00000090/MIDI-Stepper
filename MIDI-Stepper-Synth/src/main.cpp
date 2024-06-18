#include <MIDI.h>
#include <AccelStepper.h>

// Define stepper motor connections and enable pins
#define STEP1_PIN 8
#define ENABLE1_PIN 10
#define STEP2_PIN 7
#define ENABLE2_PIN 13
#define STEP3_PIN 4
#define ENABLE3_PIN 12
#define STEP4_PIN 2
#define ENABLE4_PIN 11

// Create instances of the AccelStepper class
AccelStepper stepper1(AccelStepper::DRIVER, STEP1_PIN, ENABLE1_PIN);
AccelStepper stepper2(AccelStepper::DRIVER, STEP2_PIN, ENABLE2_PIN);
AccelStepper stepper3(AccelStepper::DRIVER, STEP3_PIN, ENABLE3_PIN);
AccelStepper stepper4(AccelStepper::DRIVER, STEP4_PIN, ENABLE4_PIN);

/*
   pitches.h
   Made By Jonathan Kayne
   April 2018

   This is an array of MIDI pitches and their respective speed values.
   The index is the MIDI pitch value
   The value stored is the time value between pulses on the stepper motor that produce the desired tone.

   You can adjust these to tune your stepper motor.
   Note that the duration is in microseconds, so 1 second = 1,000,000 microseconds.

   Calculation:
   pitchVal = 1000000/frequency
   
*/

const long pitchVals[] = {
  0, //0, C-1
  0, //1, C#-1
  0, //2, D-1
  0, //3, D#-1
  0, //4, E-1
  0, //5, F-1
  0, //6, F#-1
  0, //7, G-1
  0, //8, G#-1
  0, //9, A-1
  0, //10, A#-1
  0, //11, B-1
  0, //12, C0
  0, //13, C#0
  0, //14, D0
  0, //15, D#0
  0, //16, E0
  0, //17, F0
  0, //18, F#0
  0, //19, G0
  0, //20, G#0
  0, //21, A0
  0, //22, A#0
  32258, //  23  B0 31
  30303,  //  24  C1  33
  28571, //  25  CS1 35
  27027, //  26  D1  37
  25641, //  27  DS1 39
  24390,  //  28  E1  41
  22727, //  29  F1  44
  21739, //  30  FS1 46
  20408, //  31  G1  49
  19230, //  32  GS1 52
  18182, //  33  A1  55
  17241, //  34  AS1 58
  16129, //  35  B1  62
  15385, //  36  C2  65
  14493, //  37  CS2 69
  13699, //  38  D2  73
  12821, //  39  DS2 78
  12195, //  40  E2  82
  11494, //  41  F2  87
  10753, //  42  FS2 93
  10204, //  43  G2  98
  9615, //  44  GS2 104
  9091, //  45  A2  110
  8547, //  46  AS2 117
  8130, //  47  B2  123
  7634, //  48  C3  131
  7194, //  49  CS3 139
  6803, //  50  D3  147
  6410,  //  51  DS3 156
  6061, //  52  E3  165
  5714, //  53  F3  175
  5405, //  54  FS3 185
  5102, //  55  G3  196
  4808, //  56  GS3 208
  4545, //  57  A3  220
  4292, //  58  AS3 233
  4049, //  59  B3  247
  3817, //  60  C4  262
  3610, //  61  CS4 277
  3401, //  62  D4  294
  3215, //  63  DS4 311
  3030,  //  64  E4  330
  2865, //  65  F4  349
  2703, //  66  FS4 370
  2551, //  67  G4  392
  2410, //  68  GS4 415
  2273, //  69  A4  440
  2146, //  70  AS4 466
  2024, //  71  B4  494
  1912, //  72  C5  523
  1805, //  73  CS5 554
  1704, //  74  D5  587
  1608, //  75  DS5 622
  1517, //  76  E5  659
  1433, //  77  F5  698
  1351, //  78  FS5 740
  1276, //  79  G5  784
  1203, //  80  GS5 831
  1136, //  81  A5  880
  1073, //  82  AS5 932
  1012, //  83  B5  988
  955, //  84  C6  1047
  902, //  85  CS6 1109
  851, //  86  D6  1175
  803, //  87  DS6 1245
  758, //  88  E6  1319
  716, //  89  F6  1397
  676, //  90  FS6 1480
  638,  //  91  G6  1568
  602, //  92  GS6 1661
  568, //  93  A6  1760
  536, //  94  AS6 1865
  506, //  95  B6  1976
  478, //  96  C7  2093
  451,  //  97  CS7 2217
  426, //  98  D7  2349
  402, //  99  DS7 2489
  379, //  100 E7  2637
  358, //  101 F7  2794
  338, //  102 FS7 2960
  315,  //  103 G7  3136
  301, //  104 GS7 3322
  284, //  105 A7  3520
  268, //  106 AS7 3729
  253, //  107 B7  3951
  239, //  108 C8  4186
  225, //  109 CS8 4435
  213, //  110 D8  4699
  201, //  111 DS8 4978
  0, //112, E8
  0, //113, F8
  0, //114, F#8
  0, //115, G8
  0, //116, G#8
  0, //117, A8
  0, //118, A#8
  0, //119, B8
  0, //120, C9
  0, //121, C#9
  0, //122, D9
  0, //123, D#9
  0, //124, E9
  0, //125, F9
  0, //126, F#9
  0, //127, G9
};





// Create a MIDI instance
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

// Track active notes and timings for each motor
struct Motor {
  int note;
  unsigned long interval;
  unsigned long lastStepTime;
  bool isActive;
  int enablePin;
};

Motor motors[4] = {
  { -1, 0, 0, false, ENABLE1_PIN },
  { -1, 0, 0, false, ENABLE2_PIN },
  { -1, 0, 0, false, ENABLE3_PIN },
  { -1, 0, 0, false, ENABLE4_PIN }
};

// List to track currently active notes
int activeNotes[4] = { -1, -1, -1, -1 };

void setMotorSpeed(int motorNumber, unsigned long interval, int note) {
  motors[motorNumber].note = note;
  motors[motorNumber].interval = interval;
  motors[motorNumber].lastStepTime = micros();
  motors[motorNumber].isActive = true;
  digitalWrite(motors[motorNumber].enablePin, LOW); // Enable the motor
}

void stopMotor(int motorNumber) {
  motors[motorNumber].isActive = false;
  motors[motorNumber].note = -1;
  digitalWrite(motors[motorNumber].enablePin, HIGH); // Disable the motor
}

void stepMotor(int motorNumber) {
  int stepPin;

  switch (motorNumber) {
    case 0:
      stepPin = STEP1_PIN;
      break;
    case 1:
      stepPin = STEP2_PIN;
      break;
    case 2:
      stepPin = STEP3_PIN;
      break;
    case 3:
      stepPin = STEP4_PIN;
      break;
  }

  // Toggle the step pin to create a step
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(1); // Small delay for the step signal
  digitalWrite(stepPin, LOW);
}

void handleNoteOn(int note) {
  for (int i = 0; i < 4; i++) {
    if (activeNotes[i] == -1) {
      activeNotes[i] = note;
      setMotorSpeed(i, pitchVals[note], note);
      break;
    }
  }
}

void handleNoteOff(int note) {
  for (int i = 0; i < 4; i++) {
    if (activeNotes[i] == note) {
      activeNotes[i] = -1;
      stopMotor(i);
      break;
    }
  }
}

void setup() {
  Serial.begin(31250); // Initialize Serial for MIDI communication at 31250 baud rate


  // Enable the steppers
  for (int i = 0; i < 4; i++) {
    pinMode(motors[i].enablePin, OUTPUT);
    digitalWrite(motors[i].enablePin, HIGH);  // Set HIGH to disable
  }

  // Initialize MIDI communication
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  if (MIDI.read()) {
    byte type = MIDI.getType();
    byte note = MIDI.getData1();
    byte velocity = MIDI.getData2();

    if (type == midi::NoteOn && velocity > 0) {
      handleNoteOn(note);
    } else if (type == midi::NoteOff || (type == midi::NoteOn && velocity == 0)) {
      // Note Off event
      handleNoteOff(note);
    }
  }

  // Manually control the stepping of each motor
  for (int i = 0; i < 4; i++) {
    if (motors[i].isActive) {
      unsigned long currentTime = micros();
      if (currentTime - motors[i].lastStepTime >= motors[i].interval) {
        stepMotor(i);
        motors[i].lastStepTime = currentTime;
      }
    }
  }
}