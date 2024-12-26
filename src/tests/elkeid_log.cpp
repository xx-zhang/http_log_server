#include <cstring>
#include<iostream>
#include <fstream>
#include<string>
#include <future>
#include<vector>
#include <sstream>
#include "vector"

#include "nlohmann/json.hpp"
#include "httplib.h"


// g++ -c hids_demo.cpp -o hids_demo -lpthread  
void get_host_uuid(std::string& uuid){
    // 这个地方需要返回我们的UUID; 这样加入到数据库中，可以知道到底是哪个设备源发过来的；
    uuid = "112-22-22--22-2-2-2-2--22"; 
}


void process_line(const std::string& line) {
    std::string uuid; 
    get_host_uuid(uuid); 

    std::istringstream iss(line);
    std::string token;
    // nlohmann::json string_vec;
    std::vector<std::string > string_vec; 

    while (std::getline(iss, token, '\x1e')) {
        string_vec.push_back(token); 
    }
    auto json_data_ptr = new nlohmann::json(); 
    auto json_data = *json_data_ptr; 
    json_data["uuid"] = uuid; 
    if (string_vec[0].empty()) {
        return; 
    }
    auto data_type = std::stoi(string_vec[0]) ; 

    if (string_vec.size() < 10 ){
        
        switch (data_type) {
            case 607:
                json_data["cls_log_type"] = "call_usermodehelper_exec_data";
                json_data["data_type"] = string_vec[0];
                json_data["exe"] = string_vec[1];
                json_data["argv"] = string_vec[2];
                json_data["wait"] = string_vec[3];
                break; 
            case 700:
                json_data["cls_log_type"] = "proc_file_hook_data";
                json_data["data_type"] = string_vec[0];
                json_data["module_name"] = string_vec[1];
                break; 
            case 701:
                json_data["cls_log_type"] = "syscall_table_hook_data";
                json_data["data_type"] = string_vec[0];
                json_data["module_name"] = string_vec[1];
                json_data["syscall_number"] = string_vec[2];
                break; 
            case 702:
                json_data["cls_log_type"] = "hidden_kernel_module_data";
                json_data["data_type"] = string_vec[0];
                json_data["module_name"] = string_vec[1];
                break; 
            case 703:
                json_data["cls_log_type"] = "interrupt_table_hook_data";
                json_data["data_type"] = string_vec[0];
                json_data["module_name"] = string_vec[1];
                json_data["interrupt_number"] = string_vec[2];
                break; 
            default: 
              break; 
        }
        
    }else {
        json_data["data_type"] = std::stoi(string_vec[0]);
        json_data["uid"] = string_vec[1];
        json_data["exe"] = string_vec[2];
        json_data["pid"] = string_vec[3];
        json_data["ppid"] = string_vec[4];
        json_data["pgid"] = string_vec[5];
        json_data["tgid"] = string_vec[6];
        json_data["sid"] = string_vec[7];
        json_data["comm"] = string_vec[8];
        json_data["nodename"] = string_vec[9];
        json_data["sessionid"] = string_vec[10];
        json_data["pns"] = string_vec[11];
        json_data["root_pns"] = string_vec[12];

        switch (data_type) {
            case 1:
                json_data["cls_log_type"] = "write_data";
                break; 
            case 2:
                json_data["cls_log_type"] = "open_data";
                break; 
            case 42:
                json_data["cls_log_type"] = "conncet_data";
                json_data["sa_family"] = string_vec[13];
                json_data["dip"] = string_vec[14];
                json_data["dport"] = string_vec[15];
                json_data["sport"] = string_vec[16];
                json_data["res"] = string_vec[17];
                break; 
            case 49:
                json_data["cls_log_type"] = "bind_data";
                json_data["sa_family"] = string_vec[13];
                json_data["sip"] = string_vec[14];
                json_data["sport"] = string_vec[15];
                json_data["res"] = string_vec[16];
                break; 
            case 59:
                json_data["cls_log_type"] = "execve_data";
                json_data["argv"] = string_vec[13];
                json_data["run_path"] = string_vec[14];
                json_data["stdin"] = string_vec[15];
                json_data["stdout"] = string_vec[16];
                json_data["dip"] = string_vec[17];
                json_data["dport"] = string_vec[18];
                json_data["sip"] = string_vec[19];
                json_data["sport"] = string_vec[20];
                json_data["sa_family"] = string_vec[21];
                json_data["pid_tree"] = string_vec[22];
                json_data["tty"] = string_vec[23];
                json_data["socket_pid"] = string_vec[24];
                json_data["ssh"] = string_vec[25];
                json_data["ld_preload"] = string_vec[26];
                json_data["res"] = string_vec[27];
                break; 
            case 82:
                json_data["cls_log_type"] = "rename_data";
                json_data["old_name"] = string_vec[13];
                json_data["new_name"] = string_vec[14];
                json_data["sb_id"] = string_vec[15];
                break; 
            case 101:
                json_data["cls_log_type"] = "ptrace_data";
                json_data["ptrace_request"] = string_vec[13];
                json_data["target_pid"] = string_vec[14];
                json_data["addr"] = string_vec[15];
                json_data["data"] = string_vec[16];
                json_data["pid_tree"] = string_vec[17];
                break; 
            case 112:
                json_data["cls_log_type"] = "setsid_data";
                break; 
            case 157:
                json_data["cls_log_type"] = "ptctl_data";
                json_data["option"] = string_vec[13];
                json_data["new_name"] = string_vec[14];
                break; 
            case 165:
                json_data["cls_log_type"] = "ptctr_data";
                json_data["pid_tree"] = string_vec[13];
                json_data["dev"] = string_vec[14];
                json_data["file_path"] = string_vec[15];
                json_data["fstype"] = string_vec[16];
                json_data["flags"] = string_vec[17];
                break; 
            case 601:
                json_data["cls_log_type"] = "dns_query_data";
                json_data["query"] = string_vec[13];
                json_data["sa_family"] = string_vec[14];
                json_data["dip"] = string_vec[15];
                json_data["dport"] = string_vec[16];
                json_data["sip"] = string_vec[17];
                json_data["sport"] = string_vec[18];
                json_data["opcode"] = string_vec[19];
                json_data["rcode"] = string_vec[20];
                break; 
            case 602:
                json_data["cls_log_type"] = "create_file_data";
                json_data["file_path"] = string_vec[13];
                json_data["dip"] = string_vec[14];
                json_data["dport"] = string_vec[15];
                json_data["sip"] = string_vec[16];
                json_data["sport"] = string_vec[17];
                json_data["sa_family"] = string_vec[18];
                json_data["socket_pid"] = string_vec[19];
                json_data["sb_id"] = string_vec[20];
                break; 
            case 603:
                json_data["cls_log_type"] = "load_module_data";
                json_data["ko_file"] = string_vec[13];
                json_data["pid_tree"] = string_vec[14];
                json_data["run_path"] = string_vec[15];
                break; 
            case 604:
                json_data["cls_log_type"] = "load_module_data";
                json_data["pid_tree"] = string_vec[13];
                json_data["old_uid"] = string_vec[14];
                json_data["res"] = string_vec[15];
                break; 
            case 605:
                json_data["cls_log_type"] = "unlink_data";
                json_data["file"] = string_vec[13];
                break;
            case 610:
                json_data["cls_log_type"] = "usb_device_event_data";
                json_data["product_info"] = string_vec[13];
                json_data["manufacturer"] = string_vec[14];
                json_data["serial"] = string_vec[15];
                json_data["action"] = string_vec[16];
                // action = 1 is USB_DEVICE_ADD
                // action = 2 is USB_DEVICE_REMOVE 
                break;
            case 611:
                json_data["cls_log_type"] = "privilege_escalation"; // 权限提升
                json_data["p_pid"] = string_vec[13];
                json_data["pid_tree"] = string_vec[14];
                json_data["p_cred"] = string_vec[15];
                json_data["c_cred"] = string_vec[16];
                break; 
            default: 
                json_data["cls_log_type"] = "no_data_type_expect_9";
                break; 
        }
    }
    // HTTP
    std::string local_path = "/api/default/elkeid_data/_json" ; 
    httplib::Headers local_headers = {
        {"Authorization", "Basic cm9vdEBleGFtcGxlLmNvbTpFSDFsbGt5Q2tGZDJpamJL"}, 
        {"User-Agent", "curl 7.28.1"}, 
        {"Connection",  "close"}, 
        {"Accept", "*/*"}
    };
    httplib::Client cli("http://127.0.0.1:5080");
    auto res = cli.Post(local_path.c_str(), local_headers, json_data.dump(), "application/json");
    
    // 检查响应是否有效
    if (res && res->status == 200) {
        // std::cout << "Response status: " << res->status << std::endl;
        // std::cout << "Response body: " << res->body << std::endl;
    } else {
        std::cout << "Request failed -> " << res->status << std::endl;
    }
    // std::cout << json_data.dump() << std::endl; 

}

// g++ -g  -o a.elf ./elkeid_log.cpp  -lpthread -I ../../contrib/include/


int main() {
    const int num_threads = 10; // 可以根据需要调整线程数量
    std::ifstream proc_file("/proc/elkeid-endpoint");
    char ch;
    std::string line;
    std::vector<std::future<void>> futures;

    if (proc_file.is_open()) {
        while (proc_file.get(ch)) {
            if (ch == '\x17') {
                if (!line.empty()) {
                    futures.push_back(std::async(std::launch::async, process_line, line));
                    if (futures.size() >= num_threads) {
                        futures.front().wait();
                        futures.erase(futures.begin());
                    }
                    line.clear();
                }
            } else {
                line += ch;
            }
        }
        if (!line.empty()) {
            std::cout << "逐行 line2" << std::endl; 
            futures.push_back(std::async(std::launch::async, process_line, line));
        }
        proc_file.close();
    } else {
        std::cout << "Unable to open /proc file"<< std::endl;
    }

    // 等待所有异步任务完成
    for (auto& future : futures) {
        future.wait();
    }

    return 0;
}
