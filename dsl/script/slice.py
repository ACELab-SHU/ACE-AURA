import json
import os

def replace_slice_task(tasks, slice_data, tasks_io):
    # Create a lookup dictionary for slice data
    slice_lookup = {key: value for key, value in slice_data.items()}

    for task in tasks:
        # Check if task has parentTasks and iterate through them
        if 'parentTasks' in task:
            new_parent_tasks = []
            for parent_task in task['parentTasks']:
                if 'outputVar' in parent_task and parent_task['taskId'] == 'slice':
                    output_var = parent_task['outputVar']
                    if output_var in slice_lookup:
                        # Find the corresponding entry in para_Input
                        for task2 in tasks:
                            for para in task2['para_Input']:
                                if para['name'] == slice_lookup[output_var]['slice_var']:
                                    # Preserve dest_address
                                    para_copy = para.copy()
                                    para_copy['dest_address'] = parent_task['dest_address']
                                    para_copy['slice_length'] = slice_lookup[output_var]['slice_length']
                                    para_copy['slice_data_type'] = slice_lookup[output_var]['data_type']
                                    # Add the copied entry to para_Input
                                    task['para_Input'].append(para_copy)
                                    break
                            break
                        
                        for task_name, io_data in tasks_io.items():
                            for output_key, output_value in io_data.get('output', {}).items():
                                if output_key == slice_lookup[output_var]['slice_var']:
                                    # Preserve dest_address and add slice details
                                    new_slice = {   
                                        'taskId':task_name,
                                        'outputVar':slice_lookup[output_var]['slice_var'],
                                        'outputIndex': output_value,
                                        'dest_address': parent_task['dest_address'],
                                        'concat_value': 0,
                                        'slice_length': slice_lookup[output_var]['slice_length'],
                                        'slice_data_type': slice_lookup[output_var]['data_type']
                                        }
                                    task['parentTasks'].append(new_slice)
                                    break
                            break
                    # Do not add this parent task to new_parent_tasks to effectively remove it
                else:
                    new_parent_tasks.append(parent_task)
            task['parentTasks'] = new_parent_tasks
            
        if 'childTasks' in task:
            new_child_tasks = []
            for child_task in task['childTasks']:
                if child_task['taskId'] == 'slice':
                    slice_map_var = child_task['inputVar']
                    for slice_var, slice_details in slice_lookup.items():
                        if slice_map_var in slice_lookup[slice_var]['slice_var']:
                            child_task['taskId'] = slice_lookup[slice_var]['taskid']
                            new_child_tasks.append(child_task)
                            break
                else:
                    new_child_tasks.append(child_task)
            task['childTasks'] = new_child_tasks

    return tasks

def process_ij_folder(ij_folder_path):
    for dag_folder in os.listdir(ij_folder_path):
        dag_name = dag_folder.split(".json")[0]
        dag_folder_path = os.path.join(ij_folder_path, dag_name)
        dag_input_tasks_spec = os.path.join(ij_folder_path, dag_name,"input_tasks_spec.json")
        dag_task_input_output = os.path.join(ij_folder_path, dag_name,"task_input_output.json")
        dag_slice = os.path.join('./variable/map/', f"{dag_name}_slice.json")
        dag_slice_updated_tasks = os.path.join(ij_folder_path, dag_name,"slice_updated_tasks.json")
        with open(dag_input_tasks_spec, 'r') as f:
            tasks_data = json.load(f)
    
        with open(dag_task_input_output, 'r') as f:
            tasks_io = json.load(f)

        with open(dag_slice, 'r') as f:
            slice_data = json.load(f)

        if os.path.isdir(dag_folder_path):
            updated_tasks = replace_slice_task(tasks_data, slice_data, tasks_io)

        with open(dag_slice_updated_tasks, 'w') as f:
            json.dump(updated_tasks, f, indent=4)

if __name__ == "__main__":
    ij_folder_path = './IJ'
    process_ij_folder(ij_folder_path)

