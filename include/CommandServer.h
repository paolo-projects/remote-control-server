#ifndef COMMAND_SERVER_H
#define COMMAND_SERVER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <functional>
#include "Optional.h"
#include "Configuration.h"
#include "StateManager.h"
#include "AuthenticationHandler.h"
#include "ActionParser.h"
#include "SerialMap.h"
#include "Common.h"
#include "Response.h"
#include "RemoteControlSettings.h"
#include "Logging.h"

/**
 * @brief The main command-receiving server
 * 
 * @tparam N The maximum number of actions accepted
 */
template <int N>
class CommandServer
{
public:
  CommandServer(StateManager &stateManager, CommandServerSettings settings) : stateManager(stateManager), settings(settings), authHandler(settings.AUTH_USERNAME, settings.AUTH_PASSWORD, settings.TIMEOUT_MS),
                                                                              serverCert(settings.CERTIFICATE), privateKey(settings.PRIVATE_KEY) {}
  /**
   * @brief Start the server
   */
  void startServer()
  {
    BearSSL::WiFiServerSecure server(settings.PORT);

    server.setRSACert(&serverCert, &privateKey);

    server.begin();
    udp.begin(settings.UDP_RATE_MS);

    broadcastIp = WiFi.localIP();
    broadcastIp[3] = 255;

    while (serverRunning)
    {
      auto client = server.available();

      if (client)
      {
        Log::printfln("Connection received from %s", client.remoteIP().toString().c_str());

        if (callbacks.onNewConnection.hasValue())
        {
          callbacks.onNewConnection.get()(client.remoteIP().toString(), client.remotePort());
        }

        if (authHandler.authenticate(client))
        {
          Log::println("Authentication OK");
          ActionMap action = ActionMap::fromStream(client, settings.TIMEOUT_MS);

          if (actionParser.execute(action, client))
          {
            stateManager.setState(AP_MODE);
            serverRunning = false;

            if (callbacks.onServerTermination.hasValue())
            {
              callbacks.onServerTermination.get()();
            }
          }
        }
        else
        {
          Log::println("Authentication failed");
        }

        if (callbacks.onConnectionClose.hasValue())
        {
          callbacks.onConnectionClose.get()();
        }

        client.stop();
      }

      if (callbacks.onServerLoop.hasValue())
      {
        callbacks.onServerLoop.get()();
      }
      udpBroadcast();
      delay(20);
    }
    server.stop();
  }
  /**
   * @brief Register an action into the server. In other words when the action `name` is sent, the `action`
   *  callback is invoked, with the action itself and the socket stream as parameters. If the callback returns
   * true, the server is shut-down, otherwise it won't.
   * 
   * @param name The action name
   * @param callback The action callback
   */
  void registerAction(const String &name, std::function<bool(ActionMap &, Stream &)> callback)
  {
    actionParser.with(name, callback);
  }
  /**
   * @brief Set a callback to be executed when a new connection is accepted
   * 
   * @param callback The callback, to which will be passed the client IP address and port
   */
  void setOnNewConnectionCallback(std::function<void(const String &, int)> callback)
  {
    callbacks.onNewConnection = callback;
  }
  /**
   * @brief Set a callback to be executed right before a connection is closed
   * 
   * @param callback The callback
   */
  void setOnConnectionCloseCallback(std::function<void(void)> callback)
  {
    callbacks.onConnectionClose = callback;
  }
  /**
   * @brief Set a callback to be executed at every server cycle loop. This is useful to do extra
   *  stuff as if it was executed in the `loop()` function
   * 
   * @param callback The callback
   */
  void setOnServerLoopCallback(std::function<void(void)> callback)
  {
    callbacks.onServerLoop = callback;
  }
  /**
   * @brief Set a callback to be executed when the server is terminated
   * 
   * @param callback The callback
   */
  void setOnServerTerminationCallback(std::function<void(void)> callback)
  {
    callbacks.onServerTermination = callback;
  }

private:
  struct CALLBACKS
  {
    Optional<std::function<void(const String &, int)>> onNewConnection;
    Optional<std::function<void(void)>> onConnectionClose;
    Optional<std::function<void(void)>> onServerLoop;
    Optional<std::function<void(void)>> onServerTermination;
  };

  StateManager &stateManager;
  CommandServerSettings settings;
  AuthenticationHandler authHandler;
  ActionParser<N> actionParser;
  BearSSL::X509List serverCert;
  BearSSL::PrivateKey privateKey;
  CALLBACKS callbacks = {{}, {}, {}};

  void udpBroadcast()
  {
    if (lastUdpBroadcast == 0 || millis() > lastUdpBroadcast + settings.UDP_RATE_MS)
    {
      udp.beginPacket(broadcastIp, settings.UDP_PORT);
      udp.write(settings.UDP_PACKET, settings.UDP_PACKET_SIZE);
      udp.endPacket();
      lastUdpBroadcast = millis();
    }
  }

  unsigned long lastUdpBroadcast = 0;
  WiFiUDP udp;
  IPAddress broadcastIp;
  bool serverRunning = true;
  char outBuffer[512] = {};
};

#endif
