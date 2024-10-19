#include <Servo.h>
#include <IRremote.h>

const int RECV_PIN = 6;  // Pin connected to the IR receiver signal pin
const int SERV_PIN = 3;
const int MID = 85;
const int d = 2;

Servo servo;
IRrecv irrecv(RECV_PIN);  // Create an instance of the IRrecv class
bool pos = false;

void setup() {
  Serial.begin(9600);
  Serial.println("IR Receiver Initialized");
  servo.attach(SERV_PIN);
  irrecv.begin(RECV_PIN);  // Start the receiver with the specified pin
}

void loop() {
  if (irrecv.decode()) {
    // Initialize an empty string to store the binary representation
    String binaryData = "";

    // Iterate over the raw data buffer
    for (int i = 1; i < irrecv.decodedIRData.rawDataPtr->rawlen; i += 2) {
      // We only care about the pairs of values (pulse + space)
      int pulse = irrecv.decodedIRData.rawDataPtr->rawbuf[i] * USECPERTICK;
      int space = irrecv.decodedIRData.rawDataPtr->rawbuf[i + 1] * USECPERTICK;

      // Define the thresholds for binary 0 and 1 based on data
      if (space > 1500) {
        // Long space indicates a binary 1
        binaryData += "1";
      } else if (space > 400 && space < 700) {
        // Short space indicates a binary 0
        binaryData += "0";
      }
    }

    // Convert the binary to hexadecimal
    String hexData = binaryToHex(binaryData);
    Serial.print("Decoded hex: ");
    Serial.println(hexData);

    if (hexData == "807f817e1") {
      Serial.println("WORKS");
      if (pos) {
        for (int angle = MID; angle > 0; angle -= 5) {
          servo.write(angle);
          delay(d);  // controls the speed of the Servo
        }
        delay(1000);
        for (int angle = 0; angle < MID; angle += 5) {
          servo.write(angle);
          delay(d);  // controls the speed of the Servo
        }
      } else {
        for (int angle = MID; angle < 180; angle += 5) {
          servo.write(angle);
          delay(d);  // controls the speed of the Servo
        }
        delay(1000);
        for (int angle = 180; angle > MID; angle -= 5) {
          servo.write(angle);
          delay(d);  // controls the speed of the Servo
        }
      }
      pos = !pos;
    }


    irrecv.resume();  // Prepare to receive the next signal
  }

  delay(100);  // Small delay to prevent overwhelming the serial monitor
}

// Helper function to convert binary string to hexadecimal string
String binaryToHex(String binary) {
  String hex = "";
  for (int i = 0; i < binary.length(); i += 4) {
    String chunk = binary.substring(i, i + 4);
    int decimalValue = strtol(chunk.c_str(), NULL, 2);
    hex += String(decimalValue, HEX);
  }
  return hex;
}
