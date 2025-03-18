#!/bin/sh

# 设置环境变量
export nemu=build/nemu
export AM_HOME=/home/xu/share_projects/PA/ics2017/nexus-am

# 执行 make 命令并通过 expect 自动输入 c
expect << EOF
spawn make ARCH=x86-nemu ALL=add run
expect "Enter command:"
send "c\r"
interact
EOF