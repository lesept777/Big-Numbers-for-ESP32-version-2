#include "BigNumbers2.h"

void setup() {
  Serial.begin(115200);
  splash();
}

void loop() {
  Serial.println("\n\nPlease enter an operation");
  char* input = readSerial();
  parseInput(input, 12);
}
