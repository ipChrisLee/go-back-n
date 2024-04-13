# go-back-n

## What is it

A simple (maybe not) implementation of [go-back-n](https://en.wikipedia.org/wiki/Go-Back-N_ARQ). This impementation consider packet loss and packet corruption, and verify the design by simulating the loss and corruption when transfering packets.

## Usage

To run a demo, just:

```bash
# cd the root of project.
utils/run-demo.sh
# wait a minute for transfer finished.
utils/check-demo.sh
```

In the demo, sender will read file `demo/file-to-send.txt` and send to a pipe program, receiver will receive packets and writes received contents to file `demo/file-received.txt`. Logs are writed to `demo/*.log`.

In fact, you can make sender read any file or make receiver write to any file. See `src/sender.c` and `src/receiver.c` for more details.

Remeber to run `go_back_n pipe` to make sender and receiver can connect to each other.

To check the robustness, you can `export demoFile="demo/ANNA_KARENINA.txt"` before running demo, so the file to be transferred will be [Anna_Karnenina](https://en.wikipedia.org/wiki/Anna_Karenina), a 1940KB novel.

## Intro

This is just a simple introduction.

The sender (we call it S) and the receiver (we call it R) will connect each other by two "pipes". The `go_back_n pipe s2r` is for packets from S to R, and the `go_back_n pipe r2s` is for packets from R to S (always for ACK in go-back-n).

The packet loss and packet corruption are achieved by the `pipe` program. The reason why we don't let S and R send to each other directly is that by such method, we can simulate packet loss and corruption better.

The S will split the file into picies, and send them one by one. The R will receive files, and write to the file.

More detail please my codes.