import os
import json

def process_task_json(task_json_path, venus_test_path):
    with open(task_json_path, 'r') as f:
        input_tasks_data = json.load(f)

    for task in input_tasks_data:
        taskid = task['taskId']
        actual_taskid_name = taskid.split('*')[0] if '*' in taskid else taskid
        if actual_taskid_name == "concat" or actual_taskid_name == "slice":
            continue
        filename = f"{actual_taskid_name}_resource.json"
        filepath = os.path.join(venus_test_path, filename)
        if os.path.exists(filepath):
            with open(filepath, 'r') as file:
                resource_data = json.load(file)
            has_bitalu = resource_data.get("BitALU", 0)
            has_complexunit = resource_data.get("CAU", 0)
            has_serdiv = resource_data.get("SerDiv", 0)
            bool_has_bitalu = '0'
            bool_has_complexunit = '0'
            bool_has_serdiv = '0'

            if has_bitalu != 0:
                task["has_bitalu"] = True
                bool_has_bitalu = '1'
            if has_complexunit != 0:
                task["has_complexunit"] = True
                bool_has_complexunit = '1'
            if has_serdiv != 0:
                task["has_serdiv"] = True
                bool_has_serdiv = '1'

            # task["hardwareinfo"] = "0b00000011" + bool_has_bitalu + bool_has_complexunit + bool_has_serdiv
            task["hardwareinfo"] = "0b00000110111"

    with open(task_json_path, 'w') as f:
        json.dump(input_tasks_data, f, indent=4)

def process_dag_folders(ij_directory, venus_test_directory):
    for dag_folder in os.listdir(ij_directory):
        dag_name = dag_folder.split(".json")[0]
        dag_path = os.path.join(ij_directory, dag_name)
        if os.path.isdir(dag_path):
            task_json_path = os.path.join(dag_path, "input_tasks_spec.json")
            venus_test_path = os.path.join(venus_test_directory)
            if os.path.exists(task_json_path) and os.path.exists(venus_test_path):
                process_task_json(task_json_path, venus_test_path)
            else:
                print(f"Skipping DAG folder {dag_folder} due to missing files.")

if __name__ == "__main__":
    ij_directory = "./IJ"
    venus_test_directory = "./venus_test"
    
    process_dag_folders(ij_directory, venus_test_directory)