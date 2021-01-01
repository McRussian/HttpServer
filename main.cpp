#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <fcntl.h>
#include <algorithm>
#include <thread>
#include "arg_parser.h"


std::string get_full_path(const std::string &request) {
    std::string full_path = request.substr(
            request.find('/'),
            request.size() - request.find('/'));
    full_path = "." + full_path;
    full_path.erase(
            std::remove(full_path.begin(), full_path.end(), ' '), full_path.end());
    if (full_path == "/") full_path = "index.html";
    return full_path;
}

bool handle_get_request(int socket, std::string request) {
    if (request.find("HTTP") != std::string::npos) // NOLINT(abseil-string-find-str-contains)
        request = request.substr(0, request.find("HTTP"));
    if (request.find('?') != std::string::npos)
        request = request.substr(0, request.find('?'));

    std::string fullpath = get_full_path(request);
    std::string response;

    int requested_fd = open(fullpath.c_str(), O_RDONLY);
    if (requested_fd == -1) {
        response = "HTTP/1.0 404 NOT FOUND\r\nContent-Length: 0\r\nContent-Type: text/html\r\n";
        send(socket, response.c_str(), response.size(), 0);
        return false;
    }

    response = "HTTP/1.0 200 OK\r\n\r\n";
    send(socket, response.c_str(), response.size(), 0);
    char readBuf[1024];
    while (int cntRead = read(requested_fd, readBuf, 1024)) {
        send(socket, readBuf, cntRead, 0);
    }
    close(requested_fd);
    return true;
}

bool worker(int socket) {
    char buf[2048];

    int read_size = read(socket, buf, 2048);
    if (read_size == -1 || read_size == 0) {
        shutdown(socket, SHUT_RDWR);
        close(socket);
        return false;
    }

    std::string requestStr(buf, (unsigned long) read_size);

    if (requestStr.size() > 3 && "GET" == requestStr.substr(0, 3)) {
        handle_get_request(socket, requestStr);
    } else {
        std::string response = "HTTP/1.0 400 Bad Request\r\nContent-Length: 0\r\nContent-Type: text/html\r\n\r\n";
        send(socket, response.c_str(), response.size(), 0);
    }

    shutdown(socket, SHUT_RDWR);
    close(socket);
    return true;
}


int main(int argc, char **argv) {
    auto port = "12345";
    auto ip = "127.0.0.1";
    auto directory = "tmp";

    parse_args(argc, argv, port, ip, directory);

    struct sockaddr_in host_addr{};
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(atoi(port));
    inet_aton(ip, &host_addr.sin_addr);

    int masterSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(masterSocket, (struct sockaddr *) &host_addr, sizeof(host_addr)))
        return 1;

    if (!fork()) {
        setsid();
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        chdir(directory);

        if (listen(masterSocket, SOMAXCONN))
            return 2;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
        while (true) {
            int slaveSocket = accept(masterSocket, nullptr, nullptr);
            std::thread thread(worker, slaveSocket);
            thread.detach();
        }
#pragma clang diagnostic pop
    }

    return 0;
}
