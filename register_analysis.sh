#!/bin/bash

set -e # 启用错误退出

# 定义目标文件夹
folder_path="./dsl/venus_test/ir/analysis/"  

# 初始化变量
global_max=0  # 记录所有文件中最大的 vnsxxx 的数字
max_file=""   # 记录包含最大值的文件名

# 输出每个文件的最大值
# echo "每个文件的最大 vnsxxx:"
echo "每个task的寄存器个数:"

# 遍历文件夹中的 .s 文件
for file in "$folder_path"/*.s; do
    if [[ -f "$file" ]]; then
        # 提取当前文件中最大的 vnsxxx 数字
        max_value=$(grep -o 'vns[0-9]\+' "$file" | sed 's/vns//' | sort -n | tail -1)

        # 如果文件中没有匹配到，则设置为 0
        if [[ -z "$max_value" ]]; then
            max_value=0
        fi

        # 输出文件名和最大值
        echo "$(basename "$file"): $max_value"

        # 更新全局最大值和对应文件
        if (( max_value > global_max )); then
            global_max=$max_value
            max_file=$(basename "$file")
        fi
    fi
done

# 输出结果
if [[ $global_max -gt 0 ]]; then
    echo "所有文件中最大的寄存器是: vns$global_max"
    echo "所在文件: $max_file"
else
    echo "未在任何文件中找到 vnsxxx"
fi