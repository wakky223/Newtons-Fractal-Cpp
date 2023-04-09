#pragma once

/// @brief is nan again because isnan doesn't work with -Ofast apparently
/// @param v input floating point number
/// @return weather the input is NAA
bool isnanIEEE754(double v){
    unsigned long long * i = (unsigned long long *)(&v);
    *i &= 0x7FFFFFFFFFFFFFFF;
    return *i > 0x7FF0000000000000  && *i <= 0x7FFFFFFFFFFFFFFF;
}

/// @brief is nan again because isnan doesn't work with -Ofast apparently
/// @param v input floating point number
/// @return weather the input is NAA
bool isnanIEEE754(float v){
    int * i = (int *)(&v);
    *i &= 0x7FFFFFFF;
    return *i > 0x7F800000  && *i <= 0x7FFFFFFF;
}