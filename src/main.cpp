#include <Adafruit_Protomatter.h>
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <WiFi.h>
#include <esp_netif.h>
#include <time.h>
#include <vector>
#include "cmd_interpreter.h"

// Exposed (non-static) by WiFiGeneric.cpp in arduino-esp32; not declared in any public header.
extern esp_netif_t *get_esp_interface_netif(esp_interface_t interface);

#define HOSTNAME "ESP32RGBMatrix"
#define NTP_SERVER "pool.ntp.org"
#define NTP_RESYNC_INTERVAL_MS (8UL * 60UL * 60UL * 1000UL)
#define IPCONFIG "#IPConfig"
#define CONTROL_PORT 5000
static constexpr uint32_t CLOCK_MIN = 100000;
static unsigned long lastNtpSyncMillis = 0;

// Adafruit Matrix Portal S3 HUB75 pins.
uint8_t rgbPins[] = {42, 41, 40, 38, 39, 37};
uint8_t addrPins[] = {45, 36, 48, 35};

constexpr uint8_t CLK_PIN = 2;
constexpr uint8_t LAT_PIN = 47;
constexpr uint8_t OE_PIN = 14;
constexpr uint16_t MATRIX_WIDTH = 32;
constexpr uint16_t MATRIX_HEIGHT = 32;

Adafruit_Protomatter protomatterMatrix(MATRIX_WIDTH,
										6,
										1,
										rgbPins,
										4,
										addrPins,
										CLK_PIN,
										LAT_PIN,
										OE_PIN,
										true);

Adafruit_Protomatter* matrix = &protomatterMatrix;

#define VPANEL_W MATRIX_WIDTH
#define VPANEL_H MATRIX_HEIGHT

#include "AuroraProtomatterCompat.h"

ProtomatterOutput output;
ProtomatterOutput* virtualDisp = &output;

#include <cstdlib>
#define free(pointer) ::free(pointer)
#include "aurora/EffectsLayer.hpp"
#undef free
EffectsLayer effects(MATRIX_WIDTH, MATRIX_HEIGHT);

#include "aurora/Drawable.hpp"
#include "aurora/Geometry.hpp"
#include "aurora/Patterns.hpp"

Patterns patterns;

constexpr uint32_t PATTERN_DURATION_MS = 30000;
constexpr uint32_t PALETTE_DURATION_MS = 10000;
uint32_t lastPatternChange = 0;
uint32_t lastPaletteChange = 0;
uint32_t nextFrameAt = 0;
bool autoAdvance = true;
bool displayOn = true;
static WiFiServer controlServer(CONTROL_PORT);
static WiFiClient controlClient;
static char controlLine[256];
static size_t controlLength = 0;

static void sendState();

void changePattern(int8_t step) {
	patterns.move(step);
	lastPatternChange = millis();
	nextFrameAt = lastPatternChange;
	sendState();
}

static bool clockValid() {
	return time(nullptr) >= CLOCK_MIN;
}

static void resyncNtpIfDue() {
	unsigned long now = millis();
	if (now - lastNtpSyncMillis < NTP_RESYNC_INTERVAL_MS) return;
	configTime(0, 0, NTP_SERVER);
	lastNtpSyncMillis = now;
}

// Persists the current static IP config so it can be re-applied at boot (WiFi.config() itself is not remembered by the IDF).
static void saveIP() {
	Preferences preferences;
	if (!preferences.begin(IPCONFIG, false)) {
		Serial.println("ERR: unable to open IP config namespace");
		return;
	}
	preferences.putULong("ip", static_cast<uint32_t>(WiFi.localIP()));
	preferences.putULong("gateway", static_cast<uint32_t>(WiFi.gatewayIP()));
	preferences.putULong("mask", static_cast<uint32_t>(WiFi.subnetMask()));
	preferences.end();
}

static void clearStaticIpConfig() {
	Preferences preferences;
	if (preferences.begin(IPCONFIG, false)) {
		preferences.clear();
		preferences.end();
	}
}

