#pragma once
#include <stdint.h>

bool strcmp(char arr[], const char* ch);
int strlength(const char* str);
int atoi(const char* str);
const char* ToString(uint64_t value);
const char* ToString(int64_t value);
const char* ToString(unsigned int value);
const char* ToHexString(uint64_t value);
const char* ToHexString(uint32_t value);
const char* ToHexString(uint16_t value);
const char* ToHexString(uint8_t value);
const char* ToString(double value, uint8_t decimalPlaces);
const char* ToString(double value);
