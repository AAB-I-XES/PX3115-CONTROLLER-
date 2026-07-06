#include <Adafruit_NeoPixel.h>

// ---- Pin map ----
#define PIN_LSTICK_X   1
#define PIN_LSTICK_Y   2
#define PIN_RSTICK_X   4
#define PIN_RSTICK_Y   5
#define PIN_TRIG_LT    6
#define PIN_TRIG_RT    7
#define PIN_L3         8
#define PIN_R3         9

#define PIN_DPAD_UP    10
#define PIN_DPAD_DOWN  11
#define PIN_DPAD_LEFT  12
#define PIN_DPAD_RIGHT 13

#define PIN_A          14
#define PIN_B          15
#define PIN_X          16
#define PIN_Y          17

#define PIN_LB         18
#define PIN_RB         21

#define PIN_START      33
#define PIN_BACK       34
#define PIN_GUIDE      35
#define PIN_SHARE      36

#define PIN_RUMBLE_L   37
#define PIN_RUMBLE_R   38
#define PIN_STATUS_LED 39

Adafruit_NeoPixel statusLed(1, PIN_STATUS_LED, NEO_GRB + NEO_KHZ800);

struct ControllerState {
  int lstickX, lstickY, rstickX, rstickY;
  int triggerLT, triggerRT;
  bool l3, r3;
  bool dpadUp, dpadDown, dpadLeft, dpadRight;
  bool a, b, x, y;
  bool lb, rb;
  bool start, back, guide, share;
};

ControllerState state;

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("Controller simulation starting...");

  pinMode(PIN_L3, INPUT_PULLUP);
  pinMode(PIN_R3, INPUT_PULLUP);
  pinMode(PIN_DPAD_UP, INPUT_PULLUP);
  pinMode(PIN_DPAD_DOWN, INPUT_PULLUP);
  pinMode(PIN_DPAD_LEFT, INPUT_PULLUP);
  pinMode(PIN_DPAD_RIGHT, INPUT_PULLUP);
  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT_PULLUP);
  pinMode(PIN_X, INPUT_PULLUP);
  pinMode(PIN_Y, INPUT_PULLUP);
  pinMode(PIN_LB, INPUT_PULLUP);
  pinMode(PIN_RB, INPUT_PULLUP);
  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_BACK, INPUT_PULLUP);
  pinMode(PIN_GUIDE, INPUT_PULLUP);
  pinMode(PIN_SHARE, INPUT_PULLUP);

  pinMode(PIN_RUMBLE_L, OUTPUT);
  pinMode(PIN_RUMBLE_R, OUTPUT);

  statusLed.begin();
  statusLed.show();
}

void readInputs() {
  state.lstickX = analogRead(PIN_LSTICK_X);
  state.lstickY = analogRead(PIN_LSTICK_Y);
  state.rstickX = analogRead(PIN_RSTICK_X);
  state.rstickY = analogRead(PIN_RSTICK_Y);
  state.triggerLT = analogRead(PIN_TRIG_LT);
  state.triggerRT = analogRead(PIN_TRIG_RT);

  state.l3 = !digitalRead(PIN_L3);
  state.r3 = !digitalRead(PIN_R3);
  state.dpadUp = !digitalRead(PIN_DPAD_UP);
  state.dpadDown = !digitalRead(PIN_DPAD_DOWN);
  state.dpadLeft = !digitalRead(PIN_DPAD_LEFT);
  state.dpadRight = !digitalRead(PIN_DPAD_RIGHT);
  state.a = !digitalRead(PIN_A);
  state.b = !digitalRead(PIN_B);
  state.x = !digitalRead(PIN_X);
  state.y = !digitalRead(PIN_Y);
  state.lb = !digitalRead(PIN_LB);
  state.rb = !digitalRead(PIN_RB);
  state.start = !digitalRead(PIN_START);
  state.back = !digitalRead(PIN_BACK);
  state.guide = !digitalRead(PIN_GUIDE);
  state.share = !digitalRead(PIN_SHARE);
}

void driveOutputs() {
  // Rumble demo: hold A for left rumble, hold B for right rumble.
  // In real hardware these two pins would go to a motor driver
  // (e.g. DRV8833) instead of straight to an LED.
  analogWrite(PIN_RUMBLE_L, state.a ? 200 : 0);
  analogWrite(PIN_RUMBLE_R, state.b ? 200 : 0);

  // Status LED: color follows left stick position, brightness follows
  // how far left stick 3 (click) is engaged, just to prove the analog
  // and digital reads are both feeding into output logic correctly.
  uint8_t r = map(state.lstickX, 0, 4095, 0, 255);
  uint8_t g = map(state.lstickY, 0, 4095, 0, 255);
  uint8_t b = state.l3 ? 255 : 40;
  statusLed.setPixelColor(0, statusLed.Color(r, g, b));
  statusLed.show();
}

void printState() {
  Serial.printf(
    "LX:%4d LY:%4d RX:%4d RY:%4d LT:%4d RT:%4d | "
    "Dpad U:%d D:%d L:%d R:%d | ABXY:%d%d%d%d | LB:%d RB:%d L3:%d R3:%d | "
    "Start:%d Back:%d Guide:%d Share:%d\n",
    state.lstickX, state.lstickY, state.rstickX, state.rstickY,
    state.triggerLT, state.triggerRT,
    state.dpadUp, state.dpadDown, state.dpadLeft, state.dpadRight,
    state.a, state.b, state.x, state.y,
    state.lb, state.rb, state.l3, state.r3,
    state.start, state.back, state.guide, state.share
  );
}

void loop() {
  readInputs();
  driveOutputs();
  printState();
  delay(150);
}

// ---------------------------------------------------------------------
// USB / BLE HID output — not wired up in simulation, real-device only.
// Once you move to real hardware, replace printState()'s role with
// something like this (using a library such as ESP32-BLE-Gamepad for
// wireless, or the Arduino-ESP32 core's native USBHIDGamepad for wired):
//
//   #include <BleGamepad.h>
//   BleGamepad bleGamepad("Xbox-style controller", "Custom", 100);
//   bleGamepad.begin();
//   ...
//   bleGamepad.setAxes(state.lstickX, state.lstickY, state.rstickX,
//                       state.rstickY, state.triggerLT, state.triggerRT);
//   if (state.a) bleGamepad.press(BUTTON_1); else bleGamepad.release(BUTTON_1);
//
// All the reading/mapping logic above stays exactly the same either way.
// ---------------------------------------------------------------------