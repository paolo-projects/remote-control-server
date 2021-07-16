#ifndef REMOTECONTROLSERVER_H
#define REMOTECONTROLSERVER_H

#include <Arduino.h>
#include <functional>
#include "RemoteControlSettings.h"
#include "AccessPointOperations.h"
#include "CommandServer.h"
#include "Configuration.h"
#include "StateManager.h"
#include "Logging.h"

typedef std::function<bool(ActionMap &, Stream &)> ActionCallback;

/**
 * @brief The Remote control server class
 * 
 * @tparam N The maximum number of actions handled by the server
 */
template <int N>
class RemoteControlServer
{
public:
    RemoteControlServer() = delete;
    RemoteControlServer(const RemoteControlServer &c) = delete;
    RemoteControlServer(const RemoteControlServer &&m) = delete;
    RemoteControlServer(RemoteControlSettings settings) : settings(settings), apOps(configuration, stateManager, settings.ACCESS_POINT_SETTINGS),
                                                          commandServer(stateManager, settings.COMMAND_SERVER_SETTINGS)
    {
        stateManager.registerStateFunction(CONNECTING, std::bind(this, &RemoteControlServer::connectingCallback));
        stateManager.registerStateFunction(CONNECTED, std::bind(this, &RemoteControlServer::connectedCallback));
        stateManager.registerStateFunction(AP_MODE, std::bind(this, &RemoteControlServer::apModeCallback));
    }
    /**
     * @brief Set a callback to be executed when a new connection is accepted from the main server (not the AP one)
     * 
     * @param callback The callback, to which will be passed the client IP address and port
     */
    void setOnClientConnectionCallback(std::function<void(const String &, int)> callback)
    {
        commandServer.setOnNewConnectionCallback(callback);
    }
    /**
     * @brief Set a callback to be executed right before a connection is closed in the main server (not the AP one)
     * 
     * @param callback The callback
     */
    void setOnConnectionCloseCallback(std::function<void(void)> callback)
    {
        commandServer.setOnConnectionCloseCallback(callback);
    }
    /**
     * @brief et a callback to be executed when the main server is terminated
     * 
     * @param callback The callback
     */
    void setOnServerTerminationCallback(std::function<void(void)> callback)
    {
        commandServer.setOnServerTerminationCallback(callback);
    }
    /**
     * @brief Adds an action to be handled by the server, i.e. when the given action is received,
     *  it calls the given callback passing the action object and the client socket stream
     * 
     * @param name The action name, sent by the client in the "action" field of the map
     * @param callback The callback, called with the action object and the client socket stream as arguments
     */
    void addAction(const String &name, ActionCallback callback)
    {
        commandServer.registerAction(name, callback);
    }

    /**
     * @brief Execute the current server state. It's the core function of the server, has to be executed in loop
     */
    void execute()
    {
        stateManager.executeState();
    }

private:
    Configuration configuration;
    StateManager stateManager;
    RemoteControlSettings settings;
    AccessPointOperations apOps;
    CommandServer<N> commandServer;
    unsigned long lastButtonPress = millis();

    bool connectToWlan()
    {
        WiFi.mode(WIFI_STA);
        WiFi.hostname(settings.COMMAND_SERVER_SETTINGS.WIFI_HOSTNAME);
        WiFi.begin(configuration.getBSSID(), configuration.getPass());
        Log.println("Connecting...");
        uint16_t timeout = settings.COMMAND_SERVER_SETTINGS.TIMEOUT_MS;
        while (WiFi.status() != WL_CONNECTED)
        {
            if (timeout <= 0)
            {
                Log.println("");
                Log.println("Connection Timeout!");
                return false;
            }
            // Divide 1s delay in 10 iterations of 100ms to allow manual overriding through button
            for (uint8_t i = 0; i < 10; i++)
            {
                delay(100);
            }
            timeout--;
            Log.print(".");
        }
        Log.println("");
        Log.printfln("Successfully connected to: %s, with IP: %s", configuration.getBSSID().c_str(), WiFi.localIP().toString().c_str());
        return true;
    }

    bool startAccessPoint()
    {
        WiFi.mode(WIFI_AP);
        Log.println("Starting AP mode...");
        bool result = WiFi.softAPConfig(settings.ACCESS_POINT_SETTINGS.WIFI_AP_IP_ADDRESS,
                                        settings.ACCESS_POINT_SETTINGS.WIFI_AP_IP_GATEWAY, settings.ACCESS_POINT_SETTINGS.WIFI_AP_SUBNET);
        if (!result)
        {
            Log.println("Error setting AP configuration");
            return false;
        }
        result = WiFi.softAP(settings.ACCESS_POINT_SETTINGS.WIFI_AP_SSID, settings.ACCESS_POINT_SETTINGS.WIFI_AP_PASS,
                             settings.ACCESS_POINT_SETTINGS.WIFI_AP_CHANNEL, settings.ACCESS_POINT_SETTINGS.WIFI_AP_HIDDEN,
                             settings.ACCESS_POINT_SETTINGS.WIFI_AP_MAX_CONN);
        if (!result)
        {
            Log.println("Error starting soft AP mode");
            return false;
        }
        Log.printfln("Soft AP started with SSID: %s and local IP: %s", settings.ACCESS_POINT_SETTINGS.WIFI_AP_SSID, WiFi.softAPIP().toString().c_str());
        return true;
    }

    void connectingCallback()
    {
        // Search for valid configuration and attempt connecting to the WiFi
        if (configuration.isValid())
        {
            stateManager.setState(CONNECTED);
        }
        else
        {
            // Start AP
            stateManager.setState(AP_MODE);
        }
    }

    void connectedCallback()
    {
        // Start the command server
        if (connectToWlan())
        {
            // Start server
            commandServer.startServer();
        }
        else
        {
            // Start AP
            stateManager.setState(AP_MODE);
        }
    }

    void apModeCallback()
    {
        startAccessPoint();
        accessPoint.startServer();
        delay(150);
    }
};

#endif // REMOTECONTROLSERVER_H
