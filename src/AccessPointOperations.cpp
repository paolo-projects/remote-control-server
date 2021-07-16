#include "AccessPointOperations.h"

AccessPointOperations::AccessPointOperations(
    Configuration &configuration, StateManager &stateManager,
    AccessPointSettings settings)
    : configuration(configuration), stateManager(stateManager),
      settings(settings), authHandler(settings.AUTH_USER, settings.AUTH_PASS, settings.TIMEOUT_MS),
      serverCert(settings.CERTIFICATE), privateKey(settings.PRIVATE_KEY)
{
    actionParser.with("setwifi", CALLBACK(AccessPointOperations, setWifiPassword));
}

void AccessPointOperations::setOnServerLoopCallback(std::function<void(void)> callback)
{
    onServerLoopCallback = callback;
}

void AccessPointOperations::startServer()
{
    BearSSL::WiFiServerSecure server(settings.PORT);

    server.setRSACert(&serverCert, &privateKey);

    server.begin(settings.PORT);

    while (serverRunning)
    {
        BearSSL::WiFiClientSecure incoming = server.available();
        if (incoming)
        {
            Log::printfln("Connection received from %s", incoming.remoteIP().toString().c_str());

            int timeout = millis();
            while (!incoming.available() && millis() - timeout < settings.TIMEOUT_MS)
                delay(50);

            if (incoming.available())
            {
                if (authHandler.authenticate(incoming))
                {
                    // Wait for new transmission
                    timeout = millis();
                    while (!incoming.available() && millis() - timeout < settings.TIMEOUT_MS)
                        delay(50);

                    ActionMap action = ActionMap::fromStream(incoming, settings.TIMEOUT_MS);

                    if (actionParser.execute(action, incoming))
                    {
                        serverRunning = false;
                        stateManager.setState(CONNECTING);
                    }
                }
                else
                {
                    Log::println("Bad authentication");
                }

                Log::println("Closing connection");
                incoming.stop();
            }
        }
        if (onServerLoopCallback.hasValue())
        {
            onServerLoopCallback.get()();
        }
        delay(20);
    }

    server.stop();
}

bool AccessPointOperations::setWifiPassword(ActionMap &action, Stream &output)
{
    if (action.has("bssid") && action.has("password"))
    {
        configuration.updateConfig(*action.get("bssid"), *action.get("password"));

        Response::successResponse().write(output);

        return true;
    }

    return false;
}