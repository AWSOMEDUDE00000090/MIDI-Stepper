#include <Arduino.h>
#include <MIDI.h>

const int numRows = 2;
const int numCols = 8;
const int rowPins[numRows] = {8, 9};
const int colPins[numCols] = {0, 1, 2, 3, 4, 5, 6, 7}; 
int keyState[numRows][numCols] = {{0}};
int noteMapping[numRows][numCols] = {
  {60, 62, 64, 65, 67, 69, 71, 72},
  {61, 63, -1, 66, 68, 70, -1, -1}
}; // +12 for each note to change octave

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI);
void setup() {
  for (int row = 0; row < numRows; row++) {
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], HIGH); // Set rows HIGH
  }

  for (int col = 0; col < numCols; col++) {
    pinMode(colPins[col], INPUT_PULLUP);
  }
  Serial3.begin(31250);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  for (int row = 0; row < numRows; row++) {
    digitalWrite(rowPins[row], LOW);  // Activate the current row
    for (int col = 0; col <numCols; col++) {
      if (noteMapping[row][col] == -1) continue;
      int currentState = digitalRead(colPins[col]);
      int note = noteMapping[row][col];

      if (currentState == LOW && keyState[row][col] == 0){
        MIDI.sendNoteOn(note, 127, 1);
        keyState[row][col] = 1;
      }

      if (currentState == HIGH && keyState[row][col] == 1){
        MIDI.sendNoteOff(note, 0, 1);
        keyState[row][col] = 0;
      }
    }
    digitalWrite(rowPins[row], HIGH);
  }
}

