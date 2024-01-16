#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fstream>

const int MAX_EVENTS = 10;
const int PORT = 8080;
const char* RESPONSE_MESSAGE = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: 15\r\n"
                               "\r\n"
                               "Hello, Client!\n";

void handle_client(int clientSocket) {
    // Read client data
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == -1) {
        perror("Error reading from client");
        return;
    }

    // Print and store client data in a file
    std::cout << "Received from client: " << buffer << std::endl;
    std::ofstream outputFile("client_data.txt", std::ios::app);
    if (outputFile.is_open()) {
        outputFile << "Client Data: " << buffer << std::endl;
        outputFile.close();
    } else {
        std::cerr << "Error opening file for writing" << std::endl;
    }

    // Send dummy response
    send(clientSocket, RESPONSE_MESSAGE, strlen(RESPONSE_MESSAGE), 0);

    // Close the client socket
    close(clientSocket);
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        return -1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error binding");
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Error listening");
        close(serverSocket);
        return -1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    struct pollfd fds[MAX_EVENTS];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;

    while (true) {
        int status = poll(fds, MAX_EVENTS, -1);
        if (status == -1) {
            perror("Error in poll");
            break;
        }

        if (fds[0].revents & POLLIN) {
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == -1) {
                perror("Error accepting connection");
                continue;
            }
            std::cout << "Client connected" << std::endl;

            // Handle the client
            handle_client(clientSocket);

            std::cout << "Client disconnected." << std::endl;
        }
    }
    close(serverSocket);
    return 0;
}
