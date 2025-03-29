/**
 * @file main.c
 * @brief Программа для кодирования и декодирования файлов в различных base-форматах
 * 
 * @author Фёдор
 * @date 25.03.2025
 * 
 * @note Поддерживаемые форматы: Base16, Base32, Base58, Base62, Base64, Base85
 * @warning Для работы требуются заголовочные файлы decod_func.h, encod_func.h и tables.h
 */

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define ENCOD_FUNCTION
#define DECOD_FUNCTION

#include "../include/decod_func.h"
#include "../include/encod_func.h"
#include "../include/tables.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


// Функция для получения имени файла из пути
const char* get_filename(const char* path) {
/**
 * @brief Получает имя файла из полного пути
 * 
 * @param path Полный путь к файлу
 * @return const char* Указатель на имя файла
 */
    const char* last_slash = strrchr(path, '/');
    const char* last_backslash = strrchr(path, '\\');
    const char* last_separator = (last_slash > last_backslash) ? last_slash : last_backslash;
    return (last_separator != NULL) ? last_separator + 1 : path;
}



// Функция получения расширения файла
char* extension_definition(const char* name_input) {
/**
 * @brief Определяет расширение файла
 * 
 * @param name_input Имя файла
 * @return char* Расширение файла (нужно освободить) или NULL при ошибке
 * 
 * @warning Выделяет память, которую нужно освободить через free()
 */

    size_t length = strlen(name_input);
    int num_dots = 0;
    int last_dot_index = -1;

    // Находим индекс последней точки
    for (size_t i = 0; i < length; i++) {
        if (name_input[i] == '.') {
            num_dots++;
            last_dot_index = i;
        }
    }
    // Если точек меньше двух, возвращаем NULL
    if (num_dots < 1) {
        return NULL;
    }

    size_t extension_length = length - (last_dot_index + 1);
    char* extension = (char*)malloc(extension_length + 1);
    if (!extension) {
        perror("Memory allocation error");
        return NULL;
    }

    // Копируем название алгоритма
    strncpy(extension, name_input + last_dot_index + 1, extension_length);
    extension[extension_length] = '\0'; // Добавляем завершающий ноль

    return extension;
}



// Функция преобразования файла в битовую строку
unsigned char* read_file_as_bytes(const char* filename, size_t* file_size) {
/**
 * @brief Читает файл как массив байтов
 * 
 * @param filename Имя файла
 * @param file_size Указатель для записи размера файла
 * @return unsigned char* Буфер с данными (нужно освободить) или NULL при ошибке
 * 
 * @warning Выделяет память, которую нужно освободить через free()
 */
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);  // Получаем размер файла
    rewind(file);
    
    // Выделяем память для данных файла
    unsigned char* buffer = (unsigned char*)malloc(*file_size);
    if (buffer == NULL) {
        perror("Memory allocation error");
        fclose(file);
        return NULL;
    }
    
    // Чтение файла в буфер
    fread(buffer, 1, *file_size, file);
    fclose(file);

    return buffer;
}



