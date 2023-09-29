### Решение определенного интеграла методом трапеций.

$\int_{a}^{b} {4 \over 1 + x^2} dx$

Запуск исполняемого файла происходит с использованием p процессов.  

"Основной" процесс(процесс с номером 0) разбивает отрезок $[0;1]$ на $N$ малых отрезков длиной $\Delta x$(шаг интегрирования), и вычисляет с этим разбиением интеграл в последовательном варианте. Далее этот же процесс разбивает отрезок $[0;1]$, состоящий из $N$ малых отрезков, на $p$ частей и границы каждой из оставшихся $(p - 1)$ частей рассылает остальным $(p - 1)$ процессам (с одной из частей отрезка работает "основной" процесс). Число $N$ может меняться и задается пользователем.  

Каждый из процессов, получивших свои границы части отрезка, вычисляет свою часть интеграла $I_i$ и отправляет ее «основному» процессу.  

«Основной» процесс получает все части интеграла от процессов-рабочих и, складывая их, получает исходный интеграл $I$.

Компиляция программы:
```
mpic++ main.cpp 
```

Запуск программы:
```
mpiexec -n <num_of_processes> ./a.out <num_of_segments>
```
Или через скрипт "run.sh".

Программа замеряет время работы последовательной и параллельной частей и записывает полученные результаты в файл "time.txt". Скрипт "benchmark.py" производит нужное количество запусков программы, читает данные из файла "time.txt" и строит графики производительности.