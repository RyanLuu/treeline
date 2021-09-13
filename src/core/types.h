#pragma once

#include <sstream>
#include <string>

template <typename T>
struct Result {
    Result(T t) : value(t), isError(false) {}
    Result() {}
    T value;
    bool isError;
    std::string error;
};

template <typename T>
Result<T> errorResult(const std::string &error) {
    Result<T> ret;
    ret.isError = true;
    ret.error = error;
    return ret;
}

