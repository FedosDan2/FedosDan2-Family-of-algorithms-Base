/**
 * @file decod_func.c
 * @brief Реализация функций декодирования из различных base-форматов (Base16, Base32, Base58, Base62, Base64, Base85)
 * 
 * @author Фёдор
 * @date 25.03.2025
 * 
 * @note Для работы функций требуются таблицы символов, определённые в `tables.h`
 * @warning Некоторые функции выделяют память, которую нужно освобождать вручную
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../include/decod_func.h"
#include "../include/tables.h"



// Функция поиска декодированного символа
int find_index(char c, const char table[]) {
/**
 * @brief Поиск индекса символа в заданной таблице
 * 
 * @param c Символ для поиска
 * @param table Таблица символов для поиска
 * @return int Индекс символа или -1 если не найден
 * 
 * @note Используется всеми функциями декодирования для преобразования символов
 */
    for (int i = 0; table[i] != '\0'; i++) {
        if (table[i] == c) {
            return i;
        }
    }
    return -1; // Символ не найден
}



// Функция декодирования исходного файла base16 - алгоритмом --- РАБОТАЕТ
unsigned char* base16_decode(const unsigned char* input, size_t len, unsigned char* output) {
/**
 * @brief Декодирует данные из формата Base16 (HEX)
 * 
 * @param input Указатель на входные данные в Base16
 * @param len Длина входных данных
 * @param output Буфер для записи результата (должен быть размером len/2)
 * @return unsigned char* Указатель на декодированные данные или NULL при ошибке
 * 
 * @note Входная строка должна иметь четную длину
 * @example
 * const unsigned char encoded[] = "48656C6C6F"; // "Hello" в HEX
 * unsigned char decoded[5];
 * base16_decode(encoded, 10, decoded); // Результат: "Hello"
 */
    // Проверка на четность длины входных данных
    if (len % 2 != 0) {
        fprintf(stderr, "Error: Input length must be even.\n");
        return NULL;
    }

    // Декодирование
    for (size_t i = 0; i < len; i += 2) {
        int high_nibble = find_index(input[i], base16_table);
        int low_nibble = find_index(input[i + 1], base16_table);

        // Проверка на корректность символов
        if (high_nibble == -1 || low_nibble == -1) {
            fprintf(stderr, "Error: Invalid character in string.\n");
            return NULL;
        }

        // Сборка байта из двух полубайтов
        output[i / 2] = (high_nibble << 4) | low_nibble;
    }

    return output;
}



// Функция декодирования исходного файла base32 - алгоритмом --- РАБОТАЕТ
unsigned char* base32_decode(const unsigned char* input, size_t len, size_t* output_len) {
/**
 * @brief Декодирует данные из формата Base32
 * 
 * @param input Указатель на входные данные в Base32
 * @param len Длина входных данных
 * @param output_len Указатель для записи длины выходных данных
 * @return unsigned char* Указатель на декодированные данные (нужно освободить) или NULL при ошибке
 * 
 * @note Автоматически обрабатывает дополнение '='
 * @warning Выделяет память, которую нужно освободить через free()
 */
    size_t padded_len = len;
    while (padded_len % 8 != 0) {
        padded_len++; // Учитываем недостающие символы
    }

    char* padded_input = (char*)malloc(padded_len + 1);
    if (!padded_input) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }
    
    strncpy(padded_input, input, len);
    memset(padded_input + len, '=', padded_len - len);
    padded_input[padded_len] = '\0';
    
    size_t estimated_size = (padded_len / 8) * 5;
    unsigned char* output = (unsigned char*)malloc(estimated_size);
    if (!output) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        free(padded_input);
        return NULL;
    }
    
    size_t output_pos = 0;
    for (size_t i = 0; i < padded_len; i += 8) {
        int indices[8] = {0};
        for (int j = 0; j < 8; j++) {
            if (padded_input[i + j] == '=') {
                indices[j] = 0;
            } else {
                indices[j] = find_index(padded_input[i + j], base32_table);
                if (indices[j] == -1) {
                    fprintf(stderr, "Error: Invalid character in input string.\n");
                    free(output);
                    free(padded_input);
                    return NULL;
                }
            }
        }

        uint64_t value = 0;
        for (int j = 0; j < 8; j++) {
            value |= ((uint64_t)indices[j] << (35 - j * 5));
        }

        output[output_pos++] = (value >> 32) & 0xFF;
        if (padded_input[i + 2] != '=') {
            output[output_pos++] = (value >> 24) & 0xFF;
        }
        if (padded_input[i + 4] != '=') {
            output[output_pos++] = (value >> 16) & 0xFF;
        }
        if (padded_input[i + 5] != '=') {
            output[output_pos++] = (value >> 8) & 0xFF;
        }
        if (padded_input[i + 6] != '=') {
            output[output_pos++] = value & 0xFF;
        }
    }
    
    free(padded_input);
    *output_len = output_pos;
    return output;
}



