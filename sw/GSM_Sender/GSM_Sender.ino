/*
 SMS sender

 This sketch, for the MKR GSM 1400 board,sends an SMS message
 you enter in the serial monitor. Connect your Arduino with the
 GSM shield and SIM card, open the serial monitor, and wait for
 the "READY" message to appear in the monitor. Next, type a
 message to send and press "return". Make sure the serial
 monitor is set to send a newline when you press return.

 Circuit:
 * MKR GSM 1400 board
 * Antenna
 * SIM card that can send SMS

 created 25 Feb 2012
 by Tom Igoe
*/

// Include the GSM library
#include <MKRGSM.h> //
#include <timer.h> // https://github.com/contrem/arduino-timer

//#include "arduino_secrets.h" 
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = "0000";

// initialize the library instance
GSM gsmAccess;
GSM_SMS sms;
GSMBand band;
GSMScanner scanner;

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
  
  
  Serial.println("SMS Messages Sender");

  

  return;
  
  // Test code
  Serial.println("Reading networks...");
  scanner.begin();
  //Serial.println(scanner.readNetworks());
  
  Serial.println("Reading band...");
  band.begin();
  Serial.println(band.getBand());

  // connection state
  bool connected = false;

  /*
  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected) {
        
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GSM initialized");*/
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


  Serial.print("Enter a mobile number: ");
  char remoteNum[20];  // telephone number to send sms
  readSerial(remoteNum);
  Serial.println(remoteNum);

  // sms text
  Serial.print("Now, enter SMS content: ");
  char txtMsg[200];
  readSerial(txtMsg);
  Serial.println("SENDING");
  Serial.println();
  Serial.println("Message:");
  Serial.println(txtMsg);

  // send the message
  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS();
  Serial.println("\nCOMPLETE!\n");
}

/*
  Read input serial
 */
int readSerial(char result[]) {
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}
