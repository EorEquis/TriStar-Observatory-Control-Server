// readJSON function to retrieve JSON payload from specified address
  DynamicJsonDocument readJSON(const char * Host, const char * Path) {
    // Connect to HTTP server
    eth.setTimeout(10000);
    if (!eth.connect(Host, 80)) {
      Serial.println(F("Connection failed"));
      return;
    }
  
    Serial.print(F("Connected to "));
    Serial.println(Host);
  
    // Send HTTP request
    eth.print(F("GET "));
    delay(3);         // Appears we were overflowing the send buffer.  A serial.print "fixed" it, so trying a brief delay.
    eth.print(Path);
    delay(3);
    eth.println(F(" HTTP/1.0"));
    delay(3);
    eth.print(F("Host: "));
    delay(3);
    eth.println(Host);
    delay(3);
    eth.println(F("Connection: close"));
    if (eth.println() == 0) {
      Serial.println(F("Failed to send request"));
      eth.stop();
      return;
    }
  
    // Check HTTP status
    char status[32] = {0};
    eth.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0 && strcmp(status, "HTTP/1.0 200 OK") != 0) {
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
    const size_t capacity = 195;
    DynamicJsonDocument doc(capacity);
  
    // Parse JSON object
    DeserializationError error = deserializeJson(doc, eth);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      eth.stop();
      return;
    }
    strWX = "";
    serializeJson(doc, strWX);
    return doc;
  } // END readJSON()
