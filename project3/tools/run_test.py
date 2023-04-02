from io import StringIO
from contextlib import redirect_stdout
from pathlib import Path
import argparse
import csv
import hashlib
import os
import re
import subprocess
import sys
import textwrap
import time
import traceback

from diff_output import diff_output
from fetch_encoding import update_imm_circ


CIRC_IMPORT_REGEX = re.compile(r"desc=\"file#([^\"]+?\.circ)\"")

proj_dir_path = Path(__file__).parent.parent
tests_dir_path = proj_dir_path / "tests"
logisim_path = proj_dir_path / "tools" / "logisim-evolution.jar"

banned_component_names = [
    "Pull Resistor",
    "Transistor",
    "Transmission Gate",
    "Power",
    "POR",
    "Ground",
    "Divider",
    "Random",
    "PLA",
    "RAM",
    "Random Generator",
]
known_imports_dict = {
    "cpu/cpu.circ": [
        "cpu/alu.circ",
        "cpu/branch-comp.circ",
        "cpu/control-logic.circ",
        "cpu/imm-gen.circ",
        "cpu/regfile.circ",
    ],
    "harnesses/alu-harness.circ": [
        "cpu/alu.circ",
    ],
    "harnesses/imm-gen-harness.circ": [
        "cpu/imm-gen.circ",
    ],
    "harnesses/branch-comp-harness.circ": [
        "cpu/branch-comp.circ",
    ],
    "harnesses/cpu-harness.circ": [
        "cpu/cpu.circ",
        "cpu/mem.circ",
    ],
    "harnesses/regfile-harness.circ": [
        "cpu/regfile.circ",
    ],
    "harnesses/run.circ": [
        "harnesses/cpu-harness.circ",
    ],
    "tests/unit-alu/*.circ": [
        "cpu/alu.circ",
    ],
    "tests/unit-regfile/*.circ": [
        "cpu/regfile.circ",
    ],
    "tests/unit-partial-load/*.circ": [
        "cpu/partial-load.circ",
    ],
    "tests/integration-*/*.circ": [
        "harnesses/cpu-harness.circ",
    ],
}


def find_banned(circ_path):
    if circ_path.name in ["mem.circ"]:
        return False
    with circ_path.open("r") as f:
        contents = f.read()
    found = False
    for component_name in banned_component_names:
        if re.search(rf'<comp.*\bname="{component_name}"', contents):
            print(
                f"ERROR: found banned element ({component_name}) in {circ_path.as_posix()}"
            )
            found = True
    return found


