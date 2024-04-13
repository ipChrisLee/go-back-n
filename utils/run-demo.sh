#/usr/bin/env bash

(ls utils | grep -q "anchor") || (echo "Are you in root of project?" && exit 1)


utils/build-debug.sh

export PATH="$PATH:build-debug"

mkdir -p demo

utils/stop-demo.sh || true

if [[ "$demoFile" ]]; then
	export filePathToSend="$demoFile"
else
	export filePathToSend="demo/file-to-send.txt"
fi
export filePathToRecv="demo/file-received.txt"

go_back_n pipe s2r 0.1 0.1 > demo/pipe-s2r.log 2>&1 &
go_back_n pipe r2s 0.1 0.1 > demo/pipe-r2s.log 2>&1 &
go_back_n sender "${filePathToSend}" > demo/sender.log 2>&1 &
go_back_n receiver "${filePathToRecv}" > demo/receiver.log 2>&1 &

echo "Demo is running background! You can kill them by \`pkill go_back_n\` or \`utils/stop-demo.sh\`"
