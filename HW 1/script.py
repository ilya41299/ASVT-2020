"""
Вход
Файл 1
Матрица без существенных переменных
(в строке 1 где импликант покрывает столбец)
Файл 2
Ипликанты в том же порядке, что и в строках матрицы

ПРИМЕР

file1

01010
00011
10100
11100
01010
00100
00011

file2

~1~010
~1101~
1~0~00
110~~0
11~~10
11~1~0
111~1~'
"""

from sympy.logic import simplify_logic
from sympy.logic import to_dnf
import numpy as np
import re
import sys
import itertools


def petrick_method(mat, impl_list):
    symbols_tup = tuple(chr((ord('a') + i)) for i in range(len(mat)))
    col_mat = mat.swapaxes(0, 1)
    cnf = ['(' + ' | '.join(itertools.compress(symbols_tup, col)) + ')' for col in col_mat]
    mul_of_cnf = ' & '.join(cnf)
    sim_dnf = str(to_dnf(simplify_logic(mul_of_cnf)))
    min_combo = re.sub(r'[()\s]', '', sim_dnf[:sim_dnf.find('|')]).split('&')
    add_implicants = [impl_list[ord(ch) - ord('a')] for ch in min_combo]
    print(add_implicants)


if __name__ == '__main__':
    mat_file, impl_file = sys.argv[1], sys.argv[2]
    with open(mat_file) as f:
        matrix = np.array([[int(x) for x in line.strip()] for line in f.readlines()])
    with open(impl_file) as f:
        impls = [line.strip() for line in f.readlines()]
    petrick_method(matrix, impls)