starter_file_hashes = {
    "harnesses/cpu-harness.circ": "875d04f0180d73d6c7b98a8108f4227b",
    "harnesses/run.circ": "e6749025caccb9a7ff35320adcc3dcf6",
    "tests/integration-addi/addi-basic.circ": "26be508addd9e250793b568739e23b8b",
    "tests/integration-addi/addi-negative.circ": "2c672ef9eede29c49c489f1759482b17",
    "tests/integration-addi/addi-positive.circ": "84a9dc112634a226313aec4e8bd4e834",
    "tests/integration-addi/out/addi-basic.piperef": "5adef575722b93ed04c82a37618ef2eb",
    "tests/integration-addi/out/addi-basic.ref": "4e1cfb0543418d95d3c75b49224c394d",
    "tests/integration-addi/out/addi-negative.piperef": "880daf267a095c2ab6012186be29aea6",
    "tests/integration-addi/out/addi-negative.ref": "8c4f4354e017d347a246b549c0a7c019",
    "tests/integration-addi/out/addi-positive.piperef": "554b05ad4952e65e3ac8844f36c0394e",
    "tests/integration-addi/out/addi-positive.ref": "079f17180d45eb7104220427f6360412",
    "tests/integration-all-regs/all-regs-basic.circ": "0c845f6bbacd46bd8cb36ad725d2ebc3",
    "tests/integration-all-regs/out/all-regs-basic.piperef": "1a294bd256f2900d58a05a30f8cb19a0",
    "tests/integration-all-regs/out/all-regs-basic.ref": "5859036aac504f27418af1dddfb6128e",
    "tests/integration-branch/branch-basic.circ": "dc29bd14820608b5c2f039aa86504b21",
    "tests/integration-branch/out/branch-basic.piperef": "26eeaf22f577024bf2c501f27f032ce9",
    "tests/integration-branch/out/branch-basic.ref": "5673c41c4955ffecf1b6815c6ba244cc",
    "tests/integration-programs/programs-fib.circ": "6f6e57d2eb340bde18a289f79bb3fd14",
    "tests/integration-programs/programs-power.circ": "88f37938a517029ba69d3399bc45a295",
    "tests/integration-programs/out/programs-fib.piperef": "b2723e49f6e2bf889c9c5a26f8a034ec",
    "tests/integration-programs/out/programs-fib.ref": "98b7fd0d0545cd66c747dbf30d0eb114",
    "tests/integration-programs/out/programs-power.piperef": "02108bb7b96a90ef93b2a87afa60ff7b",
    "tests/integration-programs/out/programs-power.ref": "a727b44291c2efc8ed1c319ac5929b99",
    "tests/integration-immediates/immediates-basic.circ": "6e82c5584fc8739cc6ce129c0b6ad976",
    "tests/integration-immediates/out/immediates-basic.piperef": "0d97830b0c73851d9a8b55487e2650ee",
    "tests/integration-immediates/out/immediates-basic.ref": "6da63f3c1b24f300abca821c7b7b7eec",
    "tests/integration-jump/jump-basic.circ": "27097f9c1006ee0a47f099b536b39095",
    "tests/integration-jump/jump-empty.circ": "644238d311124df295ef5ed7b6765088",
    "tests/integration-jump/jump-far.circ": "0adb81c95f5d1debde5896fcdd23d551",
    "tests/integration-jump/out/jump-basic.piperef": "23a000a99fd7da605c7d0956bec7421e",
    "tests/integration-jump/out/jump-basic.ref": "7596edead8499ee707032f40a36d1195",
    "tests/integration-jump/out/jump-empty.piperef": "cfbe380fd5e9e0ea6558e6e58e7ecb8a",
    "tests/integration-jump/out/jump-empty.ref": "fe6d0e0373a367879e4ed244c00dc71d",
    "tests/integration-jump/out/jump-far.piperef": "30565d9c925b08c32f3994f59e0f7189",
    "tests/integration-jump/out/jump-far.ref": "ed18fd0e1ccc3a05c22d60d001a50e15",
    "tests/integration-lui/lui-basic.circ": "a692841f783e7024a59e847ac281b117",
    "tests/integration-lui/out/lui-basic.piperef": "89bc4cda38d269d7174babfd4706d43d",
    "tests/integration-lui/out/lui-basic.ref": "dd77a9439d6e8f1809e2e8d2ec4610ea",
    "tests/integration-mem/mem-load-basic.circ": "068e3b6ea7cd07a0a641fe24bed43ce1",
    "tests/integration-mem/mem-sign-extend.circ": "9402a45caa076cf0104c0b9e342865d3",
    "tests/integration-mem/mem-store-basic.circ": "18a1942202e2d2459781281afb1cdcba",
    "tests/integration-mem/out/mem-load-basic.piperef": "b605abe6bd1179ca65e75ee44a2b4d9f",
    "tests/integration-mem/out/mem-load-basic.ref": "ec15cb3d0bf51001f5afc5926cd25786",
    "tests/integration-mem/out/mem-sign-extend.piperef": "d158338ecfcb249a589deed5a8c8ee99",
    "tests/integration-mem/out/mem-sign-extend.ref": "b7f514f6a8df4f716deff9039f1be3c5",
    "tests/integration-mem/out/mem-store-basic.piperef": "7dc25fcd6505835a734e88226524344d",
    "tests/integration-mem/out/mem-store-basic.ref": "b0bd3f8d5c28aa44a5908b9ce40f6c6f",
    "tests/unit-alu/alu-add.circ": "5aa4d556f4e928400f619176c39b00de",
    "tests/unit-alu/alu-all.circ": "c2ce7e4c3ca3a790edca4c1b7812029b",
    "tests/unit-alu/alu-logic.circ": "91bac4cc56aad4e3923b812d8549d3ad",
    "tests/unit-alu/alu-mult.circ": "7b36344218d7bbe6801a7dec4a131eae",
    "tests/unit-alu/alu-shift.circ": "4fee595a2843f8a4dee63bd4448ca6b1",
    "tests/unit-alu/alu-slt-sub-bsel.circ": "26547a779d769cb86b28695e28877ecd",
    "tests/unit-alu/out/alu-add.ref": "cd03531fa04dbdefbc82bb9c4d3c9ed4",
    "tests/unit-alu/out/alu-all.ref": "97b8eaac983e2774b9b47bff46d41748",
    "tests/unit-alu/out/alu-logic.ref": "e655cd4b64a1d6bf6de02882d3708203",
    "tests/unit-alu/out/alu-mult.ref": "ef42ed4cf4c85efc3f1e7a414e2012b1",
    "tests/unit-alu/out/alu-shift.ref": "09a61bb272d6563dcfa35d76840765ec",
    "tests/unit-alu/out/alu-slt-sub-bsel.ref": "a27b828a9e4d093aaff92b7183cd9fb0",
    "tests/unit-branch-comp/branch-comp-signed.circ": "907b40b5b0446d7be46c4ec57f6884d1",
    "tests/unit-branch-comp/branch-comp-unsigned.circ": "6d0b0f7529872454869b2695a423218a",
    "tests/unit-branch-comp/out/branch-comp-signed.ref": "649019e9690f30cf3a345f4a5e8f2b23",
    "tests/unit-branch-comp/out/branch-comp-unsigned.ref": "3c407d05cf821438e7fd93cf0c24be73",
    "tests/unit-partial-load/out/partial-load-byte.ref": "b06a9f39d91399ada2076dbabda0b044",
    "tests/unit-partial-load/out/partial-load-half.ref": "29eef73a529a567327be8348123dee12",
    "tests/unit-partial-load/out/partial-load-word.ref": "4d30a508d7c07f7eabc67a8dcfe0bffc",
    "tests/unit-partial-load/partial-load-byte.circ": "1d7abe2774c9fb070d91e2479ef3da24",
    "tests/unit-partial-load/partial-load-half.circ": "597a5603455639f48590470f3f4f2d38",
    "tests/unit-partial-load/partial-load-word.circ": "c6960e9864bb13b6551a1f998d5cadf4",
    "tests/unit-partial-store/out/partial-store-byte.ref": "90602d0200dbdc12e37db0323e7baf44",
    "tests/unit-partial-store/out/partial-store-half.ref": "9f7da576870d2578acef009b3b3fc5ad",
    "tests/unit-partial-store/out/partial-store-non-store.ref": "f1cded4242cb0030a4eaabd90e44c51a",
    "tests/unit-partial-store/out/partial-store-word.ref": "03c331ff93622577e349dec9aed01cf2",
    "tests/unit-partial-store/partial-store-byte.circ": "da7fe1a86a7c25b585058a488ff02d1b",
    "tests/unit-partial-store/partial-store-half.circ": "c7cd7b6ca8e40ed83d646a8652892d91",
    "tests/unit-partial-store/partial-store-non-store.circ": "f1a174e17f7ca45d46bbdcaf8fcbbe82",
    "tests/unit-partial-store/partial-store-word.circ": "ddf483cdd60be07248c848e01d0031fe",
    "tests/unit-regfile/out/regfile-more-regs.ref": "130731ddabf0a2c0d93e086cb57acc72",
    "tests/unit-regfile/out/regfile-read-only.ref": "64b6676bbb583d6a20564061cf680501",
    "tests/unit-regfile/out/regfile-read-write.ref": "127daa136c232f0ea623323d85b45972",
    "tests/unit-regfile/out/regfile-x0.ref": "e3025d4271c70ed8a32518fb95de63a8",
    "tests/unit-regfile/regfile-more-regs.circ": "a10b5644d6f2722e1ac00a2eea1994fd",
    "tests/unit-regfile/regfile-read-only.circ": "bb6d9104f12b20884c5ed5d265803fb3",
    "tests/unit-regfile/regfile-read-write.circ": "41f694fd7e6f769cded5abd1068fe16d",
    "tests/unit-regfile/regfile-x0.circ": "b4ab6fa4e3c27b637b2436ebd02426c2",
}


