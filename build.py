"""Compiles all files in src"""

import subprocess
import os
import sys
import shutil

compiler = "g++"
debug_flags = "-std=c++23 -Wall -Wno-c++98-compat -Wno-padded"
release_flags = "-std=c++23 -Wall -Wno-c++98-compat -Wno-padded -O3 -flto=auto"

project_name = "tiny-calc"
units = [
    "chunk",
    "compile",
    "interpret",
    "main",
    "repl",
    "report",
    "tokenize",
]

sccache = "sccache"
if shutil.which("sccache") is None:
    sccache = ""
    print("Warning: sccache is not installed, incremental compiles will be slow")

use_mold = True
if shutil.which("mold") is None:
    use_mold = False
    print("Warning: mold is not installed, linking will be slower")

# Generate compile command
with open("COMPILE.txt", "w", encoding="ascii") as file:
    files = ' '.join(map(lambda unit : f"src/{unit}.cpp", units))
    file.write(
        f"{compiler} {files} {release_flags} -o {project_name}\n"
    )

if not os.path.exists("build"):
    os.makedirs("build")

# Compile each module into an object file
error = False
for unit in units:
    cmd = f"{sccache} {compiler} -c {debug_flags} -fdiagnostics-color src/{unit}.cpp -o build/{unit}.o"
    result = subprocess.call(cmd, shell=True)
    if result != 0:
        error = True

# Link object files into executable
if not error:
    paths = " ".join([f"build/{unit}.o" for unit in units])
    mold = "-fuse-ld=mold" if use_mold else ""
    cmd = f"{sccache} {compiler} {paths} -o {project_name} {mold}"
    subprocess.call(cmd, shell=True)
else:
    sys.exit(-1)
