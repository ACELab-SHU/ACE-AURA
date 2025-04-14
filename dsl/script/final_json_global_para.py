import os
import json
import copy

def fill(final_json, all_data_json_path, output_json, combined_json_path):
    with open(output_json, 'r') as f:
        output_data = json.load(f)

    all_data = {}
    if os.path.isfile(all_data_json_path) and os.path.getsize(all_data_json_path) > 0:
        with open(all_data_json_path, 'r') as f:
            all_data = json.load(f)

    with open(combined_json_path, 'r') as f:
        combined_data = json.load(f)
    
    input_key = "all_input"

    data_offset_var = 0
    for task_name, task_data in combined_data.items():
        if task_name == "variable" :
            data_offset_var = task_data['data_offset']
            break

    for item in output_data:
        if input_key in item and item[input_key] is not None:
            for var in item[input_key]:
                if 'name' in var and var['name'] in all_data:
                    info = copy.deepcopy(all_data[var['name']])
                    info['offset'] = all_data[var['name']]['offset'] + data_offset_var
                    var.update(info)

    with open(final_json, 'w') as f:
        json.dump(output_data, f, indent=4)

def process_dag_files(map_folder_path, heft_base_path, final_base_path):
    for dag_folder in os.listdir(final_base_path):
        dag_base = dag_folder.split(".json")[0]
        dag_path = os.path.join(final_base_path, dag_base)
        if os.path.isdir(dag_path):
            
            final_dag_path = os.path.join(final_base_path, dag_base)
            os.makedirs(final_dag_path, exist_ok=True)
            
            final_json_path = os.path.join(final_dag_path, "final_all_input.json")
            var_json_path = os.path.join(map_folder_path, "all_data.json")

            heft_json_path = os.path.join(heft_base_path, f"{dag_base}.json")
            combined_json_path = os.path.join(final_dag_path, "combined_task_without_padding.json")
            fill(final_json_path, var_json_path, heft_json_path, combined_json_path)

map_folder_path = "./variable/map"
heft_base_path = "./heft_new/DAG"
final_base_path = "./IJ"
process_dag_files(map_folder_path, heft_base_path, final_base_path)
