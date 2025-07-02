#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 8081
#define BUFFER_SIZE 1024
#define MAX_FILES 3

int total_bytes_sent = 0;       // Global variable to track total bytes sent
int total_bytes_received = 0;    // Global variable to track total bytes received

// Function to handle file reception
void receive_file(int sock, int file_index, int *total_bytes_received, double *total_time_taken) {
    int size;

    // Read file size
    printf("Reading file size for file %d\n", file_index);
    read(sock, &size, sizeof(int));

    // Prepare to receive the file
    char filename[20];
    snprintf(filename, sizeof(filename), "received_file%d", file_index);  // Create unique filename

    printf("Receiving file %d as '%s'...\n", file_index, filename);
    FILE *file = fopen(filename, "wb");  // Save file with unique name
    if (file == NULL) {
        perror("File open failed");
        return;
    }

    char recv_buffer[1024];
    int bytes_received;
    int total_bytes = 0;  // Bytes received for this file

    // Start timing for this file
    clock_t file_start_time = clock();

    // Keep reading data until we receive the expected file size
    while (total_bytes < size) {
        bytes_received = read(sock, recv_buffer, sizeof(recv_buffer));
        if (bytes_received < 0) {
            perror("Receive failed");
            break;
        }
        fwrite(recv_buffer, 1, bytes_received, file);
        total_bytes += bytes_received;  // Update bytes received for this file
    }

    fclose(file);
    printf("File %d received successfully as '%s'\n", file_index, filename);

    // End timing for this file
    clock_t file_end_time = clock();
    double file_time_taken = (double)(file_end_time - file_start_time) * 1000 / CLOCKS_PER_SEC;  // Calculate time taken in milliseconds

    // Update total bytes received and total time taken
    *total_bytes_received += total_bytes;
    *total_time_taken += file_time_taken;  // Sum the time taken

    // Print total bytes received and time taken for this file
    printf("File %d: Total bytes received: %d, Time taken: %.2f milliseconds\n", file_index, total_bytes, file_time_taken);

    // Send total bytes received for this file back to the client
    write(sock, &total_bytes, sizeof(total_bytes));
}

