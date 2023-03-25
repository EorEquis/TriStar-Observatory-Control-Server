DynamicJsonDocument readAllSkyAI(const char * AIHost, const char * AIPath) {
  // Connect to HTTP server
  eth.setTimeout(10000);
  if (!eth.connect(AIHost, 80)) {
    Serial.println(F("Connection failed"));
    return;
  }

  Serial.println(F("Connected!"));

  // Send HTTP request
  eth.print(F("GET "));
  eth.print(AIPath);
  eth.println(F(" HTTP/1.0"));
  eth.print(F("Host: "));
  eth.println(AIHost);
  eth.println(F("Connection: close"));
  if (eth.println() == 0) {
    Serial.println(F("Failed to send request"));
    eth                .stop();
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  eth.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    eth.stop();
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!eth.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    eth.stop();
    return;
  }

  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = 175;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, eth);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    eth.stop();
    return;
  }

/*
  // Extract values
  Serial.println(F("Response:"));
  Serial.println(doc["classification"].as<const char*>());
  Serial.println(doc["confidence"].as<float>(), 6);
  Serial.println(doc["utc"].as<unsigned long>());
  Serial.println(doc["img"].as<const char*>());
*/
  // Disconnect
  eth.stop();
  return doc;
  }


DynamicJsonDocument readWX(const char * wxHost, const char * wxPath) {
  // Connect to HTTP server
  eth.setTimeout(10000);
  if (!eth.connect(wxHost, 80)) {
    Serial.println(F("Connection failed"));
    return;
  }

  Serial.println(F("Connected!"));

  // Send HTTP request
  eth.print(F("GET "));
  eth.print(wxPath);
  eth.println(F(" HTTP/1.0"));
  eth.print(F("Host: "));
  eth.println(wxHost);
  eth.println(F("Connection: close"));
  if (eth.println() == 0) {
    Serial.println(F("Failed to send request"));
    eth                .stop();
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  eth.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.0 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    eth.stop();
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!eth.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    eth.stop();
    return;
  }

  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = 250;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, eth);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    eth.stop();
    return;
  }

/*
  // Extract values
  Serial.println(F("Response:"));
  Serial.println(doc["classification"].as<const char*>());
  Serial.println(doc["confidence"].as<float>(), 6);
  Serial.println(doc["utc"].as<unsigned long>());
  Serial.println(doc["img"].as<const char*>());
*/
  // Disconnect
  eth.stop();
  return doc;
  }  
