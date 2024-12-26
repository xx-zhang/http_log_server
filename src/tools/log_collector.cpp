#include <csignal>
// #include <unistd.h>
#include <cstddef>
#include <cstring>
#include <netdb.h>
#include <openssl/lhash.h>
#include <string>

#include "nlohmann/json.hpp"
#include "gflags/gflags.h"

#include <workflow/WFTaskFactory.h>
#include <workflow/Workflow.h>
#include "workflow/WFHttpServer.h"
#include "workflow/WFFacilities.h"

// #include "workflow/HttpMessage.h"
#include "spdlog/spdlog.h"

#include "utils/coraza_log_helper.h"

// cmd: ./d.elf -log_server http://127.0.0.1:5080 
// g++ -lworkflow -lpthread -lgflags -lssl -lcrypto ./log_collector.cpp -o d.elf -I ../ -I ../../contrib/include/ -lyaml-cpp 
DEFINE_int32(port, 8811, "the server port.");
DEFINE_int32(max_per_send, 500, "the max number of logs for per send.");
DEFINE_string(log_type, "openobserve", "log to dest server default is openobserve");
DEFINE_string(log_server, "http://127.0.0.1:9080/openobserve", "the db file path, like /opt/db/");
DEFINE_string(log_auth, "Basic cm9vdEBleGFtcGxlLmNvbTpFSDFsbGt5Q2tGZDJpamJL", "http basic auto for log dest server.");
DEFINE_string(log_cfg_path, "/opt/hibot/src/data/log_waf.yaml", "log waf yaml path for remap key&value");
// DEFINE_string(index_prefix, "waf", "the prefix of db name of index");

#define REDIRECT_MAX 5
#define RETRY_MAX 2

nlohmann::json log_cfg_json; 
nlohmann::json log_buffer = {}; // 实际上是一个array, 只是用 nlhanmman/json 包裹了
std::mutex buffer_mutex;
// std::string url_log_type; 

const static bool program_terminate = true;
// const static size_t max_per_send = 10;

void http_callback(WFHttpTask *task) {
    protocol::HttpResponse *resp = task->get_resp();
    int state = task->get_state();
    int error = task->get_error();

    auto status_code = resp->get_status_code();

    if (state != WFT_STATE_SUCCESS or strcmp(status_code, "200") != 0) {
        auto phrae = resp->get_reason_phrase();
        std::string response_body;
        resp->append_output_body(response_body.data(), response_body.size());
        spdlog::error("http task error: state = {}, error = {}, status_code = {}, phrase = {} - {}",
                      state, error, status_code, phrae, response_body);
        return;
    }

}


nlohmann::json parse_post_data_standard(nlohmann::json json_data, const std::string log_type)
{
    auto x = parse_log_standard(log_cfg_json, json_data, log_type); 
    return x; 
}

void log_to_openobserve(const nlohmann::json log_buffer_data, const std::string log_type) {
    /** */
    // 下面这个打印的size是字符串size, 如果是1代表这个里面是空；
    spdlog::info("sent logs of type [{}] size is [{}]", log_type, log_buffer_data.size());
    WFHttpTask *http_task;
    std::string local_url = FLAGS_log_server + "/api/default/" + log_type + "/_json";
    http_task = WFTaskFactory::create_http_task(local_url,
                                                REDIRECT_MAX,
                                                RETRY_MAX,
                                                http_callback);
    protocol::HttpRequest *req = http_task->get_req();
    req->set_method("POST");
    // 这个地方进行handle data 为我们的内容；
    auto post_body = log_buffer_data.dump();
    req->append_output_body(post_body.data(), post_body.size());

    // req->add_header_pair("Content-Type", "application/x-www-form-urlencoded");
    // req->add_header_pair("Content-Type", "application/json");
    // spdlog::warn("authntication is {}", FLAGS_log_auth); 
    req->add_header_pair("Authorization", FLAGS_log_auth);
    req->add_header_pair("User-Agent", "curl 7.88.1");
    req->add_header_pair("Connection", "close");
    req->add_header_pair("Accept", "*/*");

    /* Limit the http response size to 100M. */
    protocol::HttpResponse *resp = http_task->get_resp();
    resp->set_size_limit(100 * 1024 * 1024);
    http_task->set_receive_timeout(10 * 1000);
    http_task->start();
}

