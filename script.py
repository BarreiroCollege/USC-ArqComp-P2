#!/bin/python3

import math
import os
import subprocess

NUM_PRUEBAS = 5
ARCHIVO_LECTURA = "out.txt"
ARCHIVO_SALIDA = "out.csv"

# CODIGO = "1_normal"
# CODIGO = "2_unrolling"
# CODIGO = "2_loops"
# CODIGO = "2_all"
CODIGO = "3_simd"
# CODIGO = "4_openmp"

Os = ["O0"]
# Os = ["O0", "O2"]
Ns = [250, 500, 750, 1000, 1500, 2000, 2550, 3000]
FLAGS = []
# FLAGS = ["-fopenmp"]


def guardar_linea(O, N, T):
    f = open(ARCHIVO_SALIDA, "a")
    f.write(str(O) + "," + str(N) + "," + ",".join(T) + "\n")
    f.close()

def ejecutar(ej, O, N):
    print("O=%-10s N=%-10s" % (str(O), str(N)))
    T = [0] * NUM_PRUEBAS
    for i in range(0, NUM_PRUEBAS):
        subprocess.run(["./" + ej, str(N)], stdout=subprocess.DEVNULL)
        f = open(ARCHIVO_LECTURA, "r")
        T[i] = f.read()
        f.close()
    guardar_linea(O, N, T)

def compilar():
    for O in Os:
        compilado = CODIGO + "_" + O
        subprocess.run(["gcc", "-Wall", "-" + O, " ".join(FLAGS), CODIGO + ".c", "-o", compilado])
        for N in Ns:
            ejecutar(compilado, O, N)


if __name__ == "__main__":
    f = open(ARCHIVO_SALIDA, 'w')
    f.write("F,C,")
    for i in range(1, NUM_PRUEBAS + 1):
        if i > 1:
            f.write(",")
        f.write("T" + str(i))
    f.write("\n")
    f.close()

    compilar()