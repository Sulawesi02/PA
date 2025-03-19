#!/bin/sh

# 设置环境变量
export nemu=build/nemu
export AM_HOME=/home/xu/share_projects/PA/ics2017/nexus-am

# 定义测试目录
test_dir="$AM_HOME/tests/cputest/tests"

# 遍历所有.c文件
for file in "$test_dir"/*.c; do
    # 提取文件名前缀（不带.c扩展名）
    prefix=$(basename "$file" .c)
    
    echo "========================================"
    echo "Running test: $prefix"
    echo "========================================"
    
    # 使用expect自动化交互
    expect << EOF
    spawn make ARCH=x86-nemu ALL=$prefix run
    expect -re {\(nemu\)}
    send "c\r"
    expect eof
EOF

    echo "\n\n"
done

echo "All tests completed!"