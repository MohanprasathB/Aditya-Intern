-const int LED1 = D1;
const int LED2 = D2; 

void setup() {
  // Initializing serial communication at 9600 bits per second
  Serial.begin(9600);

  // Configure the LED pins as outputs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  // Turn both LEDs off initially
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  // Print startup message to the console
  Serial.println("\n ESP8266 Serial communication Ready ");
  Serial.println("'1' to turn on LED 1 and '0' to turn on LED 2");
}

void loop() {
  // Check if character bytes have arrived in the serial hardware buffer
  if (Serial.available() > 0) {
    
    // Read the oldest incoming byte from the queue
    char rx_byte = Serial.read(); 

    // Check if the character matches ASCII '1'
    if (rx_byte == '1') {
      digitalWrite(LED1, HIGH); // Turn LED 1 ON
      digitalWrite(LED2, LOW);  // Force LED 2 OFF
      Serial.println("Received: 1 -> LED 1 is ON, LED 2 is OFF");
    }
    // Check if the character matches ASCII '0'
    else if (rx_byte == '0') {
      digitalWrite(LED1, LOW);  // Force LED 1 OFF
      digitalWrite(LED2, HIGH); // Turn LED 2 ON
      Serial.println("Received: 0 -> LED 1 is OFF, LED 2 is ON");
    }
  }
}