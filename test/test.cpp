#define _TEST_ENV

#include <unity.h>
#include <string>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include "Optional.h"
#include "mocks.h"
#include "Map.h"
#include "SerialMap.h"

void test_Optional();
void test_Serialization_deserialization();
void test_Stream_read_write();
void test_Errors();
void test_Map();

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_Optional);
    RUN_TEST(test_Serialization_deserialization);
    RUN_TEST(test_Stream_read_write);
    RUN_TEST(test_Errors);
    RUN_TEST(test_Map);

    return UNITY_END();
}

void test_Optional()
{
    Optional<std::string> def;
    Optional<int> empty = {};

    TEST_MESSAGE("The default-constructed and empty initializer-list constructed optionals should contain no value");

    TEST_ASSERT_FALSE(def.hasValue());
    TEST_ASSERT_FALSE(def.hasValue());

    def = "ciao";
    empty.set(52);

    TEST_MESSAGE("Assigning a value to an empty optional should set the exact (copy-constructed) value in it");

    TEST_ASSERT_TRUE(def.hasValue());
    TEST_ASSERT(def.get() == "ciao");
    TEST_ASSERT_TRUE(empty.hasValue());
    TEST_ASSERT_EQUAL_INT(empty.get(), 52);

    def = {};
    empty = {};

    TEST_MESSAGE("Assigning an empty initializer-list should reset the optional to not having a value");

    TEST_ASSERT_FALSE(def.hasValue());
    TEST_ASSERT_FALSE(empty.hasValue());

    Optional<std::function<void(void)>> fun = {};

    TEST_MESSAGE("The same should apply to optionals of complex types, such as std::function");

    TEST_ASSERT_FALSE(def.hasValue());

    fun = []()
    { std::cout << "Hello world!" << std::endl; };

    TEST_ASSERT_TRUE(fun.hasValue());
    TEST_ASSERT_NOT_NULL(dynamic_cast<std::function<void(void)> *>(&fun.get()));
    fun.get()();
}

void test_Map()
{
    Map<int, std::string, 3> map;

    TEST_MESSAGE("Should succeed setting the entries when below the size limit");

    TEST_ASSERT(map.put(45, "45"));
    TEST_ASSERT(map.put(69, "69"));
    TEST_ASSERT(map.put(90, "90"));

    TEST_MESSAGE("Map should contain the saved entries");

    TEST_ASSERT(map.has(45));
    TEST_ASSERT(map.has(69));
    TEST_ASSERT(map.has(90));

    TEST_MESSAGE("Map should not contain other random stuff");

    TEST_ASSERT(!map.has(666));
    TEST_ASSERT(!map.has(123));
    TEST_ASSERT(!map.has(6969));

    TEST_MESSAGE("Map values should match the ones set previously");

    TEST_ASSERT(*map.get(45) == "45");
    TEST_ASSERT(*map.get(69) == "69");
    TEST_ASSERT(*map.get(90) == "90");

    TEST_MESSAGE("Accessing a random key should give a nullptr back");

    TEST_ASSERT(map.get(666) == nullptr);

    TEST_MESSAGE("Putting an entry when the map is full sould return false and the key should not exist");

    TEST_ASSERT(!map.put(666, "fail"));
    TEST_ASSERT(!map.has(666));
}

void test_Serialization_deserialization()
{
    SerialMap<std::string, 10> map;
    map.put("key", "value");

    char buffer[512];
    char good[] = {
        0x10, 3, 'k', 'e', 'y', 0x11, 5, 'v', 'a', 'l', 'u', 'e', 0x00};

    auto res = map.serialize(buffer, 512);

    TEST_MESSAGE("SerialMap serialization of { \"key\": \"value\" }");

    TEST_ASSERT(std::equal(buffer, buffer + res, good));

    TEST_ASSERT(res == sizeof(good));

    TEST_MESSAGE("SerialMap deserialization of { \"will\": \"it\", \"truly\": \"work?\"}");

    char data[] = {0x10, 4, 'w', 'i', 'l', 'l', 0x11, 2, 'i', 't', 0x10, 5, 't', 'r', 'u', 'l', 'y', 0x11, 5, 'w', 'o', 'r', 'k', '?', 0x00};

    SerialMap<std::string, 10> parsed(data, sizeof(data));

    TEST_ASSERT(parsed.has("will"));
    TEST_ASSERT(parsed.has("truly"));

    TEST_ASSERT(*parsed.get("will") == "it");
    TEST_ASSERT(*parsed.get("truly") == "work?");
}

void test_Stream_read_write()
{
    TEST_MESSAGE("SerialMap deserialization of { \"will\": \"it\", \"truly\": \"work?\"} from a stream");

    char data[] = {0x10, 4, 'w', 'i', 'l', 'l', 0x11, 2, 'i', 't', 0x10, 5, 't', 'r', 'u', 'l', 'y', 0x11, 5, 'w', 'o', 'r', 'k', '?', 0x00};
    std::stringstream strm;
    strm << data;
    strm.seekg(std::ios::beg);
    IoStreamProxy strmp(strm);

    SerialMap<std::string, 10> parsed_s = SerialMap<std::string, 10>::fromStream(strmp, 3000);

    TEST_ASSERT(parsed_s.has("will"));
    TEST_ASSERT(parsed_s.has("truly"));

    TEST_ASSERT(*parsed_s.get("will") == "it");
    TEST_ASSERT(*parsed_s.get("truly") == "work?");

    SerialMap<std::string, 10> map_;
    map_.put("key", "value");

    char buffer_[512] = {};
    char good_[] = {
        0x10, 3, 'k', 'e', 'y', 0x11, 5, 'v', 'a', 'l', 'u', 'e', 0x00};

    std::stringstream strm_;
    strm_.seekp(std::ios::beg);
    IoStreamProxy strmp_(strm_);

    TEST_MESSAGE("SerialMap serialization of { \"key\": \"value\" } to a stream");

    map_.write(strmp_);

    strm_.seekg(std::ios::beg);
    strm_.read(buffer_, 512);
    unsigned long res_ = strm_.gcount();

    TEST_ASSERT(std::equal(buffer_, buffer_ + res_, good_));

    TEST_ASSERT(res_ == sizeof(good_));
}

void test_Errors()
{
    TEST_MESSAGE("Trying to parse random data should immediately stop the parsing and keep the SerialMap empty");

    char trickyData[] = {0x10, 0x45, 'B', 'a', 'd', 0x11, 0x2, 'O', 'k'};

    SerialMap<std::string, 10> map(trickyData, sizeof(trickyData));

    TEST_ASSERT(map.getSize() == 0);
}