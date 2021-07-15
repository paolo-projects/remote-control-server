#ifndef BUFFER_PARSER_H
#define BUFFER_PARSER_H

#ifndef _TEST_ENV
#include <Arduino.h>
#endif

#include <sstream>
#include "Map.h"
#include "Serializable.h"

/**
 * @brief A serializable map based on the existing implementation with String support
 * 
 * @tparam T The data type used for keys and valued. Generally it has to be a string-like type, such as std::string,
 *  otherwise the provided type must implement a constructor that accepts a null terminated char array, a << stream operator to std::io_stream types
 *  and a length() method
 * @tparam S The maximum size of the map in number of elements
 */
template <typename T = std::string, int S = 24>
class SerialMap : public Map<T, T, S>, public Serializable
{
public:
    SerialMap() = default;
    /**
     * @brief Construct a new Serial Map object read from a Stream
     * 
     * @param stream The stream
     * @param timeout The timeout 
     * @return The Serial Map
     */
    static SerialMap fromStream(Stream &stream, int timeout)
    {
        int read = 0;
        char buffer[512];

        unsigned long start = millis();

        while (stream.available() <= 0 && millis() < start + timeout)
            delay(20);

        while (stream.available() > 0)
        {
            read += stream.readBytesUntil('\0', buffer + read, 1024 - read);
        }

        Serial.print(" Read chars: ");
        Serial.println(read);

        SerialMap result(buffer, read);

        return result;
    }
    /**
     * @brief Construct a new Serial Map object from the raw data
     * 
     * @param buffer The data buffer
     * @param len The buffer size
     */
    SerialMap(const char *buffer, size_t len)
    {
        std::stringstream sst;
        sst.write(buffer, len);
        char buf[257] = {};

        while (!sst.eof())
        {
            if (!sst.good() || sst.get() != KEY_TYPE)
            {
                break;
            }

            unsigned char length = sst.get();
            sst.read(buf, length);
            buf[length] = 0;
            T key(buf);

            if (!sst.good() || sst.get() != VALUE_TYPE)
            {
                break;
            }

            length = sst.get();
            sst.read(buf, length);
            buf[length] = 0;
            T value(buf);

            Map<T, T, S>::put(key, value);
        }

        Serial.print("Keys: [ ");
        for (int i = 0; i < Map<T, T, S>::size; i++)
        {
            Serial.print(Map<T, T, S>::keys[i]);
            Serial.print(", ");
        }
        Serial.println(" ]");
    }

    /**
     * @brief Serializes the map into a binary data stream
     * 
     * @param data The output data buffer
     * @param len The size of the output data buffer
     * @return size_t The size of the data written into the buffer, or -1 if it failed
     */
    size_t serialize(char *data, size_t len) const override
    {
        size_t expectedSize = 1;
        for (int i = 0; i < Map<T, T, S>::size; i++)
        {
            expectedSize += Map<T, T, S>::keys[i].length() + Map<T, T, S>::values[i].length() + 4;
        }

        if (expectedSize > len)
        {
            return -1;
        }

        std::stringstream sst;
        for (int i = 0; i < Map<T, T, S>::size; i++)
        {
            const T &key = Map<T, T, S>::keys[i];
            const T &value = Map<T, T, S>::values[i];

            sst.put(KEY_TYPE);
            sst.put(key.length());
            sst << key.c_str();

            sst.put(VALUE_TYPE);
            sst.put(value.length());
            sst << value.c_str();
        }

        sst << '\0';

        sst.read(data, expectedSize);

        return expectedSize;
    }

    /**
     * @brief Writes the serialized map directly to the Stream
     * 
     * @param stream The stream
     */
    void write(Stream &stream)
    {
        for (auto it = Map<T, T, S>::begin(); it != Map<T, T, S>::end(); it++)
        {
            stream.write(KEY_TYPE);
            stream.write(static_cast<unsigned char>((*it).key().length()));
            stream.write((*it).key().c_str());
            stream.write(VALUE_TYPE);
            stream.write(static_cast<unsigned char>((*it).value().length()));
            stream.write((*it).value().c_str());
        }
        stream.write('\0');
    }

private:
    static constexpr char KEY_TYPE = 0x10;
    static constexpr char VALUE_TYPE = 0x11;
    static constexpr int BUFFER_LIMIT = 1024 * 10; //10kB
};

#endif
