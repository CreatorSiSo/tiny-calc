"""Compiles all files in src"""

import subprocess
import os
import sys

compiler = "g++"
wrapper = "sccache"
flags = "-std=c++23 -Wall -Wno-c++98-compat -Wno-padded" # -O3

project_name = "tiny-calc"
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

# Generate compile command
with open("COMPILE.txt", "w", encoding="ascii") as file:
    file.write(
        f"{compiler} {' '.join(map(lambda unit : f"src/{unit}.cpp", units))}"
        f" {flags} -o {project_name}\n"
    )

if not os.path.exists("build"):
    os.makedirs("build")

# Compile each module into an object file
error = False
for unit in units:
    cmd = f"{wrapper} {compiler} -c {flags} -fdiagnostics-color src/{unit}.cpp -o build/{unit}.o"
    result = subprocess.call(cmd, shell = True)
    if result != 0:
        error = True

# Link object files into executable
if not error:
    paths = " ".join([f"build/{unit}.o" for unit in units])
    cmd = f"{wrapper} {compiler} {paths} -o {project_name}"
    subprocess.call(cmd, shell=True)
else:
    sys.exit(-1)
