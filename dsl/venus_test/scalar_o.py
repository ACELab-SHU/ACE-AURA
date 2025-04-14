import os
import re
import json

def extract_info(file_path):
    with open(file_path, 'r') as file:
        content = file.read()

    file_name = os.path.basename(file_path)
    main_func_name = os.path.splitext(file_name)[0]
    vreturn_pattern = re.compile(r'vreturn\s*\((.*?)\);')
    vreturn_matches = vreturn_pattern.findall(content)
    vars = []

    for match in vreturn_matches:
        tokens = re.split(r'[,()\s]+', match.strip())
        i = 0
        while i < len(tokens):
            token = tokens[i].strip()
            # print(f"token:{token}")
            if token.startswith('&'):
                vars.extend(re.findall(r'&(\w+)', token))  # 将 'match' 改为 'token'
            elif not token.startswith('sizeof'):
                if i + 1 < len(tokens):
                    next_token = tokens[i + 1].strip()
                    # print(f"next_token:{next_token}")
                    if next_token.isdigit():
                        vars.append({token: int(next_token)})
            i += 1
                        
    return main_func_name, vars

def save_to_json(data, json_file_path):
    with open(json_file_path, 'w') as json_file:
        json.dump(data, json_file, indent=4)

folder_path = os.getcwd()
all_files_info = []

for filename in os.listdir(folder_path):
    if filename.endswith('.c'):
        file_path = os.path.join(folder_path, filename)
        main_func_name, vars = extract_info(file_path)
        file_info = {
            "function": main_func_name,
            "vreturn_o": vars
        }
        all_files_info.append(file_info)

json_file_path = 'scalar_o.json'
save_to_json(all_files_info, json_file_path)
