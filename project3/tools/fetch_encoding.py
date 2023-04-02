from pathlib import Path
import xml.etree.ElementTree as ET
import csv
import json

IMM_SEL_ROM_LOC = "(610,350)"
proj_dir_path = Path(__file__).parent.parent


def get_encoding_path():
    return proj_dir_path / "tests" / "unit-imm-gen" / "imm-gen-encoding.csv"


def get_imm_test_path(file_name):
    return proj_dir_path / "tests" / "unit-imm-gen" / file_name


def get_imm_ref_path(file_name):
    return proj_dir_path / "tests" / "unit-imm-gen" / "out" / file_name


def decimal_to_binary(n):
    # return bin(int(n)).replace("0b", "")
    return format(int(n), "03b")


def update_imm_circ():
    student_encoding = {}
    imm_sel_tests = {}

    with get_encoding_path().open(
        "r", encoding="utf-8", errors="ignore"
    ) as encoding_file:
        student_encoding = {
            row["Format"]: row["Code"] for row in csv.DictReader(encoding_file)
        }

    with get_imm_test_path("imm-sel-tests.json").open(
        "r", encoding="utf-8", errors="ignore"
    ) as imm_sel_file:
        imm_sel_tests = json.load(imm_sel_file)

    for test_file in imm_sel_tests:
        encoded_test = [student_encoding[fmt] for fmt in imm_sel_tests[test_file]]

        # update circuit
        tree = ET.parse(get_imm_test_path(f"{test_file}.circ"))
        root = tree.getroot()
        imm_sel_contents = root.find(
            f".//comp[@loc='{IMM_SEL_ROM_LOC}']/a[@name='contents']"
        )
        imm_sel_contents.text = "addr/data: 8 3\n" + " ".join(encoded_test) + "\n"

        tree.write(get_imm_test_path(f"{test_file}.circ"))

        # update reference file
        new_records = []
        with get_imm_ref_path(f"{test_file}.ref").open(
            "r", encoding="utf-8", errors="ignore"
        ) as imm_ref_file:
            encoded_binary_test = [
                decimal_to_binary(fmt_num) for fmt_num in encoded_test
            ]
            imm_ref_reader = csv.DictReader(imm_ref_file)
            i = 0
            for record in imm_ref_reader:
                record["ImmSel"] = encoded_binary_test[i]
                new_records.append([tpl[1] for tpl in record.items()])
                i += 1

        with get_imm_ref_path(f"{test_file}.ref").open(
            "w", encoding="utf-8", errors="ignore"
        ) as imm_ref_file:
            fields = ["Test", "Immediate", "Instruction", "ImmSel"]
            imm_ref_writer = csv.DictWriter(imm_ref_file, fieldnames=fields, lineterminator="\n")
            imm_ref_file.write(",".join(fields) + "\n")
            for record in new_records:
                imm_ref_file.write(",".join(record) + "\n")
