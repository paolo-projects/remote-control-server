#ifndef MOCKS_H
#define MOCKS_H

#include <string>
#include <iostream>
#include <chrono>
#include <ios>
#include <cstring>
#include <iterator>

// Mock for Stream
class Stream
{
public:
    virtual int available() { return 0; }
    virtual size_t write(char *data, size_t sz) { return 0; }
    virtual size_t write(const char *str) { return 0; }
    virtual size_t write(char c) { return 0; }
    virtual size_t readBytesUntil(char c, char *b, size_t d) { return 0; }
};

class IoStreamProxy : public Stream
{
public:
    IoStreamProxy(std::basic_iostream<char> &stream) : stream(stream) {}
    int available() override { return stream.good() ? 1 : -1; }
    size_t write(char *data, size_t sz) override
    {
        std::copy(data, data + sz, std::ostream_iterator<char>(stream));
        return sz;
    }
    size_t write(const char *str) override
    {
        stream << str;
        return strlen(str);
    }
    size_t write(char c) override
    {
        stream << c;
        return 1;
    }
    size_t readBytesUntil(char terminator, char *buffer, size_t length) override
    {
        if (length < 1)
            return 0;
        size_t index = 0;
        while (index < length)
        {
            int c = stream.get();
            if (c < 0 || c == terminator)
                break;
            *buffer++ = (char)c;
            index++;
        }
        return index; // return number of characters, not including null terminator }
    }

private:
    std::basic_iostream<char> &stream;
};

#ifdef PRINT_SERIAL
struct SerialMock
{
    void print(std::string str) { std::cout << str; }
    void print(const char *str) { std::cout << str; }
    void print(int i) { std::cout << i; }
    void println(std::string str) { std::cout << str << std::endl; }
    void println(const char *str) { std::cout << str << std::endl; }
    void println(int i) { std::cout << i << std::endl; }
};
#else
struct SerialMock
{
    void print(std::string str) {}
    void print(const char *str) {}
    void print(int i) {}
    void println(std::string str) {}
    void println(const char *str) {}
    void println(int i) {}
};
#endif

// Mock for Serial global object
SerialMock Serial;

unsigned long millis()
{
    return std::chrono::system_clock::now().time_since_epoch() /
           std::chrono::milliseconds(1);
}

void delay(unsigned long t) {}

#endif // MOCKS_H