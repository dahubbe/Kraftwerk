#include "Timer\timer.h"

auto timer = timer_create_default();

const byte ledPin = 6;
const byte interruptPin = 1;
volatile byte state = LOW;
int counter = 0;

void blink() {
  state = !state;
  counter++;
  
  // Set holdoff to prevent bounces and similar before enabling interrupt again
  timer.in(1000, enable_counter);
  detachInterrupt(interruptPin); 
}

bool enable_counter(void *) {
  EIC->INTFLAG.reg = 128; // Clear bit 7 (EXTINT7) in INTFLAG-register to remove previous triggered interrupts
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);

  return false; // Do not run again
}

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Setup input pin and LED-pin
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  Serial.println(EIC->INTENSET.reg);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);
  Serial.println(EIC->INTENSET.reg);
  //timer.every(1000, count_seconds);
  counter = 0;
  
  Serial.println("Interrupt handled counter");
}

void loop() {
  int last_counter=0;
  
  while(1)
  {
    timer.tick();
    digitalWrite(ledPin, state);
    if (last_counter != counter) {
      Serial.print("Counter: ");
      Serial.println(counter);
      last_counter = counter;
    }    
  }
}
