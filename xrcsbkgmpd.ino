#include <Keyboard.h>

// Pins
const int LEFT_TRIGGER_PIN = 2;
const int RIGHT_TRIGGER_PIN = 3;
const int ROTATION_SENSOR_PIN = 4;

// Other variables
unsigned long currentMillis = 0;
unsigned long lastRotation = 0;
float rpm = 0;
float avgOldRpm = 0;
float oldRpms[30];
bool isUpPressed = false;
bool isLeftPressed = false;
bool isRightPressed = false;
bool isPaused = false;

// Setup function
void setup() {
    // Define pins as inputs
    pinMode(ROTATION_SENSOR_PIN, INPUT_PULLUP);
    pinMode(LEFT_TRIGGER_PIN, INPUT);
    pinMode(RIGHT_TRIGGER_PIN, INPUT);
    // Initialize keyboard functionality
    Keyboard.begin();
    // Wait for activation by user
    while (digitalRead(LEFT_TRIGGER_PIN) == LOW || digitalRead(RIGHT_TRIGGER_PIN) == LOW) {
        delay(100);
    }
    lastRotation = millis();
}

// Main function
void loop() {
    currentMillis = millis();
    // Calculate current RPM on every rotation when the user is pedalling and respond with a game input
    if (digitalRead(ROTATION_SENSOR_PIN) == HIGH && currentMillis - lastRotation >= 200 &&
        currentMillis - lastRotation <= 2000) {
        // Resume the game if it was previously paused
        if (isPaused) {
            Keyboard.write(KEY_ESC);
            delay(100);
            isPaused = false;
        }
        // Calculate RPM and average old RPM
        rpm = 60000.0 / (currentMillis - lastRotation);
        avgOldRpm = (oldRpms[0] + oldRpms[1] + oldRpms[2] + oldRpms[3] + oldRpms[4] + oldRpms[5] + oldRpms[6] +
                     oldRpms[7] + oldRpms[8] + oldRpms[9] + oldRpms[10] + oldRpms[11] + oldRpms[12] + oldRpms[13] +
                     oldRpms[14] + oldRpms[15] + oldRpms[16] + oldRpms[17] + oldRpms[18] + oldRpms[19] + oldRpms[20] +
                     oldRpms[21] + oldRpms[22] + oldRpms[23] + oldRpms[24] + oldRpms[25] + oldRpms[26] + oldRpms[27] +
                     oldRpms[28] + oldRpms[29]) /
                    30.0;
        // Accelerate + Nitro
        if (rpm > avgOldRpm + 5) {
            if (!isUpPressed) {
                Keyboard.press(KEY_UP_ARROW);
                isUpPressed = true;
            }
            Keyboard.write('N');
        }
        // Brake
        else if (rpm < avgOldRpm - 5 && isUpPressed) {
            Keyboard.release(KEY_UP_ARROW);
            isUpPressed = false;
        }
        // Accelerate
        else if (rpm <= avgOldRpm + 5 && rpm >= avgOldRpm - 5 && !isUpPressed) {
            Keyboard.press(KEY_UP_ARROW);
            isUpPressed = true;
        }
        // Update old RPMs
        for (int i = 29; i > 0; i--) {
            oldRpms[i] = oldRpms[i - 1];
        }
        oldRpms[0] = rpm;
        lastRotation = currentMillis;
    }
    // Pause game when user stops pedalling
    if (currentMillis - lastRotation > 2000) {
        if (!isPaused) {
            Keyboard.releaseAll();
            Keyboard.write(KEY_ESC);
            rpm = 0;
            avgOldRpm = 0;
            for (int i = 0; i < 30; i++) {
                oldRpms[i] = 0;
            }
            isUpPressed = false;
            isLeftPressed = false;
            isRightPressed = false;
            isPaused = true;
        }
        lastRotation = currentMillis;
    }
    if (!isPaused) {
        // Fire
        if (digitalRead(LEFT_TRIGGER_PIN) == HIGH && digitalRead(RIGHT_TRIGGER_PIN) == HIGH) {
            Keyboard.write(' ');
        } else {
            // Steer left
            if (digitalRead(LEFT_TRIGGER_PIN) == HIGH && !isLeftPressed) {
                Keyboard.press(KEY_LEFT_ARROW);
                isLeftPressed = true;
            } else if (digitalRead(LEFT_TRIGGER_PIN) == LOW && isLeftPressed) {
                Keyboard.release(KEY_LEFT_ARROW);
                isLeftPressed = false;
            }
            // Steer right
            if (digitalRead(RIGHT_TRIGGER_PIN) == HIGH && !isRightPressed) {
                Keyboard.press(KEY_RIGHT_ARROW);
                isRightPressed = true;
            } else if (digitalRead(RIGHT_TRIGGER_PIN) == LOW && isRightPressed) {
                Keyboard.release(KEY_RIGHT_ARROW);
                isRightPressed = false;
            }
        }
    }
}
