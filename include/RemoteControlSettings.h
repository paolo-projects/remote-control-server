#ifndef REMOTECONTROLSETTINGS_H
#define REMOTECONTROLSETTINGS_H

#include <ESP8266WiFi.h>

struct AccessPointSettings
{
    /** @brief The name of the Access-Point WiFi network */
    const char *WIFI_AP_SSID;
    /** @brief The password of the Access-Point WiFi network */
    const char *WIFI_AP_PASS;
    /** @brief Whether the Access Point shall be hidden */
    bool WIFI_AP_HIDDEN;
    /** @brief The channel of the WiFi Access Point */
    int WIFI_AP_CHANNEL;
    /** @brief The max connections the WiFi Access Point can accept */
    int WIFI_AP_MAX_CONN;
    /** @brief The IP address of the server in AP mode */
    IPAddress WIFI_AP_IP_ADDRESS;
    /** @brief The IP address of the gateway in AP mode */
    IPAddress WIFI_AP_IP_GATEWAY;
    /** @brief The subnet mask of the local network in AP mode */
    IPAddress WIFI_AP_SUBNET;
    /** @brief The authentication username */
    const char *AUTH_USER;
    /** @brief The authentication password */
    const char *AUTH_PASS;
    /** @brief The server port */
    int PORT;
    /** @brief The connection timeout */
    int TIMEOUT_MS;
    /** @brief The server certificate in PEM format */
    const char *CERTIFICATE;
    /** @brief The server private key in PEM format */
    const char *PRIVATE_KEY;
};

struct CommandServerSettings
{
    /** @brief The hostname of the server in the Local Network */
    const char *HOSTNAME;
    /** @brief The server port */
    int PORT;
    /** @brief The username for the authentication */
    const char *AUTH_USERNAME;
    /** @brief The password for the authentication */
    const char *AUTH_PASSWORD;
    /** @brief The UDP broadcast port */
    int UDP_PORT;
    /** @brief The UDP packet to transmit */
    const char *UDP_PACKET;
    /** @brief The  UDP packet size */
    size_t UDP_PACKET_SIZE;
    /** @brief The rate at which to transmit the UDP packet */
    int UDP_RATE_MS;
    /** @brief The server certificate in PEM format */
    const char *CERTIFICATE;
    /** @brief The server private key in PEM format */
    const char *PRIVATE_KEY;
    /** @brief The connection timeout */
    int TIMEOUT_MS;
    /** @brief The timeout after which the device will stop trying to connect to the Wifi
     *  and switch to AP mode instead
     * */
    int WIFI_TIMEOUT_S;
};

struct RemoteControlSettings
{
    AccessPointSettings ACCESS_POINT_SETTINGS;
    CommandServerSettings COMMAND_SERVER_SETTINGS;
};

#endif // REMOTECONTROLSETTINGS_H