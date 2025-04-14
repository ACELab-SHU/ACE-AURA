import os
import re
import json

def convert_type(data_type):
    # 定义类型转换规则
    if 'i16' in data_type:
        return 'short'
    elif 'i8' in data_type:
        return 'char'
    elif 'i32' in data_type:
        return 'int'
    else:
        return data_type  # 未知类型保持不变

def extract_function_arguments(file_path, function_name):
    # 正则表达式匹配函数定义
    function_pattern = re.compile(rf'\b(\w+)\s+{function_name}\s*\((.*?)\)\s*{{', re.S)
    
    arg_pattern = re.compile(r'\s*(\w+)\s+(\w+)(?:\s*,\s*)?')

    function_info = {'args': []}

    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
        content_no_comments = re.sub(r'//.*?$|/\*.*?\*/', '', content, flags=re.S | re.M)
        match = function_pattern.search(content_no_comments)
        if match:
            args = match.group(2)
            arg_types = arg_pattern.findall(args)
            for arg_type, arg_name in arg_types:
                converted_type = convert_type(arg_type)  # 转换类型
                function_info['args'].append({'name': arg_name, 'type': converted_type})

    return function_info

def analyze_directory(directory):
    result = {}

    for filename in os.listdir(directory):
        if filename.endswith('.c'):
            file_path = os.path.join(directory, filename)
            function_name = os.path.splitext(filename)[0]  # 去掉扩展名，得到函数名
            function_info = extract_function_arguments(file_path, function_name)
            print(f"function_name:{function_name}")
            print(f"function_info:{function_info}")
            if function_info['args']:  # 仅当函数有参数时才添加到结果中
                result[function_name] = function_info

    return result

def save_to_json(data, output_file):
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=4)

# 使用示例
directory_path = os.getcwd()  # 设置为当前工作目录
output_json_file = 'input_type.json'  # 输出的JSON文件名

function_info = analyze_directory(directory_path)

# 保存结果为JSON文件
save_to_json(function_info, output_json_file)

print(f"Function information has been saved to {output_json_file}.")
