#include "./include/HEFTPlanningAlgorithm.hpp"
#include "./include/JsonParser.hpp"
#include "./include/JsonWriter.hpp"
#include "./include/TaskConverter.hpp"
#include "./include/InputTile.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <random>
#include <utility>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    std::vector<inputTask> inputtasks = JsonParser::parseJson(inputFile);
    std::vector<Tile> tiles = InputTile::setupTiles();

    //Developer can change their own schedule algoithm
    
    auto result = TaskConverter::convertToTasks(inputtasks);

    std::vector<Task> tasks = result.first;
    std::unordered_map<std::string, int> idMapping = result.second;

    HEFTPlanningAlgorithm heftPlanner(tasks, tiles);
    heftPlanner.run();

    std::vector<std::pair<int, double>> rankkk = heftPlanner.getRanks();

    std::vector<std::pair<std::string, double>> mappedTaskData;
    std::string mappedTaskId;

    for (const auto &entry : rankkk)
    {
        for (const auto &pair : idMapping)
        {
            if (pair.second == entry.first)
            {
                mappedTaskId = pair.first;
                mappedTaskData.push_back({mappedTaskId, entry.second});
                break;
            }
        }
    }

    std::vector<inputTask> output = inputtasks;
    std::unordered_map<std::string, int> sequentialMapping;
    int sequentialCounter = 0;

    for (const auto &data : mappedTaskData) {
        sequentialMapping[data.first] = sequentialCounter++;
    }

    for (auto &task : output) {
        task.taskId = std::to_string(sequentialMapping[task.taskId]);

        for (auto &parentTaskId : task.parentTasks) {
            if (std::get<0>(parentTaskId) != "-1") {
                std::get<0>(parentTaskId) = std::to_string(sequentialMapping[std::get<0>(parentTaskId)]);
            }
        }
        for (auto &childTaskId : task.childTasks) {
            if (std::get<0>(childTaskId) != "-1") {
                std::get<0>(childTaskId) = std::to_string(sequentialMapping[std::get<0>(childTaskId)]);    
            }
        }
    }
  
    std::ofstream outputFileStream(outputFile, std::ios::out);

    if (!outputFileStream.is_open()) {
        std::cerr << "Error opening the output file: " << outputFile << std::endl;
        return 1;
    }

    json outputJson;
    json returnJson;
    json returnJson_info;
    
    for (int count=0; count < sequentialCounter ; count++)
    {   
        json taskJson;                       
        json parentTasksJson;
    
        for (const auto& task : output) {
            if (task.taskId == std::to_string(count)) {
                auto it = std::find_if(sequentialMapping.begin(), sequentialMapping.end(),
                    [count](const std::pair<const std::string, int>& pair) { return pair.second == count; });
                if (it != sequentialMapping.end()) {
                    taskJson["debug_task_name"] = it->first;
                } 
                taskJson["current_taskId"] = count;
                taskJson["text_offset"]    = task.text_offset;
                taskJson["data_offset"]    = task.data_offset;
                taskJson["total_length"]   = task.total_length;
                taskJson["text_length"]    = task.text_length;
                taskJson["data_length"]    = task.data_length;
                taskJson["hardwareinfo"]   = task.hardwareinfo; // last 5 bits :spm_size lane_num has_serdiv has_complexunit has_bitalu
                taskJson["hash"]           = task.hash;
                taskJson["Input_Num"]      = task.parentTasks.size() + task.global_Input.size() + task.para_Input.size();
                taskJson["Output_Num"]     = task.output_num;

                for (size_t i = 0; i < task.global_Input.size(); ++i) {
                    auto global_Id = task.global_Input[i].first; 
                    auto global_addr = task.global_Input[i].second; 
                    JsonWriter::writeBinaryToJson_data_global(parentTasksJson, global_Id, global_addr);
                }

                for (size_t i = 0; i < task.para_Input.size(); ++i) {
                    auto para_Id = std::get<0>(task.para_Input[i]); 
                    auto para_addr = std::get<1>(task.para_Input[i]); 
                    auto para_slice_length = std::get<2>(task.para_Input[i]); 
                    auto para_slice_data_type = std::get<3>(task.para_Input[i]); 
                    auto decimal_dest_address = std::stoi(para_addr, nullptr, 16);
                    int slice_data_addr = std::stoi(para_slice_length) * std::stoi(para_slice_data_type) ;
                    int slice_data_dest = decimal_dest_address + slice_data_addr;
                    std::stringstream ss;
                    ss << std::hex << std::uppercase << slice_data_dest;
                    std::string hex_slice_data_dest_str = ss.str();
                    JsonWriter::writeBinaryToJson_data_para(parentTasksJson, para_Id, para_addr, para_slice_length, para_slice_data_type,hex_slice_data_dest_str);
                }

                for (size_t i = 0; i < task.return_output.size(); ++i) {
                    auto return_Id = task.return_output[i].first; 
                    auto return_port = task.return_output[i].second; 
                    auto parentTaskId = task.taskId;
                    JsonWriter::writeBinaryToJson_data(returnJson_info,return_Id,parentTaskId,return_port);
                }

                for (size_t i = 0; i < task.parentTasks.size(); ++i) {
                    auto parentTaskId = std::get<0>(task.parentTasks[i]); // 获取父任务的 ID
                    int childTaskSize;
                    int port_num = std::get<1>(task.parentTasks[i]);
                    std::string dest_address = std::get<2>(task.parentTasks[i]);
                    int concat_value = std::get<3>(task.parentTasks[i]);
                    std::string slice_length = std::get<4>(task.parentTasks[i]);
                    std::string slice_data_type = std::get<5>(task.parentTasks[i]);
                    auto decimal_dest_address = std::stoi(dest_address, nullptr, 16);
                    int slice_data_addr = std::stoi(slice_length) * std::stoi(slice_data_type) ;
                    int slice_data_dest = decimal_dest_address + slice_data_addr;
                    auto varname = std::get<6>(task.parentTasks[i]); 
                    std::stringstream ss;
                    ss << std::hex << std::uppercase << slice_data_dest;
                    std::string hex_slice_data_dest_str = ss.str();
                    JsonWriter::writeBinaryToJson(parentTasksJson, parentTaskId, port_num, dest_address,concat_value,slice_length,slice_data_type,hex_slice_data_dest_str,varname);
                }

                if (returnJson_info.empty())
                    returnJson["return_output"] = "None";
                else
                    returnJson["return_output"] = returnJson_info;
                if (parentTasksJson.empty())
                    taskJson["all_input"] = "None";
                else
                    taskJson["all_input"] = parentTasksJson;
                break;
            
        }
        
        }
        
        outputJson.push_back(taskJson);
    }
    outputJson.push_back(returnJson);
    outputFileStream << std::setw(4) << outputJson;

    outputFileStream.close();

    return 0;
}
