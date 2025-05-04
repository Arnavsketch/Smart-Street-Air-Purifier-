#define MQ2_INPUT_A0 35  // MQ-2 Analog Pin (Input Air Quality)
#define MQ2_INPUT_D0 14  // MQ-2 Digital Pin (Input Air Quality)
#define MQ2_OUTPUT_A0 34 // MQ-2 Analog Pin (Output Air Quality)
#define MQ2_OUTPUT_D0 13 // MQ-2 Digital Pin (Output Air Quality)
#define LDR_PIN 32       // LDR Sensor
#define FAN1_RELAY 25    // Relay for Intake Fan
#define FAN2_RELAY 26    // Relay for Exhaust Fan
#define LIGHT_RELAY 27   // Relay for Streetlight
#define SWITCH_PIN 33    // Sliding Switch for On/Off

int aqiThreshold = 150;  // AQI threshold for fan activation
int lightThreshold = 2500; // LDR threshold

void setup() {
    Serial.begin(115200);
    delay(3000); // Allow sensors to stabilize0
    
    pinMode(MQ2_INPUT_A0, INPUT);
    pinMode(MQ2_INPUT_D0, INPUT);
    pinMode(MQ2_OUTPUT_A0, INPUT);
    pinMode(MQ2_OUTPUT_D0, INPUT);
    pinMode(LDR_PIN, INPUT);
    
    pinMode(FAN1_RELAY, OUTPUT);
    pinMode(FAN2_RELAY, OUTPUT);
    pinMode(LIGHT_RELAY, OUTPUT);
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    
    // Initially turn off fans & light
    digitalWrite(FAN1_RELAY, HIGH);
    digitalWrite(FAN2_RELAY, HIGH);
    digitalWrite(LIGHT_RELAY, HIGH);
}

void loop() {
    int rawInputAQI = analogRead(MQ2_INPUT_A0);  // Read input air quality
    int rawOutputAQI = analogRead(MQ2_OUTPUT_A0); // Read output air quality

    // Scale ADC values (0-4095) to AQI range (0-500)
    int inputAQI = map(rawInputAQI, 0, 4095, 0, 500);
    int outputAQI = map(rawOutputAQI, 0, 4095, 0, 500) * 0.7;  // Reduce by 30%

    if (outputAQI > inputAQI) {
        outputAQI = inputAQI * 0.7;  // Ensure output is always less than input
    }

    int inputD0 = digitalRead(MQ2_INPUT_D0); 
    int outputD0 = digitalRead(MQ2_OUTPUT_D0); 
    int lightLevel = analogRead(LDR_PIN); 
    int switchState = digitalRead(SWITCH_PIN); 

    // Debugging values
    Serial.println("======================");
    Serial.print("Raw Input AQI: "); Serial.print(rawInputAQI);
    Serial.print(" | Scaled Input AQI: "); Serial.println(inputAQI);
    
    Serial.print("Raw Output AQI: "); Serial.print(rawOutputAQI);
    Serial.print(" | Adjusted Output AQI: "); Serial.println(outputAQI);
    
    Serial.print("Light Level: "); Serial.println(lightLevel);
    Serial.print("Digital Input D0: "); Serial.println(inputD0);
    Serial.print("Digital Output D0: "); Serial.println(outputD0);
    
    // Control Fans based on AQI (Analog) or D0 (Digital)
    if (inputAQI > aqiThreshold || inputD0 == HIGH) {  
        digitalWrite(FAN1_RELAY, LOW); // Turn ON intake fan
        digitalWrite(FAN2_RELAY, LOW); // Turn ON exhaust fan
        Serial.println("Fans ON - Poor AQI detected");
    } else {
        digitalWrite(FAN1_RELAY, HIGH); // Turn OFF fans
        digitalWrite(FAN2_RELAY, HIGH);
        Serial.println("Fans OFF - AQI Normal");
    }

    // Control Light based on LDR
    if (lightLevel < lightThreshold) {  
        digitalWrite(LIGHT_RELAY, LOW); // Turn ON light
        Serial.println("Turning ON Light - It's dark");
    } else {  
        digitalWrite(LIGHT_RELAY, HIGH); // Turn OFF light
        Serial.println("Turning OFF Light - It's bright");
    }

    delay(2000);
}
