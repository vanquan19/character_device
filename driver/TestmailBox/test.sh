#!/bin/bash

echo "Chạy mô phỏng: A ghi 'ABC', B ghi 'XY', C đọc"

# A ghi 'ABC'
./test write ABC &
PID_A=$!
sleep 0.5

# B ghi 'XY'
./test write XY &
PID_B=$!
sleep 0.5

# C đọc 2 byte trước → nhận A, B
./test read 2 &
sleep 1

# C tiếp tục đọc 4 byte (block nếu chưa đủ)
./test read 4 &

# Sau 3 giây, ghi thêm 1 byte để unblock tiến trình đọc
sleep 3
echo ">> Ghi thêm 'Z' để giải block"
./test write Z

