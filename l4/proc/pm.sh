#!/bin/bash

# Проверяем, передан ли PID как аргумент
if [ $# -eq 0 ]; then
    echo "Usage: $0 <pid>"
    exit 1
fi

pid=$1

# Проверяем, существует ли файл maps для указанного PID
if [ ! -f "/proc/$pid/maps" ]; then
    echo "No maps file found for PID $pid"
    exit 1
fi

# Читаем файл /proc/pid/maps и обрабатываем каждую строку
while IFS= read -r line; do
    # Извлекаем начальный адрес до символа '-'
    addr=$(echo "$line" | awk -F'-' '{print $1}')
    # Вызываем программу ./pm с PID и адресом памяти
    echo "Running: ./pm $pid $addr"
    ./pm "$pid" "$addr"
    echo -e ""
done < "/proc/$pid/maps"

# # Скрипт читает данные из стандартного ввода
# while IFS= read -r line; do
#     # Проверяем, содержит ли строка 'pid=' и извлекаем значение PID
#     # if [[ "$line" =~ ^pid=([0-9]+)$ ]]; then
#     if [[ "$line" =~ ^([0-9]+)$ ]]; then
#         pid=${BASH_REMATCH[1]}
#         echo -e "PID: $pid\n"
#     else
#         # Извлекаем адреса памяти до символа '-'
#         addr=$(echo "$line" | awk -F'-' '{print $1}')
#         # Вызываем программу ./pm с PID и адресом памяти
#         echo "Running: ./pm $pid $addr"
#         ./pm "$pid" "$addr"
#         echo -e ""
#     fi
# done
