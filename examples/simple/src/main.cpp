#include "RemoteControlServer.h"

RemoteControlSettings serverSetup();
bool setPinAction(ActionMap &action, Stream &output);

RemoteControlServer<5> server(serverSetup());

void setup()
{
    Serial.begin(9600);
    // ...
    server.addAction("setpin", setPinAction);
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
    settings.ACCESS_POINT_SETTINGS.WIFI_AP_PASS = "MyPassword";
    settings.ACCESS_POINT_SETTINGS.WIFI_AP_HIDDEN = false;
    settings.ACCESS_POINT_SETTINGS.WIFI_AP_CHANNEL = 11;
    settings.ACCESS_POINT_SETTINGS.WIFI_AP_IP_ADDRESS = IPAddress(192, 168, 7, 5);
    settings.ACCESS_POINT_SETTINGS.WIFI_AP_IP_GATEWAY = IPAddress(192, 168, 7, 1);
    settings.ACCESS_POINT_SETTINGS.WIFI_AP_SUBNET = IPAddress(255, 255, 255, 0);
    settings.ACCESS_POINT_SETTINGS.WIFI_AP_MAX_CONN = 5;
    settings.ACCESS_POINT_SETTINGS.AUTH_USER = "admin";
    settings.ACCESS_POINT_SETTINGS.AUTH_PASS = "password";
    settings.ACCESS_POINT_SETTINGS.PORT = 12345;
    settings.ACCESS_POINT_SETTINGS.TIMEOUT_MS = 5000;
    settings.ACCESS_POINT_SETTINGS.CERTIFICATE = "";
    settings.ACCESS_POINT_SETTINGS.PRIVATE_KEY = "";

    settings.COMMAND_SERVER_SETTINGS.HOSTNAME = "myserver";
    settings.COMMAND_SERVER_SETTINGS.PORT = 54321;
    settings.COMMAND_SERVER_SETTINGS.AUTH_USERNAME = "admin";
    settings.COMMAND_SERVER_SETTINGS.AUTH_PASSWORD = "password";
    settings.COMMAND_SERVER_SETTINGS.UDP_PACKET = "abcdefghi123456789";
    settings.COMMAND_SERVER_SETTINGS.UDP_PACKET_SIZE = 18;
    settings.COMMAND_SERVER_SETTINGS.UDP_PORT = 4747;
    settings.COMMAND_SERVER_SETTINGS.UDP_RATE_MS = 5000;
    settings.COMMAND_SERVER_SETTINGS.TIMEOUT_MS = 5000;
    settings.COMMAND_SERVER_SETTINGS.CERTIFICATE = "";
    settings.COMMAND_SERVER_SETTINGS.PRIVATE_KEY = "";

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