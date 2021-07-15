#include "RemoteControlServer.h"

RemoteControlSettings serverSetup();
bool setPinAction(ActionMap &action, Stream &output);

RemoteControlServer<5> server(serverSetup());

void setup()
{
    Serial.begin(9600);
    // ...
    server.addAction("setpin", setPinAction)
}

void loop()
{
    // ...
    server.execute();
}

RemoteControlSettings serverSetup()
{
    RemoteControlSettings settings;
    // define the settings
    settings.ACCESS_POINT_SETTINGS.WIFI_AP_SSID = "MyWifi";
    settings.ACCESS_POINT_SETTINGS.WIFI_AP_PASS = "myv3rystr0ngp4ssw0rd";
    settings.ACCESS_POINT_SETTINGS.WIFI_AP_HIDDEN = false;
    settings.ACCESS_POINT_SETTINGS.WIFI_AP_CHANNEL = 11;
    // ...
    settings.COMMAND_SERVER_SETTINGS.HOSTNAME = "myserver";
    settings.COMMAND_SERVER_SETTINGS.PORT = 12345;
    settings.COMMAND_SERVER_SETTINGS.AUTH_USERNAME = "johnwick";
    settings.COMMAND_SERVER_SETTINGS.AUTH_PASSWORD = "you'lln3v3rgu3ssth1sp4ss";
    // ...
    return settings;
}

bool setPinAction(ActionMap &action, Stream &output)
{
    if (action.has("pin") && action.has("level"))
    {
        digitalWrite((*action.get("pin")).toInt(), (*action.get("level")) == "high" ? HIGH : LOW);
    }
    return false;
}