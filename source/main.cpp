#include "RISCV.h"
#include "venus_ext.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <unistd.h>

uint32_t iteration = 1000000000;
Venus_Emulator emulator;

int main(int argc, char **argv) {
  emulator.dumpfile.open("emulator_venus_ins.txt");
  emulator.verbose = false;
  emulator.venus_verbose = false;
  emulator.dsl_mode = false;
  emulator.compare_result = false;
  // emulator.PROGADDR_RESET = 0x00005000;
  emulator.PROGADDR_RESET = 0x00000000; // for check.hex/rot13.hex
  emulator.PROGADDR_IRQ = 0x00000010;
  emulator.PRINTF_ADDR = 0x10000000;
  emulator.TEST_PRINTF_ADDR = 0x20000000;

  char *json_file_name;
  char *combined_bin_name;
  char *compare_file_name;

  int opt;
  while ((opt = getopt(argc, argv, "b:c:j:vw")) != -1) {
    switch (opt) {
    case 'c':
      emulator.compare_result = true;
      compare_file_name = optarg;
      break;
    case 'b':
      combined_bin_name = optarg;
      break;
    case 'j':
      emulator.dsl_mode = true;
      json_file_name = optarg;
      break;
    case 'v':
      emulator.verbose = true;
      break;
    case 'w':
      emulator.venus_verbose = true;
      break;
    default:
      fprintf(stderr,
              "Usage: %s [-vw] [-j jsonfile -b combined-binfile [-c "
              "vcslogfile]] hex_file\n",
              argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (emulator.dsl_mode) {
    using json = nlohmann::json;
    // Open the file
    ifstream file(json_file_name);
    printf("JSON file name: %s\n", json_file_name);
    if (file.is_open())
      printf("json file opened successfully\n");
    else {
      printf("json file open failed!\n");
      exit(EXIT_FAILURE);
    }
    json jsonData;
    file >> jsonData;
    file.close();

    ifstream binfile(combined_bin_name);
    if (binfile.is_open())
      printf("combined bin file opened successfully\n");
    else {
      printf("combined bin file open failed!\n");
      exit(EXIT_FAILURE);
    }

    ofstream debugfile("./emu_dsl.log");
    if (debugfile.is_open())
      printf("debug file opened successfully\n");
    else {
      printf("debug file open failed!\n");
      exit(EXIT_FAILURE);
    }

    ifstream cmpfile(compare_file_name);
    if (emulator.compare_result) {
      if (cmpfile.is_open())
        printf("compare file opened successfully\n");
      else {
        printf("compare file open failed!\n");
        exit(EXIT_FAILURE);
      }
    }
    cout << endl;

    for (const auto &taskData : jsonData) {
      // Analyze the DAG return
      if (taskData.contains("return_output")) {
        if (taskData["return_output"].is_string()) {
          debugfile << "DAG has no output value" << endl;
        } else {
          for (const auto &dagreturn : taskData["return_output"]) {
            string parenttaskport_str = dagreturn["parentTasksPort"];
            parenttaskport_str.erase(0, 2); // erase "0b"
            unsigned taskid =
                (bitset<32>(parenttaskport_str).to_ulong() >> 4) & 0b111111;
            unsigned outputid = bitset<32>(parenttaskport_str).to_ulong() & 0xF;
            cout << "The DAG return " << dagreturn["name"] << " is the "
                 << outputid << "-th output of task " << dagreturn["taskId"]
                 << " (TaskID: " << taskid
                 << "). Return Length: " << dagreturn["length"] << endl;

            std::string dagretfilename = dagreturn["name"];
            dagretfilename = "DAGRet_" + dagretfilename + ".log";
            ofstream dagretfile(dagretfilename);
            if (dagretfile.is_open())
              printf("DAG return %s file opened successfully\n",
                     dagretfilename.c_str());
            else {
              printf("DAG return %s file opened failed\n",
                     dagretfilename.c_str());
              exit(EXIT_FAILURE);
            }

            std::string dagretbyte_str = dagreturn["length"];
            int dagretbyte = stoi(dagretbyte_str);
            if (emulator.taskret[outputid][taskid].length != dagretbyte) {
              cout << "Warning: byte value in taskret structure is not the "
                      "same as the json specification. Emulator value: "
                   << emulator.taskret[outputid][taskid].length
                   << ". JSON value: " << dagretbyte << "." << endl;
            }
            char *emu_ptr = (char *)emulator.taskret[outputid][taskid].emu_ptr;
            unsigned ptr = emulator.taskret[outputid][taskid].ptr;
            for (int byte = 0; byte < dagretbyte; byte += 4) {
              int data = *(int *)(emu_ptr + byte);
              dagretfile << hex << setw(8) << setfill('0') << data << endl;
            }
            dagretfile.close();
          }
        }
        goto dsl_done;
      }

      // "None" is a string
      if (taskData.contains("all_input") &&
          !taskData["all_input"].is_string()) {

        for (const auto &input : taskData["all_input"]) {
          string type_str = input["type"];
          type_str.erase(0, 2); // erase "0b"

          // {"type", "0b01"} is a non-temp type
          if (bitset<32>(type_str).to_ulong() == 1) {
            unsigned offset_byte = input["offset"];
            unsigned offset = offset_byte;
            // unsigned offset = input["offset"];
            cout << "The input " << input["name"] << " of task "
                 << taskData["debug_task_name"] << " is a parameter/global."
                 << endl;
            cout << "offset:" << offset << endl;
            debugfile << "The input " << input["name"] << " of task "
                      << taskData["debug_task_name"]
                      << " is a parameter/global."
                      << "Length is \"" << dec << input["length"] << "\""
                      << endl;
            debugfile << "offset:" << hex << setfill('0') << setw(8)
                      << offset + 0x80000000 << endl;
            // Write data to VSPM
            for (unsigned byte = 0; byte < input["length"]; byte = byte + 4) {
              unsigned char data[4];
              // char data[4];
              binfile.seekg(offset + byte, ios::beg);
              binfile.read(reinterpret_cast<char *>(data), 4);

              unsigned wrdata =
                  data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
              // cout << wrdata << endl;

              string dest_addr_str = input["dest_address"];
              unsigned dest_addr = stoi(dest_addr_str, nullptr, 16) + byte;
              if (dest_addr >= 0x100000) {
                unsigned wraddr = dest_addr - emulator.BLOCK_VRF_OFFSET;
                printf("Write data %x to address %x\n", wrdata,
                       dest_addr + emulator.BLOCK_PERSPECTIVE_BASEADDR);
                debugfile << "Write data " << hex << setfill('0') << setw(8)
                          << wrdata << ", dest_addr " << setfill('0') << setw(8)
                          << hex
                          << dest_addr + emulator.BLOCK_PERSPECTIVE_BASEADDR +
                                 0x02000000
                          << endl;
                emulator.st_to_vspm(wraddr >> 2, wrdata);
              } else {
                unsigned wraddr = dest_addr;
                printf("Write scalar data %x to address %x\n", wrdata,
                       dest_addr + emulator.BLOCK_PERSPECTIVE_BASEADDR);
                debugfile << "Write data " << hex << setfill('0') << setw(8)
                          << wrdata << ", dest_addr " << setfill('0') << setw(8)
                          << hex
                          << dest_addr + emulator.BLOCK_PERSPECTIVE_BASEADDR +
                                 0x02000000
                          << endl;
                emulator.st_to_scalar_spm(wraddr >> 2, wrdata);
              }
              cout << endl;
            }
          } else { // temp input
            string parenttaskport_str = input["parentTasksPort"];
            parenttaskport_str.erase(0, 2); // erase "0b"
            unsigned taskid =
                (bitset<32>(parenttaskport_str).to_ulong() >> 4) & 0b111111;
            unsigned outputid = bitset<32>(parenttaskport_str).to_ulong() & 0xF;
            cout << "The input " << input["parentTasksPort"] << " of task "
                 << taskData["debug_task_name"] << " needs the " << outputid
                 << "-th output of the " << taskid << "-th task." << endl;
            debugfile << "The input " << input["parentTasksPort"] << " of task "
                      << taskData["debug_task_name"] << " needs the " << dec
                      << outputid << "-th output of the " << dec << taskid
                      << "-th task."
                      << "Length is \"" << dec
                      << emulator.taskret[outputid][taskid].length << "\""
                      << endl;

            // Write data to VSPM
            unsigned length = emulator.taskret[outputid][taskid].length;
            char *emu_ptr = (char *)emulator.taskret[outputid][taskid].emu_ptr;
            for (unsigned byte = 0; byte < length; byte = byte + 4) {

              int wrdata = *(int *)(emu_ptr + byte);
              string dest_addr_str = input["dest_address"];
              unsigned dest_addr = stoi(dest_addr_str, nullptr, 16) + byte;
              // unsigned wraddr = dest_addr - emulator.BLOCK_VRF_OFFSET;
              if (dest_addr >= 0x100000) {
                unsigned wraddr = dest_addr - emulator.BLOCK_VRF_OFFSET;
                printf("Write data %x to address %x\n", wrdata,
                       dest_addr + emulator.BLOCK_PERSPECTIVE_BASEADDR);
                debugfile << "Write data " << hex << setfill('0') << setw(8)
                          << wrdata << ", dest_addr " << hex << setfill('0')
                          << setw(8)
                          << dest_addr + emulator.BLOCK_PERSPECTIVE_BASEADDR +
                                 0x02000000
                          << ", Length: " << dec << length << endl;
                emulator.st_to_vspm(wraddr >> 2, wrdata);
              } else {
                // unsigned wraddr = dest_addr - emulator.BLOCK_SRF_OFFSET;
                unsigned wraddr = dest_addr;
                printf("Write data %x to address %x\n", wrdata,
                       dest_addr + emulator.BLOCK_PERSPECTIVE_BASEADDR);
                debugfile << "Write data " << hex << setfill('0') << setw(8)
                          << wrdata << ", dest_addr " << hex << setfill('0')
                          << setw(8)
                          << dest_addr + emulator.BLOCK_PERSPECTIVE_BASEADDR +
                                 0x02000000
                          << ", Length: " << dec << length << endl;
                emulator.st_to_scalar_spm(wraddr >> 2, wrdata);
              }
            }
          }
        }
      }

      // Set task ID
      emulator.current_taskid = taskData["current_taskId"];
      emulator.vins_issue_idx = 0;

      // Read hex in dsl folder, add directories based on the json file
      string str = json_file_name;
      string hexpath =
          str.substr(0, str.find_last_of("\\/")) + "/../venus_test/ir/";
      // string hexpath = "../dsl/bin/";
      string hexname = taskData["debug_task_name"];
      hexname = hexname.substr(0, hexname.find_last_of("*"));
      string hexfile = hexpath + hexname + ".hex";
      // string hexname = "dag1";
      // string hexfile = hexpath + hexname + ".hex";
      cout << "Reading " + hexfile << endl;

      // Set the emulator parameters based on JSON data
      emulator.TEXT_SECTION_LENGTH = taskData["text_length"];
      emulator.DATA_SECTION_OFFSET = 0x20000;
      emulator.DATA_SECTION_LENGTH = taskData["data_length"];
      cout << "emulator.TEXT_SECTION_LENGTH: " << emulator.TEXT_SECTION_LENGTH
           << endl;
      cout << "emulator.DATA_SECTION_OFFSET: " << emulator.DATA_SECTION_OFFSET
           << endl;
      cout << "emulator.DATA_SECTION_LENGTH: " << emulator.DATA_SECTION_LENGTH
           << endl;

      emulator.init_param();
      emulator.init_emulator((char *)hexfile.data());

      for (unsigned i = 0; i < iteration; ++i) {
        emulator.emulate();

        if (emulator.ebreak) {
          for (unsigned ret = 0;
               ret < emulator.taskretnum[emulator.current_taskid]; ret++) {
            unsigned length =
                emulator.taskret[ret][emulator.current_taskid].length;
            unsigned ptr = emulator.taskret[ret][emulator.current_taskid].ptr;
            char *emu_ptr = (char *)malloc(length * sizeof(char));
            emulator.taskret[ret][emulator.current_taskid].emu_ptr =
                (char *)emu_ptr;

            debugfile << "Task ID: " << dec << emulator.current_taskid
                      << ", Task Name:" << taskData["debug_task_name"]
                      << ", Return ID: " << ret << ", Length: " << length
                      << ", Address: " << hex << setfill('0') << setw(8)
                      << ptr + 0x82000000 << endl;

            vector<string> tokens;
            string token;
            unsigned addr;
            if (emulator.compare_result) {
              string line;
              cout << "ok?: " << emulator.compare_result << endl;
              // scan the line till it found a proper read transaction
              while (getline(cmpfile, line)) {
                stringstream ss(line);

                tokens.clear();
                while (getline(ss, token, ',')) {
                  // remove spaces
                  token.erase(remove(token.begin(), token.end(), ' '),
                              token.end());
                  tokens.push_back(token);
                }

                // drop it if it is not a read transaction,
                // and the source address is not in the correct range.
                string addr_str = tokens.at(6).substr(0, 5);
                addr = stoi(addr_str, nullptr, 16);
                // cout <<"addr_str:" << addr_str <<"\n" <<"addr:" << addr
                // <<"\n";
                bool isInBlock =
                    (addr >= ((emulator.CLUSTER_PERSPECTIVE_BASEADDR +
                               emulator.CLUSTER_BLOCKS_OFFSET) >>
                              12));
                bool isInDSPM =
                    ((addr & 0xFFF) >= (emulator.BLOCK_DSPM_OFFSET >> 12) &&
                     (addr & 0xFFF) < (emulator.BLOCK_CSR_OFFSET << 12));
                // cout <<"isInBlock:" << isInBlock << "\n" <<"isInDSPM:" <<
                // isInDSPM <<"\n";
                if (tokens.at(5) == "R" && isInBlock && isInDSPM) {
                  printf("Found a read transaction.\n");
                  // cout <<"tokens:" << tokens.at(5) <<"\n" ;
                  cout << endl;
                  break;
                }
              }
            }

            // return value is a scalar
            if ((ptr >= emulator.BLOCK_PERSPECTIVE_BASEADDR) &&
                (ptr < emulator.BLOCK_PERSPECTIVE_BASEADDR +
                           emulator.BLOCK_VRF_OFFSET)) {
              for (unsigned byte = 0, rbyte = length; byte < length;
                   byte = byte + 4, rbyte = rbyte - 4) {
                int val = emulator.lw_from_scalar_spm(
                    (ptr - emulator.BLOCK_PERSPECTIVE_BASEADDR + byte) >> 2);

                *(int *)(emu_ptr + byte) = val;
                debugfile << "Task ID: " << dec << emulator.current_taskid
                          << ", Task Name:" << taskData["debug_task_name"]
                          << ", Return ID: " << ret << ", val: " << hex
                          << setw(8) << setfill('0') << val << endl;
              }
            }
            // return value is a vector
            else if (ptr >= emulator.BLOCK_VRF_OFFSET) {
              for (unsigned byte = 0, rbyte = length; byte < length;
                   byte = byte + 4, rbyte = rbyte - 4) {
                unsigned val = emulator.lw_from_vspm(
                    (ptr - emulator.BLOCK_VRF_OFFSET + byte) >> 2);

                *(int *)(emu_ptr + byte) = val;

                debugfile << "Task ID: " << dec << emulator.current_taskid
                          << ", Task Name:" << taskData["debug_task_name"]
                          << ", Return ID: " << ret << ", val: " << hex
                          << setw(8) << setfill('0') << val << endl;
                if (emulator.compare_result) {
                  unsigned pkt_idx = byte / emulator.BUS_WIDTH;
                  unsigned pkt_offset = byte % emulator.BUS_WIDTH;
                  cout << "pkt_idx: " << pkt_idx << endl;
                  cout << "byte: " << byte << endl;
                  cout << "pkt_offset: " << pkt_offset << endl;
                  cout << "Task Name: " << taskData["debug_task_name"] << endl;
                  cout << "Emulator Data: " << setw(8) << setfill('0') << val
                       << endl;
                  string hardware_str =
                      tokens.at(10 + pkt_idx)
                          .substr((emulator.BUS_WIDTH - pkt_offset - 4) * 2, 8);
                  uint32_t hardware =
                      (stoi(hardware_str.substr(0, 4), nullptr, 16) << 16) +
                      stoi(hardware_str.substr(4, 4), nullptr, 16);
                  uint32_t mask =
                      (rbyte >= 4)
                          ? (0xFFFFFFFF)
                          : ((rbyte == 3) ? (0x00FFFFFF)
                                          : ((rbyte == 2) ? (0x0000FFFF)
                                                          : (0x000000FF)));

                  debugfile
                      << hex << "show me Vector!\n"
                      << "Task Name: " << taskData["debug_task_name"] << "\n"
                      << "Emulator Data: " << setw(8) << setfill('0') << val
                      << "\n"
                      << "Byte: " << byte << "\n"
                      << "ADDR: " << addr << "\n"
                      << "VCS Data: " << setw(8) << setfill('0') << hardware
                      << "\n"
                      << "Compare Mask: " << mask << "\n"
                      << "rbyte: " << rbyte << "\n"
                      << "No. Byte: " << dec << byte << " ~ " << byte + 4
                      << endl;

                  if ((val & mask) != (hardware & mask)) {
                    cout << hex << "Compare Error Vector!\n"
                         << "Task Name: " << taskData["debug_task_name"] << "\n"
                         << "Emulator Data: " << setw(8) << setfill('0') << val
                         << "\n"
                         << "Byte: " << byte << "\n"
                         << "ADDR: " << addr << "\n"
                         << "VCS Data: " << setw(8) << setfill('0') << hardware
                         << "\n"
                         << "Compare Mask: " << mask << "\n"
                         << "rbyte: " << rbyte << "\n"
                         << "No. Byte: " << dec << byte << " ~ " << byte + 4
                         << endl;

                    debugfile.close();
                    exit(EXIT_FAILURE);
                  }
                }
              }
            }
          }
          break;
        }

        if (emulator.trap)
          exit(EXIT_FAILURE);
      }
    }
  dsl_done:
    binfile.close();
    debugfile.close();
  } else {
    emulator.init_param();
    emulator.init_emulator(argv[argc - 1]);
    for (unsigned i = 0; i < iteration; ++i) {
      // for (int i=0; !emulator.trap; ++i) {
      emulator.emulate();
      if (emulator.ebreak)
        break;
      if (emulator.trap)
        exit(EXIT_FAILURE);
    }
  }

  return 0;
}

// #include <stdio.h>
// int main(){
//     printf("Hello,World!");
//     return 0;
// }