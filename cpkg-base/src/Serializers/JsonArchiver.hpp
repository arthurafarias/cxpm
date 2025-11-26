#pragma once

#include <string>

template<typename ValueType>
class ValueDescriptor {
    std::string name;
    const ValueType& value;
};

class AbstractArchiver {
    public:

    template<typename ArgumentType>
    ValueDescriptor<ArgumentType> named_value(std::string name, const ArgumentType& arg) {
        return ValueDescriptor<ArgumentType>{ .name = name, .value = arg };
    }

};

class JsonArchiver : public AbstractArchiver {
    public:
    using AbstractArchiver::named_value;
};

inline JsonArchiver& operator&(JsonArchiver& ar, const int& value) {
    return ar;
}

inline JsonArchiver& operator&(JsonArchiver& ar, const ValueDescriptor<int>& value) {
    return ar;
}

inline JsonArchiver& operator&(JsonArchiver& ar, const double& value) {
    return ar;
}

inline JsonArchiver& operator&(JsonArchiver& ar, const ValueDescriptor<double>& value) {
    return ar;
}