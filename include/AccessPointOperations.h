#ifndef ACCESS_POINT_OPERATIONS_H
#define ACCESS_POINT_OPERATIONS_H

#include <Arduino.h>
#include <functional>
#include <ESP8266WiFi.h>
#include "AuthenticationHandler.h"
#include "Configuration.h"
#include "StateManager.h"
#include "ActionParser.h"
#include "SerialMap.h"
#include "Common.h"
#include "Optional.h"
#include "RemoteControlSettings.h"

class AccessPointOperations
{
public:
  AccessPointOperations(Configuration &configuration,
                        StateManager &stateManager,
                        AccessPointSettings settings);
  void startServer();
  /**
   * @brief Set a callback to be executed in loop while the AP server is awaiting for connections
   * 
   * @param callback 
   */
  void setOnServerLoopCallback(std::function<void(void)> callback);

private:
  Configuration &configuration;
  StateManager &stateManager;
  AccessPointSettings settings;
  AuthenticationHandler authHandler;
  ActionParser<10> actionParser;
  BearSSL::X509List serverCert;
  BearSSL::PrivateKey privateKey;
  Optional<std::function<void(void)>> onServerLoopCallback;

  bool serverRunning = true;

  bool setWifiPassword(ActionMap &action, Stream &output);
};

#endif
