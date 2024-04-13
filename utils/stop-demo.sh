#/usr/bin/env bash

(ls utils | grep -q "anchor") || (echo "Are you in root of project?" && exit 1)

pkill go_back_n
