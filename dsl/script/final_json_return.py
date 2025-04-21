import json
import os
import shutil

def fill(final_json, map_json_path, output_json):
    # Check if map_json_path file is empty
    if os.path.getsize(map_json_path) == 0:
        shutil.copyfile(output_json, final_json)
        print(f"!!Warning: {map_json_path} is empty. Skipping processing.")
        print(f"========================== WORK COMPLETED ==========================")
        print(f"\n")
        return
    
    with open(output_json, 'r') as f:
        output_data = json.load(f)
    
    with open(map_json_path, 'r') as f:
        data = json.load(f)

    input_key = "return_output"
    
    for item in output_data:
        if input_key in item and item[input_key] is not None:
            for var in item[input_key]:
                if 'name' in var and var['name'] in data:
                    info = data[var['name']]
                    var.update(info)

    check_slice_length = "all_input"   

    for item2 in output_data:
        if check_slice_length in item2 and item2[check_slice_length] is not None:
            for var2 in item2[check_slice_length]:
                if 'slice_length' in var2 and 'length' in var2:
                    slice_length = int(var2['slice_length'])
                    length = var2['length']
                    if slice_length > length:
                        raise ValueError("slice_length cannot be greater than length")
                
    with open(final_json, 'w') as f:
        json.dump(output_data, f, indent=4)

    print(f"========================== WORK COMPLETED ==========================")
    print(f"\n")
def process_dag_files(map_folder_path, output_base_path, final_base_path):
    for dag_folder in os.listdir(output_base_path):
        dag_base= dag_folder.split(".json")[0]
        dag_path = os.path.join(output_base_path, dag_base)

        if os.path.isdir(dag_path):
            final_json_path = os.path.join(final_base_path, f"{dag_base}.json")
            output_json_path = os.path.join(output_base_path, dag_base, "final_all_input.json")
            fill(final_json_path, map_folder_path, output_json_path)

output_base_path = "./IJ"
map_folder_path = "./variable/map/return_value.json"
final_base_path = "./final_output"
process_dag_files(map_folder_path, output_base_path, final_base_path)