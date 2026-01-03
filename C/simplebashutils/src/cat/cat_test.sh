#!/bin/bash

function sim_test {
    ./s21_cat "$@" > .s21_cat_out 2> .s21_cat_err
    cat "$@" > .cat_out 2> .cat_err

    if diff -s .s21_cat_out .cat_out && diff .s21_cat_err .cat_err; then
        echo "test passed! w/ args: $@"
    else
        echo "test failed! w/ args: $@"
    fi

    rm .s21_cat_out .s21_cat_err .cat_out .cat_err
}

echo -e "This is a line.\n\nThis is another line.\n\nYet another line." > file1.txt

echo -e "Line with\ttab\tcharacter.\nLine with multiple\t  spaces.\n\tIndented line with tab.\nLine with trailing spaces.   " > file2.txt

echo -e "Line with non-ASCII: ñ, ü, å.\nLine with control chars: \x01\x02\x03.\nNormal text line." > file3.txt

make

# пустой файл
touch empty.txt

# Тесты без опций (просто сравнение вывода)
sim_test file1.txt
sim_test file2.txt
sim_test file3.txt

# По одному и по несколько флагов
sim_test -b file1.txt
sim_test -e file2.txt
sim_test -n file3.txt
sim_test -s file1.txt
sim_test -t file2.txt
sim_test -v file3.txt

sim_test -b -e file1.txt
sim_test -n -s file2.txt
sim_test -t -v file3.txt

sim_test -b -n -s file1.txt file2.txt
sim_test -e -t -v file2.txt file3.txt

# Проверка нескольких файлов сразу
sim_test -n file1.txt file2.txt file3.txt
sim_test -s -v file1.txt file3.txt

# Проверка опций с файлом, который содержит только пустые строки
touch empty.txt
sim_test -b empty.txt
sim_test -e empty.txt
sim_test -n empty.txt
sim_test -s empty.txt
sim_test -t empty.txt
sim_test -v empty.txt

# Проверка несуществующего файла (ошибки)
sim_test -b nofile.txt
sim_test file1.txt nofile.txt

# Проверка опций с выводом ошибок
sim_test -b -e -n -s -t -v file1.txt

rm file1.txt file2.txt file3.txt empty.txt
rm s21_cat