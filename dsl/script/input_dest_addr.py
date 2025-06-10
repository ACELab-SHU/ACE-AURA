import os
import re
import json
import sys

def update_json_with_dest_address(dag_path, task_json_path, venus_test_path, task_out_json_path, lane_num, VenusInputStructAddr):
    with open(task_json_path, 'r') as file:
        data = json.load(file)

    function_names = list(data.keys())
    VENUS_VRFADDR = 0x00100000
    print(f"VenusInputStructAddr:{VenusInputStructAddr}")
    for function_name in function_names:
        actual_function_name = function_name.split('*')[0] if '*' in function_name else function_name
        asm_file_path = os.path.join(venus_test_path, 'ir', 'analysis', actual_function_name + '_analysis.s')

        if not os.path.exists(asm_file_path):
            print(f"Assembly file not found for function {function_name}. Skipping...")
            continue

        # Initialize variables for parsing assembly file
        in_function = False
        in_vns_delimit = False
        index = 0
        scalar_i = 0
        scalar_index = 0
        vns_delimit_count = 0
        final_vector_index = 0
        num_params = len(data[function_name]['input'])
        last_bound_variable = None
        all_index = 0
        
        for variable_info in data[function_name]['input'].values():
            variable_info['updated'] = False

        # Open and parse assembly file
        with open(asm_file_path, 'r') as asm_file:
            for line in asm_file:
                if re.match(r'\s*{}\s*:'.format(actual_function_name), line):
                    in_function = True
                    continue
                if in_function and 'vns_delimit' in line:
                    in_vns_delimit = True

                    vns_delimit_count += 1
                    if vns_delimit_count % 2 == 0:
                        index += 1
                        in_vns_delimit = False
                    if vns_delimit_count == num_params * 2:
                        in_function = False
                        break
                    continue

                if in_vns_delimit and 'vns_bind' in line:
                    match = re.search(r'vns(\d+)', line)
                    if match:
                        vns_bind_registers = int(match.group(1))
                        dest_address = hex(vns_bind_registers * lane_num * 8 + VENUS_VRFADDR)
                        print(f"vns{vns_bind_registers}---{dest_address}:dest_address")
                        print(f"line:{line}")

                    vector_index = index
                    for variable_name, variable_info in data[function_name]['input'].items():
                        if variable_info['updated']:
                            continue
                        if variable_info['num'] == 1 or variable_info['num'] == '1':
                            scalar_i += 1
                            continue
                        vector_index = vector_index + scalar_i
                        if variable_info['num'] != 1 and variable_info['num'] != '1' and variable_info['index'] == vector_index:
                            variable_info['dest_address'] = dest_address
                            variable_info['updated'] = True
                            print(f"vector_index:{vector_index}")
                            print(f"variable_info:{variable_info}")
                            print(f"variable_name:{variable_name}")
                            
                            scalar_i = 0
                            all_index = all_index + 1
                            break
                        
                    in_vns_delimit = False
                    continue

        for variable_name, variable_info in data[function_name]['input'].items():
            if variable_info['updated'] == False and (variable_info['num'] == 1 or variable_info['num'] == '1'):
                with open(os.path.join(dag_path, "all_data.json"), 'r') as all_data_file:
                    all_data = json.load(all_data_file)
                print(f"variable_name:{variable_name}")
                print(f"variable_info:{variable_info}")
                if scalar_index == 0:
                    dest_address = hex(VenusInputStructAddr)
                else:
                    previous_address = int(dest_address, 16)
                    aligned_address = ((previous_address + 63 + variable_length) // 64) * 64
                    dest_address = hex(aligned_address)
                if variable_name not in all_data or 'length' not in all_data[variable_name]:
                    variable_length = 64
                else:
                    variable_length = all_data[variable_name]['length']
                variable_info['dest_address'] = dest_address
                variable_info['updated'] = True
                scalar_index = scalar_index + 1
                all_index = all_index + 1
                  
        with open("../venus_test/task_input_num.json", "r") as file:
            dsl_data = json.load(file)

        expected_input_num = dsl_data[actual_function_name]
        final_vector_index = all_index
        if(expected_input_num != final_vector_index):
            raise ValueError(f"Input number for task '{actual_function_name}' is {final_vector_index}, expected {expected_input_num}")
        all_index = 0

        for variable_info in data[function_name]['input'].values():
            if variable_info['updated'] == False:
                print(f"Attention:optimizing:{variable_info}")
            if 'updated' in variable_info:
                del variable_info['updated']

    with open(task_out_json_path, 'w') as outfile:
        json.dump(data, outfile, indent=4)

    print(f"JSON file {task_json_path} has been updated.")

def process_dag_folders(task_base_path, venus_test_base_path, lane_num, VenusInputStructAddr):
    for dag_folder in os.listdir(task_base_path):
        dag_base = dag_folder.split(".json")[0]
        dag_path = os.path.join(task_base_path, dag_base)
        if os.path.isdir(dag_path):
            task_json_path = os.path.join(dag_path, "task_input_output.json")
            venus_test_path = os.path.join(venus_test_base_path)
            task_out_json_path = os.path.join(dag_path, "task_input_dest_addr.json")
            update_json_with_dest_address(dag_path, task_json_path, venus_test_path, task_out_json_path, lane_num, VenusInputStructAddr)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python input_dest_addr.py <lane_num> <VenusInputStructAddr>")
        sys.exit(1)
    lane_num = int(sys.argv[1])
    VenusInputStructAddr = int(int(sys.argv[2], 16) - 0x80000000)  

    task_base_path = '../IJ'
    venus_test_base_path = '../venus_test'

    process_dag_folders(task_base_path, venus_test_base_path, lane_num, VenusInputStructAddr)