// Функция выбора алгоритма кодирования
unsigned char* choice_of_alg(char* file_data, size_t file_size, char** dot_output) {
/**
 * @brief Выбирает алгоритм кодирования и кодирует данные
 * 
 * @param file_data Данные для кодирования
 * @param file_size Размер данных
 * @param dot_output Указатель для записи расширения выходного файла
 * @return unsigned char* Закодированные данные (нужно освободить) или NULL при ошибке
 * 
 * @note Выводит меню выбора алгоритма пользователю
 * @warning Выделяет память, которую нужно освободить через free()
 */
    uint8_t choice;
    unsigned char* encoded_data = NULL;

    printf("Select encoding algorithm:\n");
    printf("1. Base16 - Data, hashing, memory addresses\n");
    printf("2. Base32 - Tokens (e.g., two-factor authentication)\n");
    printf("3. Base58 - Cryptocurrencies, e.g., Bitcoin\n");
    printf("4. Base62 - Links, URLs, unique identifier generation\n");
    printf("5. Base64 - Standard encoding (email, API, images)\n");
    printf("6. Base85 - PDF, PostScript, data compression\n");

    while (1) {
        printf("Enter the algorithm number (1-6): ");
        if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= 6) {
            break;
        } else {
            printf("Invalid input, please try again.\n");
            while (getchar() != '\n');  // Очистка буфера
        }
    }

    // Выделение памяти для закодированных данных
    encoded_data = (unsigned char*)malloc(file_size * 2);  // Пример для Base16 (можно изменить в зависимости от алгоритма)
    if (!encoded_data) {
        perror("Memory allocation error for encoded data");
        return NULL;
    }

    // Выбор алгоритма
    switch (choice) {
        case 1:
            encoded_data = base16_encode(file_data, file_size, encoded_data);
            *dot_output = ".base16";
            printf("Base16 worked\n");
            break;
        case 2:
            encoded_data = base32_encode(file_data, file_size, encoded_data);
            *dot_output = ".base32";
            printf("Base32 worked\n");
            break;
        case 3:
            encoded_data = base58_encode(file_data, file_size, encoded_data);
            *dot_output = ".base58";
            printf("Base58 worked\n");
            break;
        case 4:
            encoded_data = base62_encode(file_data, file_size, encoded_data);
            *dot_output = ".base62";
            printf("Base62 worked\n");
            break;
        case 5:
            encoded_data = base64_encode(file_data, file_size);
            *dot_output = ".base64";
            printf("Base64 worked\n");
            break;
        case 6:
            encoded_data = base85_encode(file_data, file_size);
            *dot_output = ".base85";
            printf("Base85 worked\n");
            break;

        default:
            printf("Please select the algorithm correctly\n");
            free(encoded_data);
            return NULL;
    }

    return encoded_data;
}



// Функция получения имени выходного файла
char *create_output_name(const char* name_input, const char* dot_output) {
/**
 * @brief Создает имя выходного файла
 * 
 * @param name_input Имя входного файла
 * @param dot_output Расширение для выходного файла
 * @return char* Имя выходного файла (нужно освободить) или NULL при ошибке
 * 
 * @warning Выделяет память, которую нужно освободить через free()
 */
    // Выделяем память для строки, длина которой равна длине имени входного файла + длина расширения + 1 для символа \0
    size_t length = strlen(name_input) + strlen(dot_output) + 1;
    char* output_name = (char*)malloc(length * sizeof(char)); // выделяем память
    if (output_name == NULL) {// Ошибка выделения памяти
        return NULL;
    }

    // Копируем имя входного файла в output_name
    strcpy(output_name, name_input);
    // Добавляем расширение
    strcat(output_name, dot_output);

    return output_name;
}




// Функция получения названия алгоритма, которым надо декодировать
char* decode_input_name(const char* name_input) {
/**
 * @brief Определяет алгоритм декодирования из имени файла
 * 
 * @param name_input Имя файла
 * @return char* Название алгоритма (нужно освободить) или NULL при ошибке
 * 
 * @warning Выделяет память, которую нужно освободить через free()
 */
    size_t length = strlen(name_input);
    int num_dots = 0;
    int last_dot_index = -1;

    // Находим индекс последней точки
    for (size_t i = 0; i < length; i++) {
        if (name_input[i] == '.') {
            num_dots++;
            last_dot_index = i;
        }
    }
    // Если точек меньше двух, возвращаем NULL
    if (num_dots < 2) {
        return NULL;
    }

    size_t algorithm_length = length - (last_dot_index + 1);
    char* algorithm = (char*)malloc(algorithm_length + 1);
    if (!algorithm) {
        perror("Memory allocation error");
        return NULL;
    }

    // Копируем название алгоритма
    strncpy(algorithm, name_input + last_dot_index + 1, algorithm_length);
    algorithm[algorithm_length] = '\0'; // Добавляем завершающий ноль

    return algorithm;
}




// Функция считывания данных из файла, который нужно декодировать 
unsigned char* open_file_to_decod(char* filepath_decoded) {
/**
 * @brief Читает файл для декодирования
 * 
 * @param filepath_decoded Путь к файлу
 * @return unsigned char* Данные файла (нужно освободить) или NULL при ошибке
 * 
 * @warning Выделяет память, которую нужно освободить через free()
 */
    FILE *decod_file = fopen(filepath_decoded, "r");
    if (!decod_file){
        perror("Error opening file to decode.\n");
        fclose(decod_file);
        return NULL;
    }

    fseek(decod_file, 0, SEEK_END);
    size_t file_size = ftell(decod_file);  // Получаем размер файла
    rewind(decod_file);
    
    // Выделяем память для данных файла
    unsigned char* buffer = (unsigned char*)malloc(file_size);
    if (buffer == NULL) {
        perror("Memory allocation error");
        fclose(decod_file);
        return NULL;
    }
    
    // Чтение файла в буфер
    fread(buffer, 1, file_size, decod_file);
    fclose(decod_file);

    return buffer;
}



