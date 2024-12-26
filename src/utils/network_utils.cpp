#include "./network_utils.h"

#include <iostream>
#include <chrono>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fstream>

// Function implementations
void initialize_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
}

SSL_CTX *create_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx) {
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
}

int create_socket(const std::string &ip, int port) {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    int timeout = 20; // 20 seconds
    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof tv);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    return sock;
}

ResponseInfo handle_request(const RequestInfo &request_info) {
    ResponseInfo response_info;
    auto start = std::chrono::high_resolution_clock::now();
    try {
        int sock = create_socket(request_info.ip, request_info.port);

        if (request_info.use_ssl) {
            SSL_CTX *ctx = create_context();
            configure_context(ctx);

            SSL *ssl = SSL_new(ctx);
            SSL_set_fd(ssl, sock);

            if (SSL_connect(ssl) <= 0) {
                ERR_print_errors_fp(stderr);
                response_info.status = "SSL Connection Failed";
            } else {
                SSL_write(ssl, request_info.request.c_str(), request_info.request.length());

                char buffer[4096];
                int bytes = SSL_read(ssl, buffer, sizeof(buffer));
                if (bytes <= 0) {
                    response_info.status = "Timeout";
                } else {
                    buffer[bytes] = '\0';
                    response_info.response = std::string(buffer);

                    SSL_shutdown(ssl);
                    SSL_free(ssl);
                }
            }

            SSL_CTX_free(ctx);
        } else {
            send(sock, request_info.request.c_str(), request_info.request.length(), 0);

            char buffer[4096];
            int bytes = recv(sock, buffer, sizeof(buffer), 0);

            if (bytes < 0) {
                response_info.status = "Faild";
                response_info.response = "Receive failed: ";
            } else if (bytes == 0) {
                response_info.status = "Faild";
                response_info.response = "Connection closed by peer";
            } else {
                buffer[bytes] = '\0';
                response_info.status = "Success";
                response_info.response = std::string(buffer);
            }
        }
        close(sock);
    } catch (...) {
        response_info.status = "Failed";
        response_info.response = "Socket Connect Error";
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    response_info.elapsed_time = elapsed.count();

    return response_info;
}

void socket_req(const RequestInfo &request_info, ResponseInfo &response_info) {
    if (request_info.use_ssl == true) {
        initialize_openssl();
    }

    response_info = handle_request(request_info);

    if (request_info.use_ssl == true) {
        cleanup_openssl();
    }
}

void get_file_content(const std::string filename, std::string &content) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "无法打开文件" << std::endl;
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    content.resize(size);
    file.read(&content[0], size);

    file.close();
}
