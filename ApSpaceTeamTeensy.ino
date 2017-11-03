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

#define RED_LED LED_1_PIN
#define YELLOW_LED LED_2_PIN
#define GREEN_LED LED_3_PIN

#define BAUD_RATE 57600

#define CORRECT_BUTTONS 0x04
#define WRONG_BUTTONS 0x05

#define NUM_STATE_FLASHES 0x01

uint8_t received_data[PAYLOAD_SIZE];
// Define a  constant value to represent the maximum payload size we can transmit over the RF24
const uint16_t maxSequenceLength = 1024;
// Define the maximum LED sequence length we should handle
uint8_t ledSequence[PAYLOAD_SIZE];
uint8_t sequenceIndex = 0;
bool showNewSequence = true;

RF24 rf24(CE_PIN, CSN_PIN);

void setup() {
  // Our baud rate is 57600 
  Serial.begin(BAUD_RATE);
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  // Initialize the RF24 connected to the teensy
  rf24.begin();
  // Set our communication channel to  13 for our team #
  rf24.setChannel(13);
  // Our power amplification level will be the minimum
  rf24.setPALevel(RF24_PA_MIN);
  rf24.openReadingPipe(1, READING_PIPE);
  rf24.openWritingPipe(WRITING_PIPE);
  // Our cyclic redundancy check will be 2 bytes long at the end of our data payload
  rf24.setCRCLength(RF24_CRC_16);
  rf24.setPayloadSize(PAYLOAD_SIZE);
}

void loop() {
  if (showNewSequence) {
    showNewSequence = false;
    ledSequence[sequenceIndex] = random(1, 4);
    flashLedSequence(ledSequence, (sequenceIndex + 1));
    int write_start_index = 0;
    for(int i = 0; i < (write_start_index/PAYLOAD_SIZE) + 1; i++){
      uint8_t data_to_write[PAYLOAD_SIZE];
      for(int j = 0; j < PAYLOAD_SIZE; j++){
        data_to_write[j] = ledSequence[write_start_index];
        write_start_index++;
      }
      rf24.write(data_to_write, PAYLOAD_SIZE);
    }
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

void flashLedSequence(uint8_t *led_sequence, uint8_t sequence_length) {
  for(int i = 0; i < sequence_length; i++){
    digitalWrite(led_sequence[i] + 1, HIGH);
    delay(750);
    digitalWrite(led_sequence[i] + 1, LOW);
  }
}

void flashCorrect() {
  for(int i = 0; i < NUM_STATE_FLASHES; i++){
    digitalWrite(GREEN_LED, HIGH);
    delay(250);
    digitalWrite(GREEN_LED, LOW);
  }
}

void flashWrong() {
  for(int i = 0; i < NUM_STATE_FLASHES; i++){
    digitalWrite(RED_LED, HIGH);
    delay(250);
    digitalWrite(RED_LED, LOW);
  }
}

void flashWin() {
  for(int i = 0; i < 3; i++){
    for(int j = 2; j < 5; j++){
      digitalWrite(j, HIGH);
    }
    delay(250);
    for(int j = 2; j < 5; j++){
      digitalWrite(j, LOW);
    }
  }
}

