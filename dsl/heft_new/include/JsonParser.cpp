#include "JsonParser.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::vector<inputTask> JsonParser::parseJson(const std::string& filename) {
    std::ifstream file(filename);
    json jsonData;
    file >> jsonData;

    std::vector<inputTask> inputTasks;

    for (const auto& taskData : jsonData) {
        inputTask task;
        task.taskId          = taskData["taskId"];
        task.computationCost = taskData["computationCost"];
        task.spm_size        = taskData["spm_size"];
        task.num_lane        = taskData["num_lane"];
        task.has_bitalu      = taskData["has_bitalu"];
        task.has_serdiv      = taskData["has_serdiv"];
        task.has_complexunit = taskData["has_complexunit"];
        task.text_offset     = taskData["text_offset"];
        task.data_offset     = taskData["data_offset"];
        task.total_length    = taskData["total_length"];
        task.text_length     = taskData["text_length"];
        task.data_length     = taskData["data_length"];
        task.output_num      = taskData["output_num"];
        task.hardwareinfo    = taskData["hardwareinfo"];
        task.hash            = taskData["hash"];

        // parent
        for (const auto& parentTask : taskData["parentTasks"]) {
            std::string parentId = parentTask["taskId"];
            int outputPort = parentTask["outputIndex"];
            std::cout << "parentTask_dest: " << parentTask["dest_address"] << std::endl;
            int concat_value = parentTask["concat_value"];
            std::string parentTask_slice_length   ;
            std::string parentTask_slice_data_type;
            std::string varname = parentTask["outputVar"];

            if (parentTask["dest_address"] != "null"){   
                std::cout << "if parentTask_dest: " << parentTask["dest_address"] << std::endl;
                std::string dest_addr = parentTask["dest_address"];
                if (parentTask.find("slice_length") != parentTask.end()) {
                    parentTask_slice_length = parentTask["slice_length"];
                    parentTask_slice_data_type = parentTask["slice_data_type"];
                }
                else {
                    parentTask_slice_length = "0";
                    parentTask_slice_data_type = "0";
                }
                task.parentTasks.push_back({parentId, outputPort, dest_addr, concat_value, parentTask_slice_length, parentTask_slice_data_type, varname});
            }
        }

        // child
        for (const auto& childTask : taskData["childTasks"]) {
            std::string childId = childTask["taskId"];
            int inputPort = childTask["inputIndex"];
            std::string dest_addr = "null";
            int concat_value = childTask["concat_value"];
            std::string childTask_slice_length ;
            std::string childTask_slice_data_type;
            std::string varname = childTask["inputVar"];
            
            if (childTask.find("slice_length") != childTask.end()) {
                childTask_slice_length    = childTask["slice_length"];
                childTask_slice_data_type = childTask["slice_data_type"];
            }
            else {
                childTask_slice_length    = "0";
                childTask_slice_data_type = "0";
            }
            task.childTasks.push_back({childId, inputPort, dest_addr, concat_value, childTask_slice_length, childTask_slice_data_type, varname});
        }

        //data
        for (const auto& global_Input : taskData["global_Input"]) {
            std::string globalId = global_Input["name"];
            std::cout << "global_Input: " << global_Input["dest_address"] << std::endl;
            if (global_Input["dest_address"] != "null")
            {   std::cout << "in global_Input: " << global_Input["dest_address"] << std::endl;
                std::string global_addr = global_Input["dest_address"];
                task.global_Input.push_back({globalId, global_addr});
            }
        }
        for (const auto& para_Input : taskData["para_Input"]) {
            std::string paraId = para_Input["name"];
            if( para_Input["dest_address"] != "null" ){
                std::string paraId_addr = para_Input["dest_address"];
                std::string para_slice_length;
                std::string para_slice_data_type;
                if (para_Input.find("slice_length") != para_Input.end()) {
                    para_slice_length = para_Input["slice_length"];
                    para_slice_data_type = para_Input["slice_data_type"];
                }
                else {
                    para_slice_length = "0";
                    para_slice_data_type = "0";
                }
                task.para_Input.push_back({paraId, paraId_addr, para_slice_length, para_slice_data_type});
            }
        }
        for (const auto& return_output : taskData["return_output"]) {
            std::string returnId = return_output["name"];
            int return_port = return_output["index"];
            task.return_output.push_back({returnId, return_port});
        }


        inputTasks.push_back(task);
    }

    return inputTasks;
}
