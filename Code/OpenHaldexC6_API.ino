#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

// use same server for API
static AsyncWebServer *api = nullptr;

void setupAPI();

// helpers
static void sendJson(AsyncWebServerRequest *request, int code, const JsonDocument &doc) {
  String out;
  serializeJson(doc, out);
  request->send(code, "application/json", out);
}

static void sendError(AsyncWebServerRequest *request, int code, const char *msg) {
  StaticJsonDocument<128> doc;
  doc["error"] = msg;
  sendJson(request, code, doc);
}

static void onJsonBody(AsyncWebServerRequest *request,
                      uint8_t *data, size_t len, size_t index, size_t total,
                      void (*done)(AsyncWebServerRequest *, const String &)) {
  // using request->_tempObject to store heap string across chunks
  if (index == 0) {
    request->_tempObject = new String();
    ((String *)request->_tempObject)->reserve(total);
  }

  String *body = (String *)request->_tempObject;
  body->concat((const char *)data, len);

  // Last chunk?
  if (index + len == total) {
    String full = *body;
    delete body;
    request->_tempObject = nullptr;

    done(request, full);
  }
}

// --------------------
// Handlers
// --------------------
static void handleStatus(AsyncWebServerRequest *request) {
  StaticJsonDocument<1024> doc;

  doc["mode"] = get_openhaldex_mode_string(state.mode);
  doc["haldexGeneration"] = haldexGeneration;

  doc["disableThrottle"] = disableThrottle;
  doc["disableSpeed"] = disableSpeed;

  doc["disableController"] = disableController;
  doc["broadcastOpenHaldexOverCAN"] = broadcastOpenHaldexOverCAN;

  doc["isStandalone"] = isStandalone;
  doc["followBrake"] = followBrake;
  doc["invertBrake"] = invertBrake;
  doc["followHandbrake"] = followHandbrake;
  doc["invertHandbrake"] = invertHandbrake;

  JsonObject can = doc.createNestedObject("can");
  can["chassis"] = hasCANChassis;
  can["haldex"] = hasCANHaldex;
  can["busFailure"] = isBusFailure;

  JsonObject telemetry = doc.createNestedObject("telemetry");
  telemetry["speed"] = received_vehicle_speed;
  telemetry["rpm"] = received_vehicle_rpm;
  telemetry["boost"] = received_vehicle_boost;
  telemetry["haldexState"] = received_haldex_state;
  telemetry["locking"] = appliedTorque;
  telemetry["haldexEngagement"] = received_haldex_engagement;

  sendJson(request, 200, doc);
}

static void applyStandaloneTasks(bool standalone) {
  // Guard handles in case tasks aren’t created yet
  if (!handle_frames10 || !handle_frames20 || !handle_frames25 || !handle_frames100 ||
      !handle_frames200 || !handle_frames1000) {
    return;
  }

  if (!standalone) {
    vTaskSuspend(handle_frames1000);
    vTaskSuspend(handle_frames200);
    vTaskSuspend(handle_frames100);
    vTaskSuspend(handle_frames25);
    vTaskSuspend(handle_frames20);
    vTaskSuspend(handle_frames10);
  } else {
    vTaskResume(handle_frames1000);
    vTaskResume(handle_frames200);
    vTaskResume(handle_frames100);
    vTaskResume(handle_frames25);
    vTaskResume(handle_frames20);
    vTaskResume(handle_frames10);
  }
}

static void handleModeJson(AsyncWebServerRequest *request, const String &body) {
  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, body) != DeserializationError::Ok) {
    sendError(request, 400, "invalid json");
    return;
  }

  String mode = doc["mode"] | "";
  mode.toUpperCase();

  if (mode == "STOCK") state.mode = MODE_STOCK;
  else if (mode == "FWD") state.mode = MODE_FWD;
  else if (mode == "5050") state.mode = MODE_5050;
  else if (mode == "6040") state.mode = MODE_6040;
  else if (mode == "7525") state.mode = MODE_7525;
  else if (mode == "CUSTOM") state.mode = MODE_CUSTOM;
  else {
    sendError(request, 400, "invalid mode");
    return;
  }

  StaticJsonDocument<128> resp;
  resp["ok"] = true;
  resp["mode"] = get_openhaldex_mode_string(state.mode);
  sendJson(request, 200, resp);
}

static void handleSettingsJson(AsyncWebServerRequest *request, const String &body) {
  StaticJsonDocument<512> doc;
  if (deserializeJson(doc, body) != DeserializationError::Ok) {
    sendError(request, 400, "invalid json");
    return;
  }

  if (doc.containsKey("disableThrottle")) {
    int v = doc["disableThrottle"];
    if (v < 0) v = 0;
    if (v > 100) v = 100;
    disableThrottle = (uint8_t)v;
    state.pedal_threshold = disableThrottle;
  }

  if (doc.containsKey("disableSpeed")) {
    int v = doc["disableSpeed"];
    if (v < 0) v = 0;
    if (v > 300) v = 300;
    disableSpeed = (uint8_t)v;
  }

  if (doc.containsKey("haldexGeneration")) {
    int g = doc["haldexGeneration"];
    if (g == 1 || g == 2 || g == 4) {
      haldexGeneration = (uint8_t)g;
    }
  }

  if (doc.containsKey("disableController"))
    disableController = (bool)doc["disableController"];

  if (doc.containsKey("broadcastOpenHaldexOverCAN"))
    broadcastOpenHaldexOverCAN = (bool)doc["broadcastOpenHaldexOverCAN"];

  if (doc.containsKey("followBrake"))
    followBrake = (bool)doc["followBrake"];

  if (doc.containsKey("invertBrake"))
    invertBrake = (bool)doc["invertBrake"];

  if (doc.containsKey("followHandbrake"))
    followHandbrake = (bool)doc["followHandbrake"];

  if (doc.containsKey("invertHandbrake"))
    invertHandbrake = (bool)doc["invertHandbrake"];

  if (doc.containsKey("isStandalone")) {
    isStandalone = (bool)doc["isStandalone"];
    applyStandaloneTasks(isStandalone);
  }

  StaticJsonDocument<128> resp;
  resp["ok"] = true;
  sendJson(request, 200, resp);
}


void setupAPI() {
  // ESPUI provides an accessor for AsyncWebServer
  api = ESPUI.WebServer();

  // GET status
  api->on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleStatus(request);
  });

  // POST mode (JSON body)
  api->on("/api/mode", HTTP_POST,
          [](AsyncWebServerRequest *request) {
            // send response in body handler
          },
          nullptr,
          [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            onJsonBody(request, data, len, index, total, handleModeJson);
          });

  // POST settings (JSON body)
  api->on("/api/settings", HTTP_POST,
          [](AsyncWebServerRequest *request) {
            // same as above, sent in body handler
          },
          nullptr,
          [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            onJsonBody(request, data, len, index, total, handleSettingsJson);
          });
}