// Функция определения алгоритма декодирования
char* url_to_decod_algorithm(const unsigned char* file_decode_data, const char* algorithm, size_t* file_size) {
/**
 * @brief Декодирует данные с использованием указанного алгоритма
 * 
 * @param file_decode_data Данные для декодирования
 * @param algorithm Алгоритм декодирования
 * @param file_size Указатель на размер данных (обновляется после декодирования)
 * @return char* Декодированные данные (нужно освободить) или NULL при ошибке
 * 
 * @warning Выделяет память, которую нужно освободить через free()
 */
    if (!file_decode_data || !algorithm || *file_size == 0) {
        fprintf(stderr, "Invalid input parameters\n");
        return NULL;
    }

    // Выделяем память (максимально возможный размер, если потребуется)
    char* decoded_data = NULL;
    size_t decoded_length = 0;
    
    size_t file_length;
    if (strcmp(algorithm, "base16") == 0) {
        // Проверка на четность длины входных данных
        if (*file_size % 2 != 0) {
            fprintf(stderr, "Error: Input length must be even for Base16 decoding.\n");
            return NULL; // или другая обработка ошибки
        }
    
        // Выделение памяти для выходных данных
        decoded_data = malloc(*file_size / 2);
        if (decoded_data == NULL) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            return NULL; // или другая обработка ошибки
        }
    
        // Декодирование
        if (base16_decode(file_decode_data, *file_size, decoded_data) == NULL) {
            free(decoded_data); // Освобождаем память в случае ошибки
            return NULL; // или другая обработка ошибки
        }
    
        // Обновление размера данных
        *file_size = *file_size / 2;
    }
    else if (strcmp(algorithm, "base32") == 0) {
        decoded_data = base32_decode(file_decode_data, *file_size, &decoded_length);
        *file_size = decoded_length; // Обновляем размер файла
    } 
    else if (strcmp(algorithm, "base58") == 0) {
        decoded_data = base58_decode(file_decode_data, *file_size, &decoded_length);
        *file_size = decoded_length; // Обновляем размер файла
    } 
    else if (strcmp(algorithm, "base62") == 0) {
        decoded_data = base62_decode(file_decode_data, *file_size, &decoded_length);
        *file_size = decoded_length; // Обновляем размер файла
    }
    else if (strcmp(algorithm, "base64") == 0) {
        decoded_data = base64_decode(file_decode_data, *file_size, &decoded_length);
        *file_size = decoded_length; // Обновляем размер файла

    } 
    else if (strcmp(algorithm, "base85") == 0) {
        decoded_data = base85_decode(file_decode_data, *file_size, &decoded_length);
        *file_size = decoded_length; // Обновляем размер файла

    } 
    else {
        fprintf(stderr, "Unknown algorithm: %s\n", algorithm);
        free(decoded_data);
        return NULL;
    }

    if (!decoded_data) {
        fprintf(stderr, "%s decoding failed\n", algorithm);
        return NULL;
    }

    return decoded_data;  // Возвращаем расшифрованные данные
}



// Функция удаления расширения
char* clear_decoded_name(unsigned char* filename) {
/**
 * @brief Удаляет расширение из имени файла
 * 
 * @param filename Имя файла
 * @return char* Имя файла без расширения
 */
    // Находим последнюю точку в имени файла
    char *last_dot = strrchr(filename, '.');
    if (last_dot) {
        // Обрезаем строку до последней точки
        *last_dot = '\0';
    }
    return filename;
}



// Функция считывания данных из файла, который нужно декодировать
char* read_decode(const char *filename, size_t *file_size){
/**
 * @brief Читает файл для декодирования
 * 
 * @param filename Имя файла
 * @param file_size Указатель для записи размера файла
 * @return char* Данные файла (нужно освободить) или NULL при ошибке
 * 
 * @warning Выделяет память, которую нужно освободить через free()
 */
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);  // Получаем размер файла
    rewind(file);
    
    // Выделяем память для данных файла
    unsigned char* buffer = (unsigned char*)malloc(*file_size);
    if (buffer == NULL) {
        perror("Memory allocation error");
        fclose(file);
        return NULL;
    }
    
    // Чтение файла в буфер
    fread(buffer, 1, *file_size, file);
    fclose(file);
    return buffer;
}



