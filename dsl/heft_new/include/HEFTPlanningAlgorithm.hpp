#ifndef HEFT_PLANNING_ALGORITHM_H
#define HEFT_PLANNING_ALGORITHM_H

#include <iostream>
#include <vector>
#include <map>
#include <unordered_set>
#include <queue>
#include <functional> 
#include <unordered_map>
struct Task {
    int taskId;
    double computationCost;
    std::vector<std::tuple<int, int, std::string, int, std::string, std::string, std::string >> parentTasks; // ID, Port Index, Dest_Addr, Concat_value, Slice_length, Slice_datatype, Varname;
    std::vector<std::tuple<int, int, std::string, int, std::string, std::string, std::string>> childTasks;  
    int spm_size;
    int num_lane;
    bool has_bitalu;
    bool has_serdiv;
    bool has_complexunit;
    std::string address;
    int length;
    std::vector<std::pair<std::string, std::string>> global_Input; 
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> para_Input;
    std::vector<std::pair<std::string, int>> return_output;
    int text_offset;
    int data_offset;
    int total_length;    
    int text_length;    
    int data_length; 
    int output_num; 
    std::string hardwareinfo;  
    std::string hash;
     
};

struct Tile {
    int tileId;
    double computationCapacity;
    int spm_size;
    int num_lane;
    bool has_bitalu;
    bool has_serdiv;
    bool has_complexunit;
};

struct inputTask {
    std::string taskId;
    double computationCost;
    std::vector<std::tuple<std::string, int, std::string, int, std::string, std::string, std::string>> parentTasks; // ID, Port Index, Dest_Addr, Concat_value, Slice_length, Slice_datatype, Varname;
    std::vector<std::tuple<std::string, int, std::string, int, std::string, std::string, std::string>> childTasks;  
    int spm_size;
    int num_lane;
    bool has_bitalu;
    bool has_serdiv;
    bool has_complexunit;
    std::string address;
    int length;
    std::vector<std::pair<std::string, std::string>> global_Input; 
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> para_Input;
    std::vector<std::pair<std::string, int>> return_output;
    int text_offset;
    int data_offset;
    int total_length;    
    int text_length;    
    int data_length; 
    int output_num;
    std::string hardwareinfo;
    std::string hash;
};

struct Event {
    int taskId;
    int tileId;
    double start;
    double finish;
};

class HEFTPlanningAlgorithm {
private:
    std::vector<Task> tasks;
    std::vector<Tile> tiles;
    std::vector<std::pair<int, double>> rankVector;
    std::unordered_set<int> currentlyCalculating;
    std::map<int, std::map<int, double>> computationCosts;
    std::map<int, std::map<int, double>> transferCosts;
    std::map<int, double> rank;
    std::map<int, double> earliestFinishTimes;
    std::map<int, std::vector<Event>> schedules;
    double epsilon = 1e-6;
    double averageBandwidth;

    double calculateAverageBandwidth();

    bool isChildTask(const Task& taskA, const Task& taskB) ;   

    static bool compareRank(const std::pair<int, double>& a, const std::pair<int, double>& b);

    void calculateComputationCosts(const std::vector<Task>& tasks, const std::vector<Tile>& tiles);

    void calculateTransferCosts(const std::vector<Task>& tasks);

    double calculateTransferCost(const Task& parent, const Task& child);

    void calculateRanks(const std::vector<Task>& tasks);

    double calculateRank(const Task& task);
    
    bool checkTaskTileMatch(const Task& task, const Tile& tile);

    void allocateTasks(const std::vector<Task>& tasks, const std::vector<Tile>& tiles);

    bool compareTasks(const Task& a, const Task& b);

    void allocateTask(const Task& task);

    double findFinishTime(const Task& task, const Tile& tile, double readyTime, bool occupySlot);  

public:
    HEFTPlanningAlgorithm(const std::vector<Task>& taskList, const std::vector<Tile>& tileList);

    void run();

    const std::vector<Task>& getTasks() const;

    const std::vector<Tile>& getTILEs() const;

    const std::vector<std::pair<int, double>>& getRanks() const ;

    const std::map<int, std::vector<Event>>& getSchedules() const;
};

#endif // HEFT_PLANNING_ALGORITHM_H
