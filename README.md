# Remote Control Server

A remote control library for the ESP8266. This library has the purpose of letting a user control the device through the local network in a secure way, with the use of TLS encryption and a username/password authentication. The messages are exchanged through a custom protocol based on serialized string maps through the `SerialMap` class.

# Dependencies

The library is build upon a mix of the Arduino library, the ESP library for Socket I/O and the C++ standard library.

The requirements thus are the Arduino framework, the ESP8266 library and support for the C++ standard library up to C++11.

# Features

The main feature of this library is the encryption of the data exchanged between the server and the clients and the authentication, achieved through the use of TLS and a basic user/password combination sent with every request. The encryption algorithms may slow down the client-server communications, although this guarantees a layer of security to avoid unauthorized use of the server.

The communication protocol is based upon null-terminated key/value pairs represented as strings, used in a request/response messages exchange. The client first sends the authentication data, the server then answers with a result message, and if the authentication was successful the client sends an action packet, where it specifies the requested action in the `action` key.

Since the device has to be connected to a local network to operate, it initially won't have any configuration set to connect to a WLAN access point. So it switches to Access Point mode, where a client can connect to the access point WiFi and, after authentication, send the credentials for connecting to the WiFi router.

The device at this point stores the credential in the emulated EEPROM and attempts a connection to the given WiFi station. If the connection is not successful, it switches back to the AP mode. If the connection was successful, it opens a TCP socket on a given port, accepting connections, while periodically broadcasting an UDP packet on the local network to notify clients of the service availability.

# The classes

-   ## RemoteControlServer&lt;N&gt;

    This is the main class used to set up the server and get it running. It has to be instanciated with a `RemoteControlServerSettings` class containing everything needed for the server to be fully operative. This class has to be instanciated ideally in the static section of your source file, and the `execute` method has to be called in the `loop` function.

    The `N` template parameter specifies how many actions will your server, at maximum, handle.

    ```c++
    RemoteControlSettings serverSetup();
    void setPinAction(ActionMap& action, Stream& output);

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

    void setPinAction(ActionMap& action, Stream& output)
    {
        if(action.has("pin") && action.has("level"))
        {
            digitalWrite((*action.get("pin")).toInt(), (*action.get("level")) == "high" ? HIGH : LOW);
        }
        return false;
    }
    ```

    -   **_void_ addAction(_const String_ &name, _ActionCallback_ callback)**

        This method binds the given action to a callback. The return value of the callback is a flag indicating whether to terminate the server and switch to AP mode. For instance, to turn on a LED after the client sends the action `ledon`, you call the method like this:

        ```c++
        server.addAction("setled", [](ActionMap& action, Stream& output) {
            if(action.has("value"))
            {
                digitalWrite(LED_PIN, (*action.get("value")) == "on" ? HIGH : LOW);
            }
            return false;
        });
        ```

        Or if you wanted an action to switch back to AP mode to receive a new WiFi configuration:

        ```c++
        server.addAction("apmode", [](ActionMap& map, Stream& output) {
            return true;
        });
        ```

    -   **_void_ setOnClientConnectionCallback(_std::function<void(const String &, int)>_ callback)**

        This method sets the given callback to be executed every time a client connects to the server

        ```c++
        server.setOnClientConnectionCallback([](const String& remoteIp, int remotePort) {
            Serial.print("New connection from: ");
            Serial.print(remoteIp);
            Serial.print(" on port ");
            Serial.println(remotePort);
        });
        ```

    -   **_void_ setOnConnectionCloseCallback(_std::function<void(void)>_ callback)**

        This method sets the given callback to be executed when the connection with the client is about to end

    -   **_void_ setOnServerTerminationCallback(_std::function<void(void)>_ callback)**

        This method sets the given callback to be executed when the main server is terminated (e.g. after the action callback returns `true`)