// Функция декодирования исходного файла base58 - алгоритмом --- РАБОТАЕТ
unsigned char* base58_decode(const unsigned char* input, size_t len, size_t* output_len) {
/**
 * @brief Декодирует данные из формата Base58 (используется в Bitcoin)
 * 
 * @param input Указатель на входные данные в Base58
 * @param len Длина входных данных
 * @param output_len Указатель для записи длины выходных данных
 * @return unsigned char* Указатель на декодированные данные (нужно освободить) или NULL при ошибке
 * 
 * @note Корректно обрабатывает ведущие '1' (кодируют нулевые байты)
 * @warning Выделяет память, которую нужно освободить через free()
 */
    size_t zero_count = 0;
    while (zero_count < len && input[zero_count] == base58_table[0]) {
        zero_count++;
    }

    // Выделяем память для результата
    size_t size = (len - zero_count) * 733 / 1000 + 1; // Оценка размера выходного буфера
    unsigned char* output = (unsigned char*)malloc(size);
    if (!output) {
        *output_len = 0;
        return NULL;
    }
    memset(output, 0, size);

    size_t output_size = 0;
    for (size_t i = zero_count; i < len; i++) {
        int carry = find_index(input[i], base58_table);
        if (carry == -1) {
            free(output);
            *output_len = 0;
            return NULL;
        }

        for (size_t j = 0; j < output_size; j++) {
            carry += (int)output[j] * 58;
            output[j] = carry % 256;
            carry /= 256;
        }

        while (carry > 0) {
            output[output_size++] = carry % 256;
            carry /= 256;
        }
    }

    // Добавляем нули в начало
    if (zero_count > 0) {
        output = (unsigned char*)realloc(output, output_size + zero_count);
        if (!output) {
            *output_len = 0;
            return NULL;
        }
        memmove(output + zero_count, output, output_size);
        memset(output, 0, zero_count);
        output_size += zero_count;
    }

    for (size_t i = 0; i < output_size / 2; i++){
        unsigned char temp = output[i];
        output[i] = output[output_size - i - 1];
        output[output_size - i - 1] = temp;
    }
    
    output[output_size] = '\0';
    *output_len = output_size;
    return output;
}



// Функция декодирования исходного файла base62 - алгоритмом --- РАБОТАЕТ
unsigned char* base62_decode(const unsigned char* input, size_t len, size_t* output_len) {
/**
 * @brief Декодирует данные из формата Base62 (0-9, A-Z, a-z)
 * 
 * @param input Указатель на входные данные в Base62
 * @param len Длина входных данных
 * @param output_len Указатель для записи длины выходных данных
 * @return unsigned char* Указатель на декодированные данные (нужно освободить) или NULL при ошибке
 * 
 * @warning Выделяет память, которую нужно освободить через free()
 */
    if (len == 0) {
        *output_len = 0;
        return NULL;
    }

    for (size_t i = 0; i < len; ++i) {
        if (find_index(input[i], base62_table) == -1) {
            *output_len = 0;
            return NULL;
        }
    }

    size_t buffer_capacity = 2;
    unsigned char* buffer = (unsigned char*)malloc(buffer_capacity);
    if (!buffer) {
        *output_len = 0;
        return NULL;
    }
    buffer[0] = 0;
    size_t buffer_len = 1;

    for (size_t i = 0; i < len; ++i) {
        int digit = find_index(input[i], base62_table);
        uint32_t carry;

        carry = 0;
        for (size_t j = 0; j < buffer_len; ++j) {
            uint32_t temp = (uint32_t)buffer[j] * 62 + carry;
            buffer[j] = (unsigned char)(temp % 256);
            carry = temp / 256;
        }
        while (carry > 0) {
            if (buffer_len >= buffer_capacity) {
                buffer_capacity *= 2;
                unsigned char* new_buffer = (unsigned char*)realloc(buffer, buffer_capacity);
                if (!new_buffer) {
                    free(buffer);
                    *output_len = 0;
                    return NULL;
                }
                buffer = new_buffer;
            }
            buffer[buffer_len++] = (unsigned char)(carry % 256);
            carry /= 256;
        }

        carry = digit;
        for (size_t j = 0; j < buffer_len; ++j) {
            uint32_t temp = (uint32_t)buffer[j] + carry;
            buffer[j] = (unsigned char)(temp % 256);
            carry = temp / 256;
            if (carry == 0) break;
        }
        while (carry > 0) {
            if (buffer_len >= buffer_capacity) {
                buffer_capacity *= 2;
                unsigned char* new_buffer = (unsigned char*)realloc(buffer, buffer_capacity);
                if (!new_buffer) {
                    free(buffer);
                    *output_len = 0;
                    return NULL;
                }
                buffer = new_buffer;
            }
            buffer[buffer_len++] = (unsigned char)(carry % 256);
            carry /= 256;
        }
    }

    unsigned char* result = (unsigned char*)malloc(buffer_len);
    if (!result) {
        free(buffer);
        *output_len = 0;
        return NULL;
    }
    for (size_t i = 0; i < buffer_len; ++i) {
        result[i] = buffer[buffer_len - 1 - i];
    }
    free(buffer);
    
    result[buffer_len] = '\0';
    *output_len = buffer_len;
    return result;
}



