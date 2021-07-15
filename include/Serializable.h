#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <stdlib.h>

class Serializable
{
public:
    virtual size_t serialize(char *, size_t) const = 0;
};

#endif // SERIALIZABLE_H