// Applies a previously saved static IP config, if any, directly via WiFi.config().
static bool loadIP() {
	Preferences preferences;
	if (!preferences.begin(IPCONFIG, true)) return false;
	IPAddress ipValue = preferences.getULong("ip", 0);
	IPAddress gatewayValue = preferences.getULong("gateway", 0);
	IPAddress maskValue = preferences.getULong("mask", 0);
	preferences.end();
	if (ipValue == IPAddress() || maskValue == IPAddress()) return false;
	return WiFi.config(ipValue, gatewayValue, maskValue);
}

// Reads the live esp_netif state so it always matches reality, including after a reboot.
static bool isDhcpEnabled() {
	esp_netif_t *netif = get_esp_interface_netif(ESP_IF_WIFI_STA);
	esp_netif_dhcp_status_t dhcpStatus;
	if (!netif || esp_netif_dhcpc_get_status(netif, &dhcpStatus) != ESP_OK) return true;
	return dhcpStatus == ESP_NETIF_DHCP_STARTED;
}

static void printStatus(Print &out) {
	time_t now = time(nullptr);
	struct tm timeinfo;
	char timeStr[32] = "invalid";
	if (clockValid() && gmtime_r(&now, &timeinfo)) {
		strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S UTC", &timeinfo);
	}
	out.printf("status wifi=%s time=\"%s\" dhcp=%s display=%s ip=",
				  WiFi.status() == WL_CONNECTED ? "up" : "down",
				  timeStr,
				  isDhcpEnabled() ? "on" : "off",
				  displayOn ? "on" : "off");
	out.print(WiFi.localIP());
	out.print(" mask=");
	out.print(WiFi.subnetMask());
	out.print(" gateway=");
	out.print(WiFi.gatewayIP());
	out.print(" dns=");
	out.print(WiFi.dnsIP());
	out.println();
}

static void configureStaticIp(const char *localText, const char *gatewayText, const char *subnetText, Print &out) {
	IPAddress localIp;
	IPAddress gateway;
	IPAddress subnet;
	if (!localIp.fromString(localText) || !gateway.fromString(gatewayText) || !subnet.fromString(subnetText)) {
		out.println("ERR: invalid IP address");
		return;
	}
	if (!WiFi.config(localIp, gateway, subnet)) {
		out.println("ERR: static IP config failed");
		return;
	}
	WiFi.disconnect();
	WiFi.begin();
	saveIP();
	out.println("static ip configured");
}

static void configureDhcp(Print &out) {
	IPAddress unset;
	if (!WiFi.config(unset, unset, unset)) {
		out.println("ERR: DHCP config failed");
		return;
	}
	clearStaticIpConfig();
	WiFi.disconnect();
	WiFi.begin();
	out.println("dhcp configured");
}

// Pushes the display state to the console and to the control client, if any.
// pattern en dernier, entre guillemets (le nom peut contenir des espaces)
static void sendState() {
	const char *fmt = "state on=%d auto=%d pattern=\"%s\"\n";
	Serial.printf(fmt, displayOn, autoAdvance, patterns.getCurrentPatternName());
	if (controlClient.connected()) controlClient.printf(fmt, displayOn, autoAdvance, patterns.getCurrentPatternName());
}

// Protomatter resume() re-runs the whole ESP32-S3 init (new DMA channel each time) and the display
// stays off, so restart only what stop() halted: scan counters and the refresh timer.
extern Protomatter_core *_PM_protoPtr;
static void matrixResume() {
	Protomatter_core *core = _PM_protoPtr;
	core->plane = core->numPlanes - 1;
	core->row = core->numRowPairs - 1;
	core->prevRow = (core->numRowPairs > 1) ? (core->row - 1) : 1;
	core->swapBuffers = 0;
	hw_timer_t *timer = static_cast<hw_timer_t *>(core->timer);
	timerWrite(timer, 0);
	timerAlarm(timer, 1000, true, 0);
	timerStart(timer);
}

// Off: Protomatter stop() blanks the matrix and halts the refresh (timer + DMA); animations are
// suspended, WiFi stays up.
static void setDisplay(bool on) {
	if (on != displayOn) {
		displayOn = on;
		if (on) {
			matrixResume();
			lastPatternChange = millis();
			lastPaletteChange = lastPatternChange;
			nextFrameAt = lastPatternChange;
		} else {
			matrix->stop();
		}
	}
	sendState();
}

