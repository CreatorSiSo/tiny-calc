"""Check all test cases and generate TEST.txt"""

from __future__ import annotations

import json
import os
import subprocess
from itertools import chain, repeat
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
    def __init__(
        self, title: str, description: str, args: str, input: list[str], expected: str
    ):
        self.title = title
        self.description = description
        self.args = args
        self.input = input
        self.expected = expected

    @staticmethod
    def from_str(input: str, path: Path) -> Test:
        second_separator = input.find("---", 3)

        if second_separator == -1:
            print(
                "\n"
                f"'{path}': Invalid test file format\n"
                "    Missing front matter seperator\n"
            )
            exit(-1)

        front_matter_string = (
            "{" + ",".join(input[3:second_separator].strip().splitlines()) + "}"
        )

        front_matter = json.loads(front_matter_string)

        return Test(
            title=front_matter["title"],
            description=front_matter["description"],
            args=front_matter["args"],
            input=front_matter["input"],
            expected=input[second_separator + 4 :],
        )

    def to_str(self) -> str:
        return (
            "---\n"
            f'"title": "{self.title}"\n'
            f'"description": "{self.description}"\n'
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


def validate_snapshot(path: Path) -> Test:
    print(f"\nRunning '{path}':")

    test = Test.from_str(path.read_text(), path)
    returncode, stdout = run_test(test)

    if test.expected == stdout:
        print("✔ Success")
        return test

    # print(f"  Command: '{cmd}'")
    # print(f"  ReturnCode: {returncode}")
    # print(f"  Input: {test.input}")
    # print(f"  Expected: '{escape(test.expected)}'")
    # print(f"  Stdout: '{escape(stdout)}'")
    print("✗ Failure")

    new_test = Test(
        title=test.title,
        description=test.description,
        args=test.args,
        input=test.input,
        expected=stdout,
    )
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

    return new_test


def combine_input_output(test: Test) -> str:
    if test.expected.find(">> ") == -1:
        return test.expected

    lines = test.expected.split(">> ")
    input = chain(map(lambda input: input + "\n", test.input), repeat(""))

    with_input = map(
        lambda zipped: zipped[0] + ">> " + zipped[1],
        zip(lines, input),
    )

    return "".join(with_input).rstrip(">> ")


def main():
    with open("TEST.txt", mode="w") as test_docs:
        test_docs.write(
            "# Tests\n\n"
            "This file was auto generated by the `test.py` script.\n"
            "Snapshot tests can be found in the `tests` folder.\n"
            "The output of a test combines `stdin`, `stdout` and `stderr` in one string.\n\n\n"
        )

        for path in test_files("tests"):
            test = validate_snapshot(path)

            description = (
                f"{test.description}\n\n"
                if len(test.description.strip()) > 0
                else "" ""
            )
            inputs = ", ".join(
                map(lambda input: f'"{escape(input + "\n")}"', test.input)
            )
            output = combine_input_output(test)

            test_docs.write(
                f"## {test.title}\n\n"
                f"{description}"
                f"Command: tiny-calc {test.args}\n"
                f"Inputs: [{inputs}]\n"
                f"Output:\n"
                "```\n"
                f"{output}\n"
                "```\n\n"
            )


main()