int main(void) {
/**
 * @brief Главная функция программы
 * 
 * @return int Код завершения программы
 * 
 * @note Предоставляет интерфейс для выбора между кодированием и декодированием
 */
    printf("Encode / Decode: ");
    char ans[10];
    char output_dir[] = "output/";
    scanf("%s", &ans);
    if (strcmp(ans, "Encode") == 0)
    {
        char filepath[256]; // Выделяем память для хранения пути к файлу
        uint8_t choice; //Выбор алгоритма кодирования
        size_t file_size;
        // Ввод пути к файлу
        printf("Enter the file path: ");
        scanf("%s", filepath);

        // Извлекаем имя файла (для информации)
        const char* file_encode_name = get_filename(filepath);
        if (!file_encode_name){
            perror("Error getting file name");
            return 1;
        }
        char* extension = extension_definition(file_encode_name);
        if (!extension){
            perror("Error getting file extension.");
            return 1;
        }
        
        char* dot_output = NULL;
        printf("File name: %s\n", file_encode_name);

        char* file_data = read_file_as_bytes(filepath, &file_size);// Преобразование файла в байтовую строку
        if (!file_data) {
            perror("File reading error.\n");
            return 1;
        }

        unsigned char* encoded_data = choice_of_alg(file_data, file_size, &dot_output);// Выбираем алгоритм кодирования
        if (!encoded_data) {
            perror("File encoding error.\n");
            free(file_data);
            return 1;
        }

        char* output_n = create_output_name(file_encode_name, dot_output);// Получение имени 
        if (!output_n) {
            perror("Error creating output file name.\n");
            free(file_data);
            free(encoded_data);
            return 1;
        }
        char output_name[256];
        snprintf(output_name, sizeof(output_name), "%s%s", output_dir, output_n);

        if (encoded_data) {
            printf("The file has been successfully encoded!\n", encoded_data);
            FILE *output = fopen(output_name, "w");
            if (output){
                size_t encoded_data_len = strlen(encoded_data);
                fwrite(encoded_data, 1, encoded_data_len, output);
                fclose(output);
            }
            else{
                perror("Error writing to file.\n");
            }
            free(encoded_data); // Освобождаем память после использования    
        } else {
            printf("File encoding error.\n");
        }
    }

    else if (strcmp(ans, "Decode") == 0){
        char filepath_decode[256]; // Буфер для пути к файлу
        size_t file_size;
        printf("Enter the path to the file: ");
        scanf("%255s", filepath_decode); // Читаем путь к файлу

        const char* file_decode_name = get_filename(filepath_decode); // Получаем имя файла
        char* algorithm = decode_input_name(file_decode_name); // Получаем алгоритм

        if (algorithm) {
            printf("Algorithm: %s\n", algorithm);
        } else {
            perror("Unable to determine algorithm.\n");
            free(algorithm); // Освобождаем память
            return 1; // Завершаем программу с ошибкой
        }

        unsigned char* file_decode_data = read_decode(filepath_decode, &file_size); // Получаем внутренность закодированного файла
        if (!file_decode_data){
            perror("Error reading data");
            return 1;
        }

        unsigned char* decoded_data = url_to_decod_algorithm(file_decode_data, algorithm, &file_size);
        if (decoded_data) {
            char output_name[256];
            char *final_name = strdup(file_decode_name);
            if (!final_name) {
                perror("Error allocating memory for final_name");
                return 1;
            }
            final_name = clear_decoded_name(final_name);

            snprintf(output_name, sizeof(output_name), "%s%s", output_dir, final_name);
            printf("The file has been successfully decoded!\n", decoded_data);
            FILE *output = fopen(output_name, "wb");
            if (output){
                fwrite(decoded_data, 1, file_size, output);
                fclose(output);
            }
            else{
                perror("Error writing to file.\n");
            }
            free(decoded_data); // Освобождаем память после использования    
        } else {
            printf("File decoding error.\n");
        }
        
    }   
    else{
        printf("Incorrect algorithm name.\n");
    }
    printf("\n");
    printf("The program is completed.\n");


    return 0;
}

// 467 + 195 + 397 + 24 + 24 = 1107