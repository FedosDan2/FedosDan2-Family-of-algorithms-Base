/**
 * @file encod_func.c
 * @brief Реализация функций кодирования в различные base-форматы (Base16, Base32, Base58, Base62, Base64, Base85).
 * 
 * @author Фёдор
 * @date 25.03.2025
 * 
 * @note Для работы функций требуются таблицы символов, определённые в `tables.h`.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../include/encod_func.h"
#include "../include/tables.h"


/**
 * @brief Кодирует данные в формат Base16 (HEX).
 * 
 * @param input Указатель на входные данные.
 * @param input_len Длина входных данных в байтах.
 * @param output Буфер для записи результата (должен быть размером `2 * input_len + 1`).
 * @return char* Указатель на закодированную строку (тот же, что и `output`).
 * 
 * @note Каждый байт входных данных кодируется двумя символами HEX (0-9, A-F).
 * 
 * @example
 * unsigned char data[] = {0xAB, 0xCD};
 * char encoded[5];
 * base16_encode(data, 2, encoded); // Результат: "ABCD"
 */
// Функция кодирования исходного файла base16 - алгоритмом --- РАБОТАЕТ
char* base16_encode(const unsigned char *input, size_t input_len, char *output) {
    // Таблица символов Base16
    const char *base16_chars = "0123456789ABCDEF";

    // Проходим по каждому байту входных данных
    for (size_t i = 0; i < input_len; i++) {
        // Кодируем старший и младший полубайты
        output[2 * i] = base16_chars[(input[i] >> 4) & 0x0F]; // Старший полубайт
        output[2 * i + 1] = base16_chars[input[i] & 0x0F];    // Младший полубайт
    }

    // Завершаем строку нулевым символом
    output[2 * input_len] = '\0';
    return output;
}



/**
 * @brief Кодирует данные в формат Base32.
 * 
 * @param input Указатель на входные данные.
 * @param len Длина входных данных в байтах.
 * @param output Буфер для записи результата (должен быть достаточного размера).
 * @return char* Указатель на закодированную строку.
 * 
 * @note Используется таблица символов `base32_table` из `tables.h`.
 * 
 * @example
 * const char data[] = "Hello";
 * char encoded[20];
 * base32_encode(data, 5, encoded); // Результат: "JBSWY3DP"
 */
// Функция кодирования исходного файла base32 - алгоритмом --- РАБОТАЕТ
char* base32_encode(const char* input, size_t len, char* output) {
    int i, j;
    unsigned int val = 0;
    int bit_count = 0;
    size_t output_len = (len * 8 + 4) / 5; // длина выходного массива Base32
    for (i = 0, j = 0; i < len; i++) {
        val = (val << 8) | (unsigned char)input[i];
        bit_count += 8;
        while (bit_count >= 5) {
            output[j++] = base32_table[(val >> (bit_count - 5)) & 0x1F];
            bit_count -= 5;
        }
    }
    if (bit_count > 0) {
        output[j++] = base32_table[(val << (5 - bit_count)) & 0x1F];
    }
    output[j] = '\0';
    return output;
}




/**
 * @brief Кодирует данные в формат Base58 (используется в Bitcoin-адресах).
 * 
 * @param input Указатель на входные данные.
 * @param len Длина входных данных в байтах.
 * @param output Буфер для записи результата (должен быть достаточного размера).
 * @return char* Указатель на закодированную строку или `NULL` при ошибке.
 * 
 * @note Ведущие нули кодируются как '1'.
 * @warning Требует выделения памяти внутри функции.
 * 
 * @example
 * unsigned char data[] = {0x00, 0xAB, 0xCD};
 * char encoded[10];
 * base58_encode(data, 3, encoded); // Результат: "1qfr"
 */
// Функция кодирования исходного файла base58 - алгоритмом --- РАБОТАЕТ
char* base58_encode(const unsigned char* input, size_t len, char* output) {
    unsigned int carry;
    size_t i, j, output_len;

    // Выделяем память с запасом
    unsigned char* result = (unsigned char*)calloc(len * 138 / 100 + 1, sizeof(unsigned char));
    if (!result) {
        perror("Ошибка выделения памяти для результата");
        return NULL;
    }

    // Обработка ведущих нулей
    for (i = 0; i < len && input[i] == 0; i++) {
        output[i] = '1'; // Каждый ведущий ноль кодируется как '1'
    }
    j = i; // Индекс для записи результата

    // Главный цикл кодирования
    for (; i < len; i++) {
        carry = input[i];
        for (size_t k = 0; k < j; k++) {
            carry += 256 * result[k];
            result[k] = carry % 58;
            carry /= 58;
        }
        // Обрабатываем остаток от деления
        while (carry > 0) {
            result[j++] = carry % 58;
            carry /= 58;
        }
    }

    // Вычисляем длину результата
    output_len = j;
    // Преобразуем результат в строку символов
    for (i = 0; i < output_len; i++) {
        output[i] = base58_table[result[output_len - i - 1]];
    }
    output[output_len] = '\0';  // Завершаем строку

    // Освобождаем память
    free(result);

    return output;
}