class TestCase:
    def __init__(self, circ_path, name=None):
        self.circ_path = Path(circ_path)
        self.id = str(self.circ_path)
        self.name = name or self.circ_path.stem

    def can_pipeline(self):
        if self.circ_path.match("unit-*/*.circ"):
            return False
        return True

    def fix_circ(self):
        fix_circ(self.circ_path)

    def check_hashes(self, pipelined=False):
        passed, reason = check_hash(self.circ_path)
        if not passed:
            return passed, reason

        passed, reason = check_hash(self.get_expected_table_path(pipelined=pipelined))
        if not passed:
            return passed, reason

        passed, reason = check_hash(proj_dir_path / "harnesses" / "cpu-harness.circ")
        if not passed:
            return passed, reason

        passed, reason = check_hash(proj_dir_path / "harnesses" / "run.circ")
        if not passed:
            return passed, reason

        return (True, "Circuit data matches starter code")

    def get_actual_table_path(self):
        return self.circ_path.parent / "out" / f"{self.name}.out"

    def get_expected_table_path(self, pipelined=False):
        path = self.circ_path.parent / "out" / f"{self.name}.ref"
        if pipelined:
            path = path.with_name(f"{self.name}.piperef")
        return path

    def run(self, pipelined=False):
        self.fix_circ()
        passed, reason = self.check_hashes(pipelined=pipelined)
        if not passed:
            return passed, reason, None

        if pipelined and not self.can_pipeline():
            pipelined = False
        passed = False
        proc = None

        output_path = self.get_actual_table_path()
        output_path.parent.mkdir(parents=True, exist_ok=True)
        try:
            with output_path.open("w") as output_file:
                proc = subprocess.Popen(
                    [
                        "java",
                        "-jar",
                        str(logisim_path),
                        "-tty",
                        "table,binary,csv",
                        str(self.circ_path),
                    ],
                    stdout=output_file,
                    encoding="utf-8",
                    errors="ignore",
                )
                proc.wait()

            with redirect_stdout(StringIO()) as s:
                passed = diff_output(self.circ_path, pipelined)
                s.flush()
                s.seek(0)
                diff = s.read().strip("\n")

            if not passed:
                return False, "Did not match expected output", diff
            return True, "Matched expected output", None
        except KeyboardInterrupt:
            kill_proc(proc)
            sys.exit(1)
        except:
            traceback.print_exc()
            kill_proc(proc)
        return False, "Errored while running test", None


