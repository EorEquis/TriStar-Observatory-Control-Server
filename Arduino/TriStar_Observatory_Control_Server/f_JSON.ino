DynamicJsonDocument readJSON(const char * Host, const char * Path) {
  static DynamicJsonDocument doc(512); // Adjust the capacity as needed
  static size_t bytesRead = 0;
  
  char url[strlen(Host) + strlen(Path) + 8];
  strcpy(url, "http://");
  strcat(url, Host);
  strcat(url, Path);

  #ifdef DEBUG
    Serial.print("Constructed URL: ");
    Serial.println(url);
  #endif
  
  switch (httpState) {
    case HttpState::Idle:
      // Initiate the asynchronous HTTP request
      int httpCode = httpClient.get(url);
      httpState = HttpState::SendingRequest;
      #ifdef DEBUG
        Serial.print("httpCode is ");
        Serial.println(httpCode);
        Serial.println("httpState is presumably SendingRequest");
      #endif    
      break;

    case HttpState::SendingRequest:
      // Check if the request has been sent
      if (httpClient.available()) {
        httpState = HttpState::ReceivingResponse;
      }
      break;

    case HttpState::ReceivingResponse:
      // Read the HTTP response data if available
      while (httpClient.available()) {
        char c = httpClient.read();
        if (bytesRead < sizeof(responseBuffer) - 1) {
          responseBuffer[bytesRead++] = c;
        }
      }

      // Check if the response is complete
      if (httpClient.responseStatusCode() == 0) {
        // Incomplete response, continue receiving
      } else {
        // Complete response received, parse it
        DeserializationError error = deserializeJson(doc, responseBuffer);
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
        }
        // Reset the HTTP state
        httpState = HttpState::Done;
        bytesRead = 0;
      }
      break;

    case HttpState::Done:
      // Reset the HTTP state for the next request
      httpState = HttpState::Idle;
      break;
  }

  return doc;
}
