#ifndef DECODING_H
#define DECODING_H

#include <stdio.h>

// Функция кодирования исходного файла base16 - алгоритмом 
unsigned char* base16_decode(const unsigned char* input, size_t len, unsigned char* output);

// Функция кодирования исходного файла base32 - алгоритмом
unsigned char* base32_decode(const unsigned char* input, size_t len, size_t* output_len);

// Функция кодирования исходного файла base58 - алгоритмом
unsigned char* base58_decode(const unsigned char* input, size_t len, size_t* output_len);

// Функция кодирования исходного файла base62 - алгоритмом 
unsigned char* base62_decode(const unsigned char* input, size_t len, size_t* output_len);

// Функция кодирования исходного файла base64 - алгоритмом
unsigned char* base64_decode(const unsigned char* input, size_t len, size_t* output_len);

// Функция кодирования исходного файла base85 - алгоритмом 
unsigned char* base85_decode(const unsigned char* input, size_t len, size_t* output_len);

#endif