// Single TCP control client (a new connection replaces the previous one), one command per line.
static void handleControlServer() {
	WiFiClient incoming = controlServer.accept();
	if (incoming) {
		controlClient.stop();
		controlClient = incoming;
		controlClient.setNoDelay(true);
		controlLength = 0;
		sendState();
	}
	while (controlClient.connected() && controlClient.available()) {
		char c = static_cast<char>(controlClient.read());
		if (c == '\r') continue;
		if (c == '\n') {
			controlLine[controlLength] = '\0';
			controlLength = 0;
			handleConsole(controlLine, controlClient);
		} else if (controlLength < sizeof(controlLine) - 1) {
			controlLine[controlLength++] = c;
		}
	}
}

static void setAutoAdvance(bool on) {
	autoAdvance = on;
	lastPatternChange = millis();
	sendState();
}

const Command COMMANDS[] = {
	CMD("?",        0, { printHelp(out); },        "?"),
	CMD("help",     0, { printHelp(out); },        "help"),
	CMD("status",   0, { printStatus(out); },      "status"),
	CMD("ping",     0, { out.println("pong"); },   "ping"),
	CMD("on",       0, { setDisplay(true); },      "on (display on)"),
	CMD("off",      0, { setDisplay(false); },     "off (display off)"),
	CMD("wifi",     2, { WiFi.begin(argv[0], argv[1]); out.println("wifi configured"); }, "wifi <ssid> <password>"),
	CMD("ipstatic", 3, { configureStaticIp(argv[0], argv[1], argv[2], out); }, "ipstatic <ip> <gateway> <subnet>"),
	CMD("ipdhcp",   0, { configureDhcp(out); },    "ipdhcp"),
	CMD("n",        0, { changePattern(1); },      "n (next pattern)"),
	CMD("p",        0, { changePattern(-1); },     "p (previous pattern)"),
	CMD("a",        0, { setAutoAdvance(!autoAdvance); }, "a (toggle auto advance)"),
	CMD("play",     0, { setAutoAdvance(true); },  "play (auto advance on)"),
	CMD("pause",    0, { setAutoAdvance(false); }, "pause (stay on current pattern)"),
};

const size_t COMMANDS_COUNT = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

void setup() {
	Serial.begin(115200);
	// delay(2000);
	Serial.print("Starting ...");

	const auto status = matrix->begin();
	if (status != PROTOMATTER_OK) {
		Serial.print("Protomatter init failed: ");
		Serial.println(status);
		while (true) {
			delay(1000);
		}
	}

	patterns.listPatterns();
	lastPatternChange = millis();
	lastPaletteChange = lastPatternChange;
	Serial.print("Starting Aurora pattern: ");
	Serial.println(patterns.getCurrentPatternName());

	WiFi.persistent(true);
	WiFi.setAutoReconnect(true);
	WiFi.mode(WIFI_STA);
	WiFi.setHostname(HOSTNAME);
	loadIP();
	WiFi.begin();
	configTime(0, 0, NTP_SERVER);
	lastNtpSyncMillis = millis();
	ArduinoOTA.setHostname(HOSTNAME);
	ArduinoOTA.onStart([]() { setDisplay(false); });
	ArduinoOTA.begin();
	controlServer.begin();
	printStatus(Serial);
}

void loop() {
	handleSerialInput();
	resyncNtpIfDue();
	ArduinoOTA.handle();
	handleControlServer();
	if (!displayOn) return;
	const uint32_t now = millis();

	// Changement aléatoire de palette désactivé : palette fixe RainbowColors_p (chargée par EffectsLayer),
	// pour un rendu identique aux ports Soulmate IDE.
	// if (now - lastPaletteChange >= PALETTE_DURATION_MS) {
	// 	effects.RandomPalette();
	// 	lastPaletteChange = now;
	// }

	if (autoAdvance && now - lastPatternChange >= PATTERN_DURATION_MS) {
		changePattern(1);
	}

	if (static_cast<int32_t>(now - nextFrameAt) >= 0) {
		nextFrameAt = now + patterns.drawFrame();
	}
}
