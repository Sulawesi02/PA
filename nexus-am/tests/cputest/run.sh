#!/bin/sh

# 设置环境变量
export nemu=build/nemu
export AM_HOME=/home/xu/share_projects/PA/ics2017/nexus-am

# 使用 expect 自动输入 c
expect << EOF
spawn make ARCH=x86-nemu ALL=add run
# 更灵活地匹配提示信息
expect -re {\(nemu\)}
send "c\r"
expect eof
EOF
    