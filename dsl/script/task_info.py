import json
import os

def fill_index(final_json, source, output_json):
    with open(source, 'r') as f:
        data = json.load(f)
    
    # 读取output.json文件
    with open(output_json, 'r') as f:
        output_data = json.load(f)
    
    for item in output_data:
        if 'taskId' in item and item['taskId'] is not None :
            taskId = item['taskId']
            actual_task_name = taskId.split('*')[0] if '*' in taskId else taskId
            if actual_task_name == "concat" or actual_task_name == "slice":
                continue
            if taskId in data:
                text_offset = data[taskId]['text_offset']
                data_offset = data[taskId]['data_offset']
                total_length = data[taskId]['total_length']
                text_length = data[taskId]['text_length']
                data_length = data[taskId]['data_length']
                hash = data[taskId]['hash']
                item['text_offset'] = text_offset
                item['data_offset'] = data_offset
                item['total_length'] = total_length
                item['text_length'] = text_length
                item['data_length'] = data_length
                item['hash'] = hash

    with open(final_json, 'w') as f:
        json.dump(output_data, f, indent=4)

def process_dag_folders(base_path):
    for dag_folder in os.listdir(base_path):
        dag_base= dag_folder.split(".json")[0]
        dag_path = os.path.join(base_path, dag_base)
        if os.path.isdir(dag_path):
            final_json_path = os.path.join(dag_path, "input_tasks_spec.json")
            source = os.path.join(dag_path, "combined_task_without_padding.json")
            output_json_path = os.path.join(dag_path, "input_tasks.json")
            fill_index(final_json_path, source, output_json_path)

if __name__ == "__main__":
    base_path = "./IJ"
    process_dag_folders(base_path)