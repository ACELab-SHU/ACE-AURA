import json
import os

def process_dag_folder(dag_folder_path):
    input_tasks_spec_path = os.path.join(dag_folder_path, 'input_tasks_spec.json')
    task_input_dest_addr_path = os.path.join(dag_folder_path, 'task_input_dest_addr.json')

    with open(input_tasks_spec_path, 'r') as f:
        task_data = json.load(f)

    with open(task_input_dest_addr_path, 'r') as f:
        dest_data = json.load(f)

    for task in task_data:
        # Match parentTasks
        previous_dest_address = None
        previous_length = 0
        previous_concat_value = 0

        for parent_task in task.get('parentTasks', []):
            concat_dest_address = parent_task['dest_address']   
            concat_value = parent_task['concat_value']   
            concat_length = parent_task['concat_length']   

            if concat_dest_address == "null":
                continue
            dest_address = "null"  # default dest_address
            dest_task_id = task['taskId']
            
            for dest_param, dest_param_value in dest_data[dest_task_id]['input'].items():
                if dest_param == concat_dest_address: 
                   dest_address = dest_param_value.get('dest_address', "null")  # Assign the value directly
                   break
            # Assign dest_address value to parent_task['dest_address']
            parent_task['dest_address'] = dest_address

            current_dest_address = dest_address

            if concat_value == previous_concat_value and previous_concat_value != 0:
                if previous_dest_address:
                    old_address = int(previous_dest_address, 16)
                    new_address = old_address + previous_length
                    parent_task['dest_address'] = hex(new_address)

                previous_dest_address = parent_task['dest_address']
                previous_length = concat_length
            else:
                # # Check if the new dest_address is less than the previous dest_address
                # if previous_dest_address is not None and current_dest_address != "null":
                #     previous_dest_address_int = int(previous_dest_address, 16)
                #     current_dest_address_int = int(current_dest_address, 16)
                    
                #     if current_dest_address_int < previous_dest_address_int:
                #         raise ValueError(f"Error: Current dest_address ({current_dest_address}) is less than previous dest_address ({previous_dest_address}).")
                
                previous_dest_address = current_dest_address
                previous_length = concat_length

            previous_concat_value = concat_value


        for parent_task in task.get('parentTasks', []):
            output_var = parent_task['outputVar']
            not_concat_var_dest_addr = parent_task['dest_address']
            if not_concat_var_dest_addr != "null":
                continue
            dest_task_id = task['taskId']
            for dest_param, dest_param_value in dest_data[dest_task_id]['input'].items():
                if dest_param == output_var:
                    dest_address = dest_param_value.get('dest_address', "null")  # Assign the value directly
                    break
            # Assign dest_address value to parent_task['dest_address']
            parent_task['dest_address'] = dest_address

        for parent_task in task.get('global_Input', []):
            var = parent_task['name']
            dest_address = "null"  # default dest_address
            dest_task_id = task['taskId']
            for dest_param, dest_param_value in dest_data[dest_task_id]['input'].items():
                if dest_param == var:
                    dest_address = dest_param_value.get('dest_address', "null")  # Assign the value directly
                    break
            # Assign dest_address value to parent_task['dest_address']
            parent_task['dest_address'] = dest_address

        for parent_task in task.get('para_Input', []):
            var = parent_task['name']
            dest_address = "null"  # default dest_address
            dest_task_id = task['taskId']
            for dest_param, dest_param_value in dest_data[dest_task_id]['input'].items():
                if dest_param == var:
                    dest_address = dest_param_value.get('dest_address', "null")  # Assign the value directly
                    break
            # Assign dest_address value to parent_task['dest_address']
            parent_task['dest_address'] = dest_address

    with open(input_tasks_spec_path, 'w') as f:
        json.dump(task_data, f, indent=4)

def process_ij_folder(ij_folder_path):
    for dag_folder in os.listdir(ij_folder_path):
        dag_name = dag_folder.split(".json")[0]
        dag_folder_path = os.path.join(ij_folder_path, dag_name)
        if os.path.isdir(dag_folder_path):
            process_dag_folder(dag_folder_path)

if __name__ == "__main__":
    ij_folder_path = './IJ'
    process_ij_folder(ij_folder_path)