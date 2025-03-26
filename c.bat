@echo off
setlocal enabledelayedexpansion

:: Проверяем наличие компилятора
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo GCC не найден. Убедитесь, что MinGW установлен и добавлен в PATH
    pause
    exit /b 1
)

:: Компилируем все исходные файлы
gcc -Wall -Wextra -std=c99 -Iinclude src/encod_func.c src/decod_func.c src/tables.c src/main.c -o main

if %errorlevel% neq 0 (
    echo Ошибка компиляции
    pause
    exit /b 1
)

echo Сборка успешно завершена
echo Запуск программы...
main.exe
pause