def fix_circ(circ_path):
    circ_path = circ_path.resolve()

    for glob, known_imports in known_imports_dict.items():
        if circ_path.match(glob):
            old_data = None
            data = None
            is_modified = False
            with circ_path.open("r", encoding="utf-8") as test_circ:
                old_data = test_circ.read()
                data = old_data
            for match in re.finditer(CIRC_IMPORT_REGEX, old_data):
                import_path_str = match.group(1)
                import_path = (circ_path.parent / Path(import_path_str)).resolve()
                for known_import in known_imports:
                    if import_path.match(known_import):
                        known_import_path = (proj_dir_path / known_import).resolve()
                        expected_import_path = Path(
                            os.path.relpath(known_import_path, circ_path.parent)
                        )
                        if import_path_str != expected_import_path.as_posix():
                            print(
                                f"Fixing bad import {import_path_str} in {circ_path.as_posix()} (should be {expected_import_path.as_posix()})"
                            )
                            data = data.replace(
                                import_path_str, expected_import_path.as_posix()
                            )
                            is_modified = True
                        break
                else:
                    expected_import_path = Path(
                        os.path.relpath(import_path, circ_path.parent)
                    )
                    if import_path_str != expected_import_path.as_posix():
                        print(
                            f"Fixing probably bad import {import_path_str} in {circ_path.as_posix()} (should be {expected_import_path.as_posix()})"
                        )
                        data = data.replace(
                            import_path_str, expected_import_path.as_posix()
                        )
                        is_modified = True
            if is_modified:
                with circ_path.open("w", encoding="utf-8") as test_circ:
                    test_circ.write(data)
            break


