#!/usr/bin/env bash
set -eu

###########################################################
# Change the next line as needed if auto-detection fails. #
# When in doubt, ask on Piazza.                           #
#   e.g. python_exec="py"                                 #
###########################################################
python_exec=""
if [ -z "${python_exec}" ]; then
  if command -v py 2>&1 >/dev/null; then
    python_exec=py
  elif command -v python3 2>&1 >/dev/null; then
    python_exec=python3
  elif command -v python 2>&1 >/dev/null; then
    python_exec=python
  fi
fi
if [[ "$("${python_exec}" -c 'import sys; print(sys.version_info[0])')" -ne "3" ]]; then
  echo "Error: couldn't detect Python version. Is `${python_exec}` Python 3?"
  "${python_exec}" -V
  exit 1
fi

help() {
  echo "--------------------HELP--------------------"
  echo "To test this project type: bash test.sh all"
  echo "To test part A, type: bash test.sh part_a"
  echo "To test part B, type: bash test.sh part_b"
  echo "To test coverage, type: bash test.sh coverage"
  echo "To test a function type: bash test.sh test_FUNCTION_NAME"
  echo "    Remember to replace FUNCTION_NAME"
  echo "--------------------------------------------"
}

if [ -z "${1:-}" ]; then
  help
  exit 1
fi

case "${1}" in
  all)
    "${python_exec}" -m unittest unittests.py -v
    ;;
  coverage)
    "${python_exec}" -m unittest studenttests.py -v
    ;;
  part_a)
    "${python_exec}" -m unittest unittests.TestAbs unittests.TestRelu unittests.TestArgmax unittests.TestDot unittests.TestMatmul -v
    ;;
  part_b)
    "${python_exec}" -m unittest unittests.TestReadMatrix unittests.TestWriteMatrix unittests.TestClassify unittests.TestChain -v
    ;;
  test_abs)
    "${python_exec}" -m unittest unittests.TestAbs -v
    ;;
  test_relu)
    "${python_exec}" -m unittest unittests.TestRelu -v
    ;;
  test_argmax)
    "${python_exec}" -m unittest unittests.TestArgmax -v
    ;;
  test_dot)
    "${python_exec}" -m unittest unittests.TestDot -v
    ;;
  test_matmul)
    "${python_exec}" -m unittest unittests.TestMatmul -v
    ;;
  test_read_matrix)
    "${python_exec}" -m unittest unittests.TestReadMatrix -v
    ;;
  test_write_matrix)
    "${python_exec}" -m unittest unittests.TestWriteMatrix -v
    ;;
  test_classify)
    "${python_exec}" -m unittest unittests.TestClassify -v
    ;;
  test_chain)
    "${python_exec}" -m unittest unittests.TestChain -v
    ;;
  *)
    help
    exit 1
    ;;
esac