void log_aggregator(const std::string log_type) {
    // function is to send to remote
    nlohmann::json _logs;
    {
        std::lock_guard<std::mutex> lock(buffer_mutex);
        _logs.swap(log_buffer);
        log_buffer = {}; // 将 log buffer 进行恢复到空。后面有了再进行逐步添加。
    }
    if (!_logs.empty()) {
        log_to_openobserve(_logs, log_type);
    }
}

void add_log_entry(const std::string log_entry, const std::string log_type) {
    {
        std::lock_guard<std::mutex> lock(buffer_mutex);

        nlohmann::json json_data = nlohmann::json::parse(log_entry); 
        if(json_data.is_array()){
            for(auto log_item: json_data){
                auto x = parse_post_data_standard(log_item, log_type);
                if( ! x.empty()){
                    log_buffer.push_back(x);
                }
            }
            return ;
        }

        if(json_data.is_object()){
            auto x = parse_post_data_standard(json_data, log_type);
            if( ! x.empty()){
                log_buffer.push_back(x);
            }
            return ; 
        }
    
    }

    if (log_buffer.size() >= FLAGS_max_per_send) {
        log_aggregator(log_type);
    }
}

void handle_query(WFHttpTask *server_task, const std::string log_prefix, const std::string post_data) {
    // add_log_entry(post_data, log_prefix);
    auto log_type = get_middle_str(log_prefix); 
    // spdlog::warn("[debug] input log_prefix is {}   ->   {}", log_prefix, log_type);

    WFGoTask *go_task = WFTaskFactory::create_go_task("common", add_log_entry, post_data, log_type);
    go_task->start(); 

    WFTimerTask *timer = WFTaskFactory::create_timer_task("test", 2, 0, [=](WFTimerTask *task) {
        // spdlog::warn("timer callback, state = %d, error = %d.\n", task->get_state(), task->get_error());
        log_aggregator(log_type);
    });
    // 这里串接http的任务； 如果任务怎么样就执行；
    // timertask后面执行相关的任务，如果条目达到就会进行操作；
    timer->start();
}

void process(WFHttpTask *server_task) {
    protocol::HttpRequest *req = server_task->get_req();
    protocol::HttpResponse *resp = server_task->get_resp();
    // protocol::HttpHeaderCursor cursor(req);

    char buf[8192];
    const auto request_url = req->get_request_uri();

    // TODO 除了我们约定的URL其他的都不准访问。
    if (strcmp(req->get_method(), "POST") == 0) {
        const void *body_ptr = nullptr;
        size_t body_size = 0;
        req->get_parsed_body(&body_ptr, &body_size);
        auto post_data = (std::string) static_cast<char *>((char *)body_ptr);
        std::string response_body = "OK\r\n";
        try {
            handle_query(server_task, request_url, post_data); // 开始进行处理 server_task
        } catch (...) {
            spdlog::error("handle query EROROR and query is {}", post_data);
            response_body = "ER\r\n";
        }

        resp->set_header_pair("Content-Type", "plain/text");
        resp->append_output_body(response_body.data(), response_body.size());

    } else {
        resp->set_status_code("405");
        resp->set_reason_phrase("Method Not Allowed");
    }
}

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo) {
    wait_group.done();
}

int main(int argc, char *argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    // google::InitGoogleLogging(argv[0]);
    spdlog::warn("[MAIN] start server 0.0.0.0:{}", FLAGS_port);

    get_log_cfg_json(FLAGS_log_cfg_path, log_cfg_json); 

    signal(SIGINT, sig_handler);
    WFHttpServer server(process);

    if (server.start(FLAGS_port) == 0) {
        wait_group.wait();
        server.stop();
    } else {
        perror("Cannot start server");
        exit(1);
    }

    return 0;
}

// 计数器和时间的限制；
// https://github.com/sogou/workflow/blob/master/docs/about-counter.md
// https://github.com/sogou/workflow/blob/master/docs/about-timer.md
