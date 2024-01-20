

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
