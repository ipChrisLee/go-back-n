#/usr/bin/env bash

(ls utils | grep -q "anchor") || (echo "Are you in root of project?" && exit 1)

if [[ "$demoFile" ]]; then
	export filePathToSend="$demoFile"
else
	export filePathToSend="demo/file-to-send.txt"
fi
export filePathToRecv="demo/file-received.txt"

diff "$filePathToSend" "$filePathToRecv"
