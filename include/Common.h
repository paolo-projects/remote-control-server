#ifndef COMMON_H
#define COMMON_H

#define CALLBACK(class, method) std::bind(&class ::method, this, std::placeholders::_1, std::placeholders::_2)
typedef SerialMap<String, 10> ActionMap;
typedef SerialMap<String, 1> ResponseMap;

#endif