void receiveNumCalculator(int sock, double *calculator_time_taken) {
    char buffer[BUFFER_SIZE];
    float num1, num2;
    char operation[10];
    char result_buffer[BUFFER_SIZE];

    // Start timing for calculator function
    clock_t calculator_start_time = clock();

    // Receive first number
    memset(buffer, 0, BUFFER_SIZE);
    int bytes = recv(sock, buffer, sizeof(buffer), 0);
    total_bytes_received += bytes;  // Track received bytes
    num1 = atof(buffer);  // Convert string to float
    printf("Number 1: %f\n", num1);

    // Receive second number
    memset(buffer, 0, BUFFER_SIZE);
    bytes = recv(sock, buffer, sizeof(buffer), 0);
    total_bytes_received += bytes;  // Track received bytes
    num2 = atof(buffer);  // Convert string to float
    printf("Number 2: %f\n", num2);

    // Prepare acknowledgment message
    sprintf(buffer, "Numbers received: %f and %f. Please enter a calculation or type 'stop' to exit.", num1, num2);
    send(sock, buffer, strlen(buffer), 0);
    total_bytes_sent += strlen(buffer);  // Track sent bytes

    while (1) {
        // Read the operation from the client
        memset(operation, 0, sizeof(operation));
        bytes = read(sock, operation, sizeof(operation));
        total_bytes_received += bytes;  // Track received bytes

        // Check for stop command
        if (strcmp(operation, "stop") == 0) {
            printf("Stopping calculations as requested by the client.\n");
            break;  // Exit the loop
        }

        // Perform the calculation and convert result to string
        float result;
        if (strcmp(operation, "add") == 0) {
            result = num1 + num2;
        } else if (strcmp(operation, "subtract") == 0) {
            result = num1 - num2;
        } else if (strcmp(operation, "multiply") == 0) {
            result = num1 * num2;
        } else if (strcmp(operation, "divide") == 0) {
            if (num2 != 0) {
                result = num1 / num2;
            } else {
                const char *error_msg = "Error: Division by zero.";
                write(sock, error_msg, strlen(error_msg) + 1);
                total_bytes_sent += strlen(error_msg) + 1;  // Track sent bytes
                continue;  // Skip sending the result, continue the loop
            }
        } else {
            const char *invalid_msg = "Invalid operation. Please enter 'add', 'subtract', 'multiply', 'divide', or 'stop'.";
            write(sock, invalid_msg, strlen(invalid_msg) + 1);
            total_bytes_sent += strlen(invalid_msg) + 1;  // Track sent bytes
            continue;  // Skip sending the result, continue the loop
        }

        // Send the result back to the client as a string
        sprintf(result_buffer, "%f", result);
        write(sock, result_buffer, strlen(result_buffer) + 1);  // Include null terminator
        total_bytes_sent += strlen(result_buffer) + 1;  // Track sent bytes
    }

    // End timing for calculator function
    clock_t calculator_end_time = clock();
    *calculator_time_taken = (double)(calculator_end_time - calculator_start_time) * 1000 / CLOCKS_PER_SEC;  // Calculate time taken in milliseconds

    // Print total time taken for the calculator function
    printf("Total time taken for calculator function: %.2f milliseconds\n", *calculator_time_taken);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the specified IP and port
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        // Accept a client connection
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sock < 0) {
            perror("Accept failed");
            close(server_sock);
            exit(EXIT_FAILURE);
        }

        printf("Client connected.\n");

        // Variables to accumulate total bytes and time
        int total_bytes_received_accumulated = 0;
        int total_bytes_sent_accumulated = 0;
        double total_time_taken_accumulated = 0.0;

        // Loop to handle client requests
        while (1) {
            // Receive the choice from the client
            int choice;
            memset(buffer, 0, BUFFER_SIZE);
            recv(client_sock, buffer, sizeof(buffer), 0);
            choice = atoi(buffer);
            printf("Client choice: %d\n", choice);

            // Handle file transfer or calculator function based on client's choice
            if (choice == 1) {
                int total_bytes_received = 0;  // Reset for each file transfer
                double total_time_taken = 0.0; // Reset for each file transfer

                clock_t start_time = clock();
                for (int file_index = 1; file_index <= MAX_FILES; file_index++) {
                    receive_file(client_sock, file_index, &total_bytes_received, &total_time_taken);
                }
                clock_t end_time = clock();
                total_time_taken += (double)(end_time - start_time) * 1000 / CLOCKS_PER_SEC;  // Calculate total time taken in milliseconds

                // Update accumulated totals
                total_bytes_received_accumulated += total_bytes_received;
                total_time_taken_accumulated += total_time_taken;

                // Send total bytes received and total time taken to the client
                write(client_sock, &total_bytes_received, sizeof(total_bytes_received));
                write(client_sock, &total_time_taken, sizeof(total_time_taken));

                // Print total bytes received and time taken
                printf("Total bytes received for all files: %d\n", total_bytes_received);
                printf("Total time taken to receive all files: %.2f milliseconds\n", total_time_taken);
            } else if (choice == 2) {
                double calculator_time_taken = 0.0; // Reset for each calculation session
                receiveNumCalculator(client_sock, &calculator_time_taken); // Pass address to update time

                // Update accumulated totals for the calculator
                total_bytes_received_accumulated += total_bytes_received; // Track received bytes
                total_bytes_sent_accumulated += total_bytes_sent;         // Track sent bytes
                total_time_taken_accumulated += calculator_time_taken;    // Update total time
                
            } else if (choice == 3) {
                printf("Client requested to exit.\n");
                break;  // Exit the loop and close the connection
            } else {
                printf("Invalid choice received from client.\n");
            }
       //     printf("Total bytes received during the session: %d\n", total_bytes_received_accumulated);
      //  printf("Total bytes sent during the session: %d\n", total_bytes_sent_accumulated);
       // printf("Total time taken during the session: %.2f milliseconds\n", total_time_taken_accumulated);

        // Print overall totals
        
        
        printf("Overall total bytes received: %d\n", total_bytes_received_accumulated);
        printf("Overall total time taken: %.2f milliseconds\n", total_time_taken_accumulated);
        write(client_sock, &total_bytes_received_accumulated, sizeof(total_bytes_received_accumulated));
        write(client_sock, &total_time_taken_accumulated, sizeof(total_time_taken_accumulated));
      //  write(client_sock, &total_bytes_sent_accumulated, sizeof(total_bytes_sent_accumulated));
       // write(client_sock, &total_time_taken_accumulated, sizeof(total_time_taken_accumulated));


        }
        

        // Print the accumulated totals after the client disconnect
      
        //write(client_sock, &total_bytes_received_accumulated, sizeof(total_bytes_received_accumulated));
        //write(client_sock, &total_bytes_sent_accumulated, sizeof(total_bytes_sent_accumulated));
        // Close the client socket after handling the request
       // close(client_sock);
        //printf("Client disconnected.\n");
    }

    // Close the server socket
    close(server_sock);
    return 0;
}

