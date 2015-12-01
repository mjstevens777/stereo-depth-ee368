#!/bin/bash
python run-test-matrix.py > tests.cmd
cat tests.cmd|tr "\n" "\0"|xargs -P $(($(nproc) - 2)) -0 -n 1 sh -c
