"""Compiles all files in src"""

import subprocess
import os
import sys

project_name = "tiny-calc"
flags = "-std=c++23 -fdiagnostics-color -Wall -Wno-c++98-compat -Wno-padded" # -O3
units = [
    "chunk",
    "compile",
    "interpret",
    "main",
    "repl",
    "report",
    "tokenize",
    "utf8",
]

with open("COMPILE.txt", "w", encoding="ascii") as file:
    file.write("g++ ")
    for unit in units:
        file.write(f"src/{unit}.cpp ")
    file.write(f"{flags} ")
    file.write(f"-o {project_name}\n")

if not os.path.exists("build"):
    os.makedirs("build")

error = False
for unit in units:
    cmd = f"sccache g++ -c {flags} src/{unit}.cpp -o build/{unit}.o"
    result = subprocess.call(cmd, shell = True)
    if result != 0:
        error = True

if not error:
    paths = " ".join([f"build/{unit}.o" for unit in units])
    cmd = f"sccache g++ {paths} -o {project_name}"
    subprocess.call(cmd, shell=True)
else:
    sys.exit(-1)
