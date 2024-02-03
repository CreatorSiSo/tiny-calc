"""Check all test cases and generate TEST.txt"""

import os
import json
import subprocess


def files_in_dir(directory: str) -> list[str]:
    return [
        file
        for file in map(
            lambda file: os.path.join(directory, file), os.listdir(directory)
        )
        if os.path.isfile(file)
    ]


class Test:
    def __init__(self, args: str, input: list[str], expected: str):
        self.args = args
        self.input = input
        self.expected = expected


def read_test(test_config: str) -> Test:
    second_separator = test_config.find("---", 3)

    if second_separator == -1:
        print(
            f"<{path}>: Invalid test file format\n" "    Missing front matter seperator"
        )
        exit(-1)

    front_matter_string = (
        "{" + ",".join(test_config[3:second_separator].strip().splitlines()) + "}"
    )

    front_matter = json.loads(front_matter_string)

    return Test(
        front_matter["args"],
        front_matter["input"],
        test_config[second_separator + 4 :],
    )


def run_test(test: Test):
    cmd = f"./tiny-calc {test.args}"
    child = subprocess.Popen(
        cmd,
        shell=True,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    for line in test.input:
        child.stdin.write((line + "\n").encode("utf8"))
    child.stdin.close()
    child.wait()
    stdout = child.stdout.read().decode("utf8")

    def escape(input: str) -> str:
        return input.encode("unicode_escape").decode("utf8")

    if test.expected == stdout:
        print("✔ Success")
    else:
        print(f"  Command: '{cmd}'")
        print(f"  ReturnCode: {child.returncode}")
        print(f"  Input: {test.input}")
        print(f"  Expected: '{escape(test.expected)}'")
        print(f"  Stdout: '{escape(stdout)}'")
        print("✗ Failure")
        exit(-1)


for path in files_in_dir("tests"):
    print(f"\nRunning '{path}':")

    with open(path) as file:
        test = read_test(file.read())
        run_test(test)
