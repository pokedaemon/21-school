#!/bin/bash

function mem_test {
    leaks --atExit ./s21_grep "$@" | grep leaks # doesn't work yet (inf loop)
}

function sim_test {
    grep "$@" > .tmp_grep 2> /dev/null
    ./s21_grep "$@" > .tmp_s21_grep 2> /dev/null

    if diff .tmp_grep .tmp_s21_grep > .diff_out; then
        echo "test passed: grep $@" 
    else
        echo "test failed: grep $@"
        cat .diff_out
    fi
    
    rm .tmp_grep .tmp_s21_grep
}

# Similarity tests

make # build s21_grep

# Обычный поиск текста в файле
sim_test -e "root" /etc/passwd

# Поиск без учёта регистра
sim_test -i -e "root" /etc/passwd

# Подсчёт количества совпадений
sim_test -c -e "root" /etc/passwd;

# Вывод номеров строк с совпадениями
sim_test -n -e "root" /etc/passwd

# Поиск строк, не содержащих шаблон
sim_test -v -e "root" /etc/passwd

# Тесты с несколькими файлами
sim_test -e "root" /etc/passwd /etc/group
sim_test -i -e "root" /etc/passwd /etc/group
sim_test -v -e "root" /etc/passwd /etc/group
sim_test -c -e "root" /etc/passwd /etc/group
sim_test -l -e "root" /etc/passwd /etc/group
sim_test -n -e "root" /etc/passwd /etc/group

# Тест на несуществующий файл
sim_test -e "pattern" non_existing_file.txt

# Тест комбинированных флагов (часть 3)
sim_test -i -v -e "root" /etc/passwd /etc/group
sim_test -i -n -e "root" /etc/passwd /etc/group

# Memory leaks tests

# Обычный поиск текста в файле
sim_test -e "root" /etc/passwd

# Поиск без учёта регистра
sim_test -i -e "root" /etc/passwd

# Подсчёт количества совпадений
sim_test -c -e "root" /etc/passwd;

# Вывод номеров строк с совпадениями
sim_test -n -e "root" /etc/passwd

# Поиск строк, не содержащих шаблон
sim_test -v -e "root" /etc/passwd

# Тесты с несколькими файлами
sim_test -e "root" /etc/passwd /etc/group
sim_test -i -e "root" /etc/passwd /etc/group
sim_test -v -e "root" /etc/passwd /etc/group
sim_test -c -e "root" /etc/passwd /etc/group
sim_test -l -e "root" /etc/passwd /etc/group
sim_test -n -e "root" /etc/passwd /etc/group

# Подготовка файлов для тестов (можно в начале скрипта)
echo -e "foo bar foo\nbar foo bar\nfoobar" > testfile.txt
echo "foo bar" > file1.txt
echo "bar foo" > file2.txt

# Тест - вывод совпадений 'foo'
sim_test -o -e "foo" testfile.txt

# Тест - регистронезависимый поиск с выводом совпадений
sim_test -o -i -e "foo" testfile.txt

# Тест - совпадения из нескольких файлов
sim_test -o -e "foo" file1.txt file2.txt

# Тест - нет совпадений для паттерна
sim_test -o -e "nonexistentpattern" testfile.txt

rm testfile.txt file1.txt file2.txt .diff_out

