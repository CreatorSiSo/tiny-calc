"""Check all test cases and generate TEST.txt"""

from __future__ import annotations
import os
import json
import subprocess
from pathlib import Path


def test_files(directory: str) -> list[Path]:
    return [
        file
        for file in map(lambda file: Path(directory, file), os.listdir(directory))
        if file.is_file() and file.suffix == ".snap"
    ]


def escape(input: str) -> str:
    return input.encode("unicode_escape").decode("utf8")


class Test:
    def __init__(self, args: str, input: list[str], expected: str):
        self.args = args
        self.input = input
        self.expected = expected

    @staticmethod
    def from_str(input: str) -> Test:
        second_separator = input.find("---", 3)

        if second_separator == -1:
            print(
                f"<{path}>: Invalid test file format\n"
                "    Missing front matter seperator"
            )
            exit(-1)

        front_matter_string = (
            "{" + ",".join(input[3:second_separator].strip().splitlines()) + "}"
        )

        front_matter = json.loads(front_matter_string)

        return Test(
            front_matter["args"],
            front_matter["input"],
            input[second_separator + 4 :],
        )

    def to_str(self) -> str:
        return (
            "---\n"
            f'"args": "{self.args}"\n'
            f'"input": {json.dumps(self.input)}\n'
            "---\n"
            f"{self.expected}"
        )


def run_test(test: Test) -> tuple[int, str]:
    cmd = f"./tiny-calc {test.args}"
    with subprocess.Popen(
        cmd,
        shell=True,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    ) as child:
        for input in test.input:
            child.stdin.write(input + "\n")
        child.stdin.close()
        child.wait()

        output = child.stdout.read()
        return child.returncode, output


for path in test_files("tests"):
    print(f"\nRunning '{path}':")

    test = Test.from_str(path.read_text())
    returncode, stdout = run_test(test)

    if test.expected == stdout:
        print("✔ Success")
        continue

    # print(f"  Command: '{cmd}'")
    # print(f"  ReturnCode: {returncode}")
    # print(f"  Input: {test.input}")
    # print(f"  Expected: '{escape(test.expected)}'")
    # print(f"  Stdout: '{escape(stdout)}'")
    print("✗ Failure")

    new_test = Test(test.args, test.input, stdout)
    new_path = path.with_suffix(".snap.new")
    Path(new_path).write_text(new_test.to_str())
    subprocess.run(
        f"PAGER='' git diff --no-index {path} {new_path}",
        shell=True,
    )

    answer = input("Accept this change? [Y/n]: ").strip().lower()
    if answer == "n":
        new_path.unlink()
    else:
        new_path.rename(path)