/**
 * @brief Кодирует данные в формат Base62 (0-9, A-Z, a-z).
 * 
 * @param input Указатель на входные данные.
 * @param len Длина входных данных в байтах.
 * @param output Буфер для записи результата (должен быть достаточного размера).
 * @return char* Указатель на закодированную строку или `NULL` при ошибке.
 * 
 * @note Используется таблица символов `base62_table` из `tables.h`.
 * @warning Требует выделения памяти внутри функции.
 */
// Функция кодирования исходного файла base62 - алгоритмом --- РАБОТАЕТ
char* base62_encode(const unsigned char* input, size_t len, char* output) {
    unsigned int carry;
    size_t i, j, output_len;
    unsigned char* result = (unsigned char*)malloc(len * 2); // Максимальная длина результата

    if (!result) {
        perror("Ошибка выделения памяти для результата");
        return NULL;
    }

    // Главный цикл кодирования
    for (i = 0, j = 0; i < len; i++) {
        carry = input[i];
        for (size_t k = 0; k < j; k++) {
            carry += 256 * result[k];
            result[k] = carry % 62;
            carry /= 62;
        }
        // Обрабатываем остаток от деления
        while (carry > 0) {
            result[j++] = carry % 62;
            carry /= 62;
        }
    }

    // Вычисляем длину результата
    output_len = j;
    // Преобразуем результат в строку символов
    for (i = 0; i < output_len; i++) {
        output[i] = base62_table[result[output_len - i - 1]];
    }
    output[output_len] = '\0';  // Завершаем строку

    // Освобождаем память
    free(result);

    return output;
}



/**
 * @brief Кодирует данные в формат Base64.
 * 
 * @param input Указатель на входные данные.
 * @param len Длина входных данных в байтах.
 * @return char* Указатель на закодированную строку (нужно освободить через `free()`).
 * 
 * @note Дополнение '=' добавляется, если длина не кратна 3.
 * @warning Выделяет память внутри функции.
 * 
 * @example
 * unsigned char data[] = {0xAB, 0xCD, 0xEF};
 * char* encoded = base64_encode(data, 3); // Результат: "q83D"
 * free(encoded);
 */
// Функция кодирования исходного файла base64 - алгоритмом --- РАБОТАЕТ
char* base64_encode(const unsigned char* input, size_t len) {
    // Вычисляем длину выходной строки
    size_t output_len = 4 * ((len + 2) / 3); // Каждые 3 байта кодируются в 4 символа
    char* output = (char*)malloc(output_len + 1); // +1 для завершающего нуля
    if (!output) return NULL;

    size_t i, j;
    uint32_t val;

    for (i = 0, j = 0; i < len; i += 3) {
        // Собираем 24-битное значение из 3 байтов
        val = (input[i] << 16) | ((i + 1 < len ? input[i + 1] : 0) << 8) | (i + 2 < len ? input[i + 2] : 0);

        // Разбиваем на 4 группы по 6 бит и кодируем в Base64
        output[j++] = base64_table[(val >> 18) & 0x3F];
        output[j++] = base64_table[(val >> 12) & 0x3F];
        output[j++] = (i + 1 < len) ? base64_table[(val >> 6) & 0x3F] : '=';
        output[j++] = (i + 2 < len) ? base64_table[val & 0x3F] : '=';
    }

    output[j] = '\0'; // Завершаем строку
    return output;
}




/**
 * @brief Кодирует данные в формат Base85 (используется в PDF и PostScript).
 * 
 * @param input Указатель на входные данные.
 * @param len Длина входных данных в байтах.
 * @return char* Указатель на закодированную строку (нужно освободить через `free()`).
 * 
 * @note Каждые 4 байта кодируются в 5 символов.
 * @warning Выделяет память внутри функции.
 */
// Функция кодирования исходного файла base85 - алгоритмом --- РАБОТАЕТ
char* base85_encode(const unsigned char* input, size_t len) {
    // Вычисляем размер выходного буфера
    size_t output_len = ((len + 3) / 4) * 5 + 1; // +1 для завершающего нуля
    char* output = (char*)malloc(output_len);
    if (!output) {
        perror("Ошибка выделения памяти для результата");
        return NULL;
    }

    char* ptr = output;

    // Обрабатываем входные данные блоками по 4 байта
    for (size_t i = 0; i < len; i += 4) {
        // Считываем блок из 4 байтов (дополняем нулями, если нужно)
        uint32_t value = 0;
        size_t block_size = (len - i > 4) ? 4 : len - i;
        for (size_t j = 0; j < block_size; j++) {
            value = (value << 8) | input[i + j];
        }
        for (size_t j = block_size; j < 4; j++) {
            value <<= 8; // Дополняем нулями
        }

        // Кодируем значение в 5 символов Base85
        char block[5];
        for (int j = 4; j >= 0; j--) {
            block[j] = base85_table[value % 85];
            value /= 85;
        }

        // Копируем закодированный блок в выходной буфер
        memcpy(ptr, block, 5);
        ptr += 5;
    }

    // Добавляем завершающий ноль (если нужно)
    *ptr = '\0';

    return output;
}