#ifndef ENCODING_H
#define ENCODING_H

#include <stdio.h>

// Функция кодирования исходного файла base16 - алгоритмом --- РАБОТАЕТ
char* base16_encode(const unsigned char *input, size_t input_len, char *output);
// Функция кодирования исходного файла base32 - алгоритмом --- РАБОТАЕТ
char* base32_encode(const char* input, size_t len, char* output);

// Функция кодирования исходного файла base58 - алгоритмом --- РАБОТАЕТ
char* base58_encode(const unsigned char* input, size_t len, char* output);

// Функция кодирования исходного файла base62 - алгоритмом --- РАБОТАЕТ
char* base62_encode(const unsigned char* input, size_t len, char* output);

// Функция кодирования исходного файла base64 - алгоритмом --- РАБОТАЕТ
char* base64_encode(const unsigned char* input, size_t len);

// Функция кодирования исходного файла base85 - алгоритмом --- РАБОТАЕТ
char* base85_encode(const unsigned char* input, size_t len);

#endif