def run_tests(search_paths, pipelined=False):
    circ_paths = []
    for search_path in search_paths:
        if search_path.is_file() and search_path.suffix == ".circ":
            circ_paths.append(search_path)
            continue
        for circ_path in search_path.rglob("*.circ"):
            circ_paths.append(circ_path)
    circ_paths = sorted(circ_paths)

    has_banned_circuit = False
    for circ_path in proj_dir_path.rglob("cpu/*.circ"):
        fix_circ(circ_path)
        if find_banned(circ_path):
            has_banned_circuit = True
    for circ_path in proj_dir_path.rglob("harnesses/*.circ"):
        fix_circ(circ_path)
    if has_banned_circuit:
        return

    failed_tests = []
    passed_tests = []
    for circ_path in circ_paths:
        if "imm" in str(circ_path):
            update_imm_circ()

        test = TestCase(circ_path)
        did_pass, reason = False, "Unknown test error"
        try:
            did_pass, reason, extra = test.run(pipelined=pipelined)
        except KeyboardInterrupt:
            raise
        except SystemExit:
            raise
        except:
            traceback.print_exc()
        if did_pass:
            print(f"PASS: {test.id}", flush=True)
            passed_tests.append(test)
        else:
            print(f"FAIL: {test.id} ({reason})", flush=True)
            failed_tests.append(test)
        if extra:
            print(textwrap.indent(extra, "  "), flush=True)

    print(
        f"Passed {len(passed_tests)}/{len(failed_tests) + len(passed_tests)} tests",
        flush=True,
    )


def check_hash(path):
    rel_path = path.resolve().relative_to(proj_dir_path.resolve())
    rel_path_str = rel_path.as_posix()
    if rel_path_str not in starter_file_hashes:
        return (True, f"Starter does not have hash for {rel_path_str}")
    with path.open("rb") as f:
        contents = f.read()
    contents = contents.replace(b"\r\n", b"\n")
    hashed_val = hashlib.md5(contents).hexdigest()
    if hashed_val != starter_file_hashes[rel_path_str]:
        return (False, f"{rel_path_str} was changed from starter")
    return (True, f"{rel_path_str} matches starter file")


def kill_proc(proc):
    if proc.poll() is None:
        proc.terminate()
        for _ in range(10):
            if proc.poll() is not None:
                return
            time.sleep(0.1)
    if proc.poll() is None:
        proc.kill()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run Logisim tests")
    parser.add_argument(
        "test_path",
        help="Path to a test circuit, or a directory containing test circuits",
        type=Path,
        nargs="+",
    )
    parser.add_argument(
        "-p",
        "--pipelined",
        help="Check against reference output for 2-stage pipeline (when applicable)",
        action="store_true",
        default=False,
    )
    args = parser.parse_args()

    run_tests(args.test_path, args.pipelined)
