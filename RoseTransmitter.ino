#include <ESPGizmoDefault.h>
#include <Bounce2.h>
#include <ESP8266HTTPClient.h>
#include <FastLED.h>

#define TRANSMITTER     "RoseTransmitter"
#define SW_UPDATE_URL   "http://iot.vachuska.com/RoseTransmitter.ino.bin"
#define SW_VERSION      "2023.11.03.001"

#define EVERY_X_MILLIS(T, N)  if (T < millis()) { T = millis() + N;

#define SPOT_PIN  14
#define DOME_PIN  12
#define PIXIE_PIN 13
#define PETAL_PIN  5

#define LED_PIN   4

CRGB indicator[4];
char state[4];

uint32_t stateTimer = 0;

Bounce2::Button spotButton = Bounce2::Button();
Bounce2::Button domeButton = Bounce2::Button();
Bounce2::Button pixieButton = Bounce2::Button();
Bounce2::Button petalButton = Bounce2::Button();

void setup() {
    gizmo.suggestIP(IPAddress({10, 10, 0, 1}));
    gizmo.beginSetup(TRANSMITTER, SW_VERSION, PASSKEY);
    gizmo.setUpdateURL(SW_UPDATE_URL);
    setupButtons();
    setupIndicator();
    gizmo.endSetup();
}

void setupButtons() {
    setupButton(&spotButton, SPOT_PIN);
    setupButton(&domeButton, DOME_PIN);
    setupButton(&pixieButton, PIXIE_PIN);
    setupButton(&petalButton, PETAL_PIN);
}

void setupButton(Bounce2::Button *button, int pin) {
    button->attach(pin, INPUT_PULLUP);
    button->interval(5);
    button->setPressedState(LOW);
}

void setupIndicator() {
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(indicator, 4);  // GRB ordering is typical
    for (int i = 0; i < 4; i++) {
        indicator[i] = CRGB::Blue;
        state[i] = 255;
    }
    FastLED.setBrightness(32);
    FastLED.show();
}

void loop() {
    if (gizmo.isNetworkAvailable(finishConnection)) {
        handleButtons();
        handleIndicators();

        EVERY_X_MILLIS(stateTimer, 2000)
            sendCommand("state");
        }
    }
}

void finishConnection() {
    Serial.printf("%s is ready\n", TRANSMITTER);
    sendCommand("state");
}

void sendCommand(const char *cmd) {
    char url[128];
    snprintf(url, 127, "http://10.10.10.1/%s", cmd);
    HTTPClient http;
    http.begin(url);
    gizmo.led(true);
    int code = http.GET();
    if (code != HTTP_CODE_OK) {
        Serial.printf("Failed to send command: %s\n", http.errorToString(code).c_str());
    } else {
        String payload = http.getString();
        snprintf(state, 5, "%s", payload.c_str());
        Serial.printf("resp=[%s]\n", state);
    }
    http.end();
    gizmo.led(false);
}

void handleButtons() {
    handleButton(&spotButton, "spot");
    handleButton(&domeButton, "dome");
    handleButton(&pixieButton, "pixie");
    handleButton(&petalButton, "petal");
}

void handleButton(Bounce2::Button *button, const char *name) {
    button->update();
    if (button->pressed()) {
        Serial.printf("Pressed %s button\n", name);
        sendCommand(name);
    }
}

void handleIndicators() {
    for (int i = 0; i < 4; i++) {
        indicator[3 - i] = (state[i] == '0' ? CRGB::Black :
                            (state[i] == '1' ? CRGB::Red :
                             (state[i] == '2' ? CRGB::Orange :
                              (state[i] == '3' ? CRGB::Purple :
                               (state[i] == '4' ? CRGB::Blue : CRGB::Green)))));
    }
    FastLED.show();
}
