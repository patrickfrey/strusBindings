#!/bin/sh

PYTHONMALLOC=malloc valgrind --leak-check=full --leak-resolution=high --show-reachable=yes --undef-value-errors=yes --track-origins=yes --malloc-fill=23 --free-fill=69 --num-callers=12 --suppressions=../../../tests/python3/test.sup $@

