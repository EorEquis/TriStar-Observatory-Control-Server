

// Function to initiate the wx JSON request
void startWxJSONRequest(const char* host, const char* path) {
  wxRequest.client.stop(); // Close any previous connection
  wxRequest.host = host;
  wxRequest.path = path;
  wxRequest.requestInProgress = true;
  wxRequest.startTime = millis();

  // Connect to the server and send the HTTP GET request
  if (wxRequest.client.connect(host, 80)) {
    wxRequest.client.print(F("GET "));
    wxRequest.client.print(path);
    wxRequest.client.println(F(" HTTP/1.0"));
    wxRequest.client.print(F("Host: "));
    wxRequest.client.println(host);
    wxRequest.client.println(F("Connection: close"));
    wxRequest.client.println();
  } else {
    wxRequest.requestInProgress = false; // Connection failed
  }
}

// Function to process the wx JSON response
DynamicJsonDocument processWxJSONResponse() {
  DynamicJsonDocument doc(512); // Adjust size as needed

  if (wxRequest.requestInProgress && wxRequest.client.available()) {
    // Read and ignore HTTP headers
    wxRequest.responseInProgress = true;
    while (wxRequest.client.available()) {
      String line = wxRequest.client.readStringUntil('\n');
      line.trim();
      if (line.equals("")) { // Empty line indicates end of headers
        break;
      }
    }
    // Parse the JSON object
    DeserializationError error = deserializeJson(doc, wxRequest.client);
    if (error) {
      // TODO Handle deserialization error
    }
    else {
      calcScore=true;
    }

    wxRequest.requestInProgress = false;
    wxRequest.client.stop();
  } else if (millis() - wxRequest.startTime > 10000) { // 10-second timeout
    wxRequest.requestInProgress = false;
    wxRequest.client.stop();
  }
  return doc;
}

// Function to initiate the AI JSON request
void startAIJSONRequest(const char* host, const char* path) {
  aiRequest.client.stop(); // Close any previous connection
  aiRequest.host = host;
  aiRequest.path = path;
  aiRequest.requestInProgress = true;
  aiRequest.startTime = millis();

  // Connect to the server and send the HTTP GET request
  if (aiRequest.client.connect(host, 80)) {
    aiRequest.client.print(F("GET "));
    aiRequest.client.print(path);
    aiRequest.client.println(F(" HTTP/1.0"));
    aiRequest.client.print(F("Host: "));
    aiRequest.client.println(host);
    aiRequest.client.println(F("Connection: close"));
    aiRequest.client.println();
  } else {
    aiRequest.requestInProgress = false; // Connection failed
  }
}

// Function to process the AI JSON response
DynamicJsonDocument processAIJSONResponse() {
  DynamicJsonDocument doc(512); // Adjust size as needed

  if (aiRequest.requestInProgress && aiRequest.client.available()) {
    // Read and ignore HTTP headers
    while (aiRequest.client.available()) {
      String line = aiRequest.client.readStringUntil('\n');
      line.trim();
      if (line.equals("")) { // Empty line indicates end of headers
        break;
      }
    }

    // Parse the JSON object
    DeserializationError error = deserializeJson(doc, aiRequest.client);
    if (error) {
      // TODO Handle deserialization error
    }
    else {
      calcScore=true;
    }
    aiRequest.requestInProgress = false;
    aiRequest.client.stop();
  } else if (millis() - aiRequest.startTime > 10000) { // 10-second timeout
    aiRequest.requestInProgress = false;
    aiRequest.client.stop();
  }
  return doc;
}

// Function to initiate the UPS JSON request
void startUpsJSONRequest(const char* host, const char* path, const char* port) {
  upsRequest.client.stop(); // Close any previous connection
  upsRequest.host = host;
  upsRequest.path = path;
  upsRequest.requestInProgress = true;
  upsRequest.startTime = millis();

  // Connect to the server and send the HTTP GET request
  if (upsRequest.client.connect(host, 4242)) {
    upsRequest.client.print(F("GET "));
    upsRequest.client.print(path);
    upsRequest.client.println(F(" HTTP/1.0"));
    upsRequest.client.print(F("Host: "));
    upsRequest.client.println(host);
    upsRequest.client.println(F("Connection: close"));
    upsRequest.client.println();
  } else {
    upsRequest.requestInProgress = false; // Connection failed
  }
}

// Function to process the UPS JSON response
DynamicJsonDocument processUpsJSONResponse() {
  DynamicJsonDocument doc(512); // Adjust size as needed

  if (upsRequest.requestInProgress && upsRequest.client.available()) {
    // Read and ignore HTTP headers
    while (upsRequest.client.available()) {
      String line = upsRequest.client.readStringUntil('\n');
      line.trim();
      if (line.equals("")) { // Empty line indicates end of headers
        break;
      }
    }

    // Parse the JSON object
    DeserializationError error = deserializeJson(doc, upsRequest.client);
    if (error) {
      // TODO Handle deserialization error
    }
    else {
      calcScore=true;
    }
    upsRequest.requestInProgress = false;
    upsRequest.client.stop();
  } else if (millis() - upsRequest.startTime > 10000) { // 10-second timeout
    upsRequest.requestInProgress = false;
    upsRequest.client.stop();
  }
  return doc;
}

//DynamicJsonDocument readJSON(const char * Host, const char * Path) {
//  // Connect to HTTP server
//  eth.setTimeout(10000);
//  if (!eth.connect(Host, 80)) {
//    Serial.println(F("Connection failed"));
//    return;
//  }
//
//  Serial.print(F("Connected to "));
//  Serial.println(Host);
//
//  // Send HTTP request
//  eth.print(F("GET "));
//  delay(3);         // Appears we were overflowing the send buffer.  A serial.print "fixed" it, so trying a brief delay.
//  eth.print(Path);
//  delay(3);
//  eth.println(F(" HTTP/1.0"));
//  delay(3);
//  eth.print(F("Host: "));
//  delay(3);
//  eth.println(Host);
//  delay(3);
//  eth.println(F("Connection: close"));
//  if (eth.println() == 0) {
//    Serial.println(F("Failed to send request"));
//    eth.stop();
//    return;
//  }
//
//  // Check HTTP status
//  char status[32] = {0};
//  eth.readBytesUntil('\r', status, sizeof(status));
//  if (strcmp(status, "HTTP/1.1 200 OK") != 0 && strcmp(status, "HTTP/1.0 200 OK") != 0) {
//    Serial.print(F("Unexpected response: "));
//    Serial.println(status);
//    eth.stop();
//    return;
//  }
//
//  // Skip HTTP headers
//  char endOfHeaders[] = "\r\n\r\n";
//  if (!eth.find(endOfHeaders)) {
//    Serial.println(F("Invalid response"));
//    eth.stop();
//    return;
//  }
//
//  // Allocate the JSON document
//  // Use arduinojson.org/v6/assistant to compute the capacity.
//  const size_t capacity = 512;
//  DynamicJsonDocument doc(capacity);
//
//  // Parse JSON object
//  DeserializationError error = deserializeJson(doc, eth);
//  if (error) {
//    Serial.print(F("deserializeJson() failed: "));
//    Serial.println(error.f_str());
//    eth.stop();
//    return;
//  }
//  return doc;
//}
