import json
from collections import defaultdict

# 读取JSON文件
def read_json_file(file_path):
    with open(file_path, 'r') as file:
        return json.load(file)

# 统计每个 debug_task_name 的 data_length 和 text_length
def calculate_lengths(json_data):
    stats = defaultdict(lambda: {"data_length": 0, "text_length": 0})

    for entry in json_data:
        if "debug_task_name" not in entry:
            continue
        task_name = entry["debug_task_name"]
        stats[task_name]["data_length"] += entry["data_length"]
        stats[task_name]["text_length"] += entry["text_length"]

    return stats

# 主函数
def main():
    file_path = "./dsl/final_output/dag1.json"  # 替换为实际的JSON文件路径
    json_data = read_json_file(file_path)
    stats = calculate_lengths(json_data)

    for task_name, lengths in stats.items():
        print(f"Task Name: {task_name}")
        print(f"  Total Data Length: {lengths['data_length']}")
        print(f"  Total Text Length: {lengths['text_length']}\n")

if __name__ == "__main__":
    main()