#ifndef BUFFER_PARSER_H
#define BUFFER_PARSER_H

#ifndef _TEST_ENV
#include <Arduino.h>
#endif

#include "Map.h"
#include "Serializable.h"
#include "Logging.h"

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
        char buffer[BUFFER_SIZE];

        unsigned long start = millis();

        while (stream.available() <= 0 && millis() < start + timeout)
            delay(20);

        while (stream.available() > 0)
        {
            read += stream.readBytesUntil('\0', buffer + read, BUFFER_SIZE - read);
            delay(10);
        }

        Log.printfln("Read chars: %d", read);

        return SerialMap(buffer, read);
    }
    /**
     * @brief Construct a new Serial Map object from the raw data
     * 
     * @param buffer The data buffer
     * @param len The buffer size
     */
    SerialMap(const char *buffer, size_t len)
    {
        //std::stringstream sst;
        //sst.write(buffer, len);
        char buf[257] = {};

        size_t cursor = 0;

        while ((cursor + 2) < len)
        {
            if (buffer[cursor++] != KEY_TYPE)
            {
                break;
            }

            unsigned char length = buffer[cursor++];

            if (cursor + length + 2 > len)
            {
                break;
            }

            //sst.read(buf, length);
            memcpy(buf, buffer + cursor, length);
            buf[length] = 0;
            T key(buf);
            cursor += length;

            if (buffer[cursor++] != VALUE_TYPE)
            {
                break;
            }

            length = buffer[cursor++];

            if (cursor + length > len)
            {
                break;
            }

            //sst.read(buf, length);
            memcpy(buf, buffer + cursor, length);
            buf[length] = 0;
            T value(buf);
            cursor += length;

            Map<T, T, S>::put(key, value);
        }
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

        size_t written = 0;

        for (int i = 0; i < Map<T, T, S>::size; i++)
        {
            const T &key = Map<T, T, S>::keys[i];
            const T &value = Map<T, T, S>::values[i];

            data[written++] = KEY_TYPE;
            data[written++] = (unsigned char)key.length();

            memcpy(data + written, key.c_str(), key.length());
            written += key.length();

            data[written++] = VALUE_TYPE;
            data[written++] = (unsigned char)value.length();
            memcpy(data + written, value.c_str(), value.length());
            written += value.length();
        }

        data[written++] = '\0';

        return written;
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

    void print(Stream &stream)
    {
        stream.print("[ \"");
        for (int i = 0; i < Map<T, T, S>::size; i++)
        {
            stream.print(Map<T, T, S>::keys[i].c_str());
            stream.print("\" => \"");
            stream.print(Map<T, T, S>::values[i].c_str());
            stream.print("\", ");
        }
        stream.println(" ]");
    }

private:
    static constexpr char KEY_TYPE = 0x10;
    static constexpr char VALUE_TYPE = 0x11;
    static constexpr int BUFFER_SIZE = 512; //10kB
};

#endif
