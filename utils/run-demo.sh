#/usr/bin/env bash

(ls utils | grep -q "anchor") || (echo "Are you in root of project?" && exit 1)

utils/build-debug.sh

export PATH="$PATH:build-debug"

mkdir -p demo

go_back_n sender > demo/sender.log 2>&1 &
go_back_n receiver > demo/receiver.log 2>&1 &
go_back_n pipe > demo/pipe.log 2>&1 &

echo "Demo is running background!"
