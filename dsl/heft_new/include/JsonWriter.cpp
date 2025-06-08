#include "JsonWriter.hpp"
#include <bitset>
#include <iostream>

void JsonWriter::writeBinaryToJson(json &jsonData, const std::string& taskId , int port_num, std::string dest_addr, int concat_value, std::string slice_length, std::string slice_data_type, std::string hex_slice_data_dest_str, std::string varname)
{
    std::string taskid_binary = std::bitset<6>(std::stoi(taskId)).to_string();
    json taskDataJson;
    taskDataJson["type"] = "0b00";
    taskDataJson["parentTasks"] = taskId;
    taskDataJson["name"] = varname;
    taskDataJson["dest_address"] = dest_addr;                
    taskDataJson["parentTasksPort"] = "0b" + taskid_binary + std::bitset<4>(port_num).to_string();
    taskDataJson["concat_value"] = concat_value;
    taskDataJson["slice_length"] = slice_length;
    taskDataJson["slice_data_type"] = slice_data_type;
    taskDataJson["slice_data_dest_str"] = "0x" + hex_slice_data_dest_str;
    jsonData.push_back(taskDataJson);
}

void JsonWriter::writeBinaryToJson_data_global(json &jsonData, const std::string& Id, std::string addr)
{
    json taskDataJson;
    taskDataJson["name"] = Id;
    taskDataJson["dest_address"] = addr;
    taskDataJson["parentTasksPort"] = "0b0000000000";
    std::cout << "writeBinaryToJson_data_global parentTask_dest: " << addr << std::endl;    
    jsonData.push_back(taskDataJson);
}

void JsonWriter::writeBinaryToJson_data_para(json &jsonData, const std::string& Id, std::string addr, std::string slice_length, std::string slice_data_type,std::string hex_slice_data_dest_str)
{
    json taskDataJson;
    taskDataJson["name"] = Id;
    taskDataJson["dest_address"] = addr;
    taskDataJson["parentTasksPort"] = "0b0000000000";
    taskDataJson["slice_length"] = slice_length;
    taskDataJson["slice_data_type"] = slice_data_type;
    std::cout << "writeBinaryToJson_data__para parentTask_dest: " << addr << std::endl;    
    taskDataJson["slice_data_dest_str"] = "0x" + hex_slice_data_dest_str;
    jsonData.push_back(taskDataJson);
}

void JsonWriter::writeBinaryToJson_data(json &jsonData, const std::string& Id,const std::string& taskId, int port_num)
{
    json taskDataJson;
    taskDataJson["name"] = Id;
    std::string taskid_binary = std::bitset<6>(std::stoi(taskId)).to_string();
    taskDataJson["parentTasks"] = taskId;
    taskDataJson["parentTasksPort"] = "0b"+taskid_binary + std::bitset<4>(port_num).to_string();
    jsonData.push_back(taskDataJson);
}
