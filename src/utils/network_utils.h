#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <string>
#include <openssl/ssl.h>

// Structs for request and response information
struct RequestInfo {
    std::string ip;
    int port;
    std::string request;
    bool use_ssl;
};

struct ResponseInfo {
    std::string response;
    std::string status;
    double elapsed_time;
};

// Function declarations
void initialize_openssl();
void cleanup_openssl();
SSL_CTX *create_context();
void configure_context(SSL_CTX *ctx);
int create_socket(const std::string &ip, int port);
ResponseInfo handle_request(const RequestInfo &request_info);
void socket_req(const RequestInfo &request_info, ResponseInfo &response_info);
void get_file_content(const std::string filename, std::string &content);

#endif // NETWORK_UTILS_H
