#!/bin/bash

# Проверяем наличие компилятора
if ! command -v gcc &> /dev/null
then
    echo "GCC не найден. Установите его перед продолжением."
    exit 1
fi

# Создаем папку output, если ее нет
mkdir -p output

# Компилируем проект
gcc -Wall -Wextra -std=c99 -Iinclude src/encod_func.c src/decod_func.c src/tables.c src/main.c -o output/main

if [ $? -ne 0 ]; then
    echo "Ошибка компиляции"
    exit 1
fi

echo "Сборка успешно завершена"
echo "Запуск программы..."
./output/main