#ifndef OPTIONAL_H
#define OPTIONAL_H

#include <initializer_list>

template <typename T>
class Optional
{
public:
    Optional() {}
    Optional(const T &copy)
        : has_value(copy.has_value), value(copy.value != nullptr ? new T(*(copy.value)) : nullptr)
    {
    }
    ~Optional()
    {
        if (value != nullptr)
        {
            delete value;
            value = nullptr;
        }
    }

    bool hasValue()
    {
        return has_value;
    }
    T &get()
    {
        return *value;
    }
    void set(const T &value)
    {
        *this = value;
    }

    Optional<T> &operator=(const T &assign)
    {
        if (value != nullptr)
        {
            delete value;
        }
        has_value = true;
        value = new T(assign);
        return *this;
    }
    Optional<T> &operator=(const Optional<T> &assign)
    {
        if (value != nullptr)
        {
            delete value;
        }
        if (!assign.has_value)
        {
            has_value = false;
            value = nullptr;
        }
        else
        {
            has_value = true;
            value = new T(*(assign.value));
        }
        return *this;
    }
    Optional<T> &operator=(std::initializer_list<T> l)
    {
        if (value != nullptr)
        {
            delete value;
        }
        if (l.size() == 0)
        {
            has_value = false;
            value = nullptr;
        }
        else
        {
            *this = *l.begin();
        }
        return *this;
    }

private:
    bool has_value = false;
    T *value = nullptr;
};

#endif // OPTIONAL_H