// Функция декодирования исходного файла base64 - алгоритмом --- РАБОТАЕТ
unsigned char* base64_decode(const unsigned char* input, size_t len, size_t* output_len){
/**
 * @brief Декодирует данные из формата Base64
 * 
 * @param input Указатель на входные данные в Base64
 * @param len Длина входных данных
 * @param output_len Указатель для записи длины выходных данных
 * @return unsigned char* Указатель на декодированные данные (нужно освободить) или NULL при ошибке
 * 
 * @note Автоматически обрабатывает дополнение '='
 * @warning Выделяет память, которую нужно освободить через free()
 */
    size_t padded_len = len;
    while (padded_len % 4 != 0) {
        padded_len++; // Учитываем недостающие символы
    }

    char* padded_input = (char*)malloc(padded_len + 1);
    if (!padded_input) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }
    
    strncpy(padded_input, input, len);
    memset(padded_input + len, '=', padded_len - len);
    padded_input[padded_len] = '\0';
    
    size_t estimated_size = (padded_len / 4) * 3;
    unsigned char* output = (unsigned char*)malloc(estimated_size);
    if (!output) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        free(padded_input);
        return NULL;
    }

    size_t output_pos = 0;
    for (size_t i = 0; i < padded_len; i += 4) {
        int indices[4] = {0};
        for (int j = 0; j < 4; j++) {
            if (padded_input[i + j] == '=') {
                indices[j] = 0;
            } else {
                indices[j] = find_index(padded_input[i + j], base64_table);
                if (indices[j] == -1) {
                    fprintf(stderr, "Error: Invalid character in input string.\n");
                    free(output);
                    free(padded_input);
                    return NULL;
                }
            }
        }

        uint32_t value = 0;
        for (int j = 0; j < 4; j++) {
            value |= ((uint32_t)indices[j] << (18 - j * 6));
        }

        output[output_pos++] = (value >> 16) & 0xFF;
        if (padded_input[i + 2] != '=') {
            output[output_pos++] = (value >> 8) & 0xFF;
        }
        if (padded_input[i + 3] != '=') {
            output[output_pos++] = value & 0xFF;
        }

    }
    
    free(padded_input);
    *output_len = output_pos;
    return output;
}



// Функция декодирования исходного файла base85 - алгоритмом --- РАБОТАЕТ
unsigned char* base85_decode(const unsigned char* input, size_t len, size_t* output_len) {
/**
 * @brief Декодирует данные из формата Base85 (используется в PDF/PostScript)
 * 
 * @param input Указатель на входные данные в Base85
 * @param len Длина входных данных
 * @param output_len Указатель для записи длины выходных данных
 * @return unsigned char* Указатель на декодированные данные (нужно освободить) или NULL при ошибке
 * 
 * @note Автоматически пропускает пробелы и символы новой строки
 * @warning Выделяет память, которую нужно освободить через free()
 */
    if (!input|| len == 0) {
        *output_len = 0;
        return NULL;
    }

    // Удаляем возможные пробелы и символы новой строки
    unsigned char* cleaned_input = (unsigned char*)malloc(len + 1);
    size_t cleaned_len = 0;
    for (size_t i = 0; i < len; i++) {
        if (input[i] != ' ' && input[i] != '\n' && input[i] != '\r' && input[i] != '\t') {
            cleaned_input[cleaned_len++] = input[i];
        }
    }
    cleaned_input[cleaned_len] = '\0';

    // Проверяем, что длина входных данных кратна 5
    if (cleaned_len % 5 != 0) {
        free(cleaned_input);
        *output_len = 0;
        return NULL;
    }

    // Вычисляем длину выходных данных
    *output_len = (cleaned_len / 5) * 4;
    unsigned char* output = (unsigned char*)malloc(*output_len);
    if (!output) {
        free(cleaned_input);
        *output_len = 0;
        return NULL;
    }

    // Декодирование
    size_t output_index = 0;
    for (size_t i = 0; i < cleaned_len; i += 5) {
        unsigned long value = 0;
        for (int j = 0; j < 5; j++) {
            const char* pos = strchr(base85_table, cleaned_input[i + j]);
            if (pos == NULL) {
                free(cleaned_input);
                free(output);
                *output_len = 0;
                return NULL;
            }
            value = value * 85 + (pos - base85_table);
        }

        // Распаковка 32-битного значения в 4 байта
        output[output_index++] = (value >> 24) & 0xFF;
        output[output_index++] = (value >> 16) & 0xFF;
        output[output_index++] = (value >> 8) & 0xFF;
        output[output_index++] = value & 0xFF;
    }
    output[output_index++] = '\0';
    free(cleaned_input);
    return output;
}