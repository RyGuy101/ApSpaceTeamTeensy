#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#define CE_PIN 0 //TODO
#define CSN_PIN 0 //TODO
#define READING_PIPE 0xC2C2C2C2C2
#define WRITING_PIPE 0xE7E7E7E7E7
#define PAYLOAD_SIZE 32

#define LED_1_PIN 2
#define LED_2_PIN 3
#define LED_3_PIN 4

#define LED_1 0x01
#define LED_2 0x02
#define LED_3 0x03

#define CORRECT_BUTTONS 0x04
#define WRONG_BUTTONS 0x05

uint8_t received_data[PAYLOAD_SIZE];

const uint8_t maxSequenceLength = PAYLOAD_SIZE;
uint8_t ledSequence[maxSequenceLength];
uint8_t sequenceIndex = 0;
bool showNewSequence = true;

RF24 rf24(CE_PIN, CSN_PIN);

void setup() {
  Serial.begin(57600);
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  rf24.begin();
  rf24.setChannel(13);
  rf24.setPALevel(RF24_PA_MIN);
  rf24.openReadingPipe(1, READING_PIPE);
  rf24.openWritingPipe(WRITING_PIPE);
  rf24.setCRCLength(RF24_CRC_16);

  rf24.setPayloadSize(PAYLOAD_SIZE);
}

void loop() {
  if (showNewSequence) {
    showNewSequence = false;
    ledSequence[sequenceIndex] = random(1, 4);
    flashLedSequence();
    rf24.write(ledSequence, maxSequenceLength);
    rf24.startListening();
    sequenceIndex++;
  }
  
  if(rf24.available()) {
    rf24.read(&received_data, PAYLOAD_SIZE);
    rf24.stopListening();
    if (received_data[0] == CORRECT_BUTTONS) {
      if (sequenceIndex == maxSequenceLength) {
        flashWin();
        sequenceIndex = 0;
      } else {
        flashCorrect();
      }
    } else {
      flashWrong();
      sequenceIndex = 0;
    }
    showNewSequence = true;
  }  
}

void flashLedSequence() {
  
}

void flashCorrect() {
  
}

void flashWrong() {
  
}

void flashWin() {
  
}