-   ## RemoteControlSettings

    This class stores the settings needed to initialize the server. It's composed of two more objects, one for the Access-Point-related configuration, and one for the main server configuration.

    -   **_AccessPointSettings_ ACCESS_POINT_SETTINGS**

        -   **_char \*_ WIFI_AP_SSID**

            The name of the Access-Point WiFi network

        -   **_char \*_ WIFI_AP_PASS**

            The password of the Access-Point WiFi network

        -   **_bool_ WIFI_AP_HIDDEN**

            Whether the Access Point shall be hidden

        -   **_int_ WIFI_AP_CHANNEL**

            The channel of the WiFi Access Point

        -   **_int_ WIFI_AP_MAX_CONN**

            The max connections the WiFi Access Point can accept

        -   **_IPAddress_ WIFI_AP_IP_ADDRESS**

            The IP address of the server in AP mode

        -   **_IPAddress_ WIFI_AP_IP_GATEWAY**

            The IP address of the gateway in AP mode

        -   **_IPAddress_ WIFI_AP_SUBNET**

            The subnet mask of the local network in AP mode

        -   **_char \*_ AUTH_USER**

            The authentication username

        -   **_char \*_ AUTH_PASS**

            The authentication password

        -   **_int_ PORT**

            The server port

        -   **_int_ TIMEOUT_MS**

            The connection timeout

        -   **_char_ \*CERTIFICATE**

            The server certificate in PEM format

        -   **_char_ \*PRIVATE_KEY**

            The server private key in PEM format

    -   **_CommandServerSettings_ COMMAND_SERVER_SETTINGS**

        -   **_char \*_ HOSTNAME;**

            The hostname of the server in the Local Network

        -   **_int_ PORT;**

            The server port

        -   **_char \*_ AUTH_USERNAME;**

            The username for the authentication

        -   **_char \*_ AUTH_PASSWORD;**

            The password for the authentication

        -   **_int_ UDP_PORT;**

            The UDP broadcast port

        -   **_char_ \*UDP_PACKET;**

            The UDP packet to transmit

        -   **_size_t_ UDP_PACKET_SIZE;**

            The UDP packet size

        -   **_int_ UDP_RATE_MS;**

            The rate at which to transmit the UDP packet

        -   **_char_ \*CERTIFICATE;**

            The server certificate in PEM format

        -   **_char_ \*PRIVATE_KEY;**

            The server private key in PEM format

        -   **_int_ TIMEOUT_MS;**

            The connection timeout

    ```c++
    RemoteControlSettings settings;

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
    ```

    Then in the loop section of your project you only need to call the `execute` function:

    ```c++
    void loop()
    {
        server.execute();
    }
    ```

-   ## SerialMap&lt;T, N&gt;

    The `SerialMap` class is at the base of the socket communication protocol. The template parameters `T` and `N` are, respectively, the type of the string class used for storing strings (it's there only to support the Arduino `String` and the `std::string` types indifferently) and the max number of key/value pairs the map is allowed to contain.

    This class supports serialization and de-serialization out-of-the box, through the constructor accepting an array of `char`, and through the `serialize` method which writes the serialized data to an array of `char`.

    Anothe feature it supports is reading and writing directly to Arduino `Stream` objects, avoiding the manual creation of a buffer, through the static method `::fromStream(Stream&)` and the instance method `write(Stream&)`.

    -   **SerialMap()**

        This is the default constructor, initializing an empty map.

    -   **SerialMap(const _char_ \*buffer, _size_t_ len)**

        This constructor initializes the map from the provided _char_ array

    -   **static _SerialMap_ fromStream(_Stream_ &stream, _int_ timeout)**

        This static method initializes the map from the given `Stream` object, reading the data directly from it.

    -   **_size_t_ serialize(_char_ \*data, _size_t_ len) const**

        Serialized the map content to a `char` array, returning the size of the written data.

    -   **_void_ write(_Stream_ &stream)**

        Writes the serialized data directly to the provided `Stream` object.

    _Inherited from the base `Map` class_

    -   **_bool_ put(const _T_ &key, const _E_ &value)**

        Sets the key/value pair on the map object, returning `false` if the map is full.

    -   **const _E_ \*get(const _T_ &key) const**

        Gets a pointer to the value associated with the given key. It returns a pointer to the value, and `nullptr` when the given key has not been found. The map can also be accessed through the index operator:

        ```c++
        String *value = map['key'];
        ```

    -   **_bool_ remove(const _T_ &key)**

        Attempts removing a key/value pair from the map, returning `false` if the given key has not been found.

    -   **_bool_ has(const _T_ &key) const**

        Returns `true` if the map contains the given key, `false` otherwise.

    -   **_int_ getSize() const**

        Returns the map size
