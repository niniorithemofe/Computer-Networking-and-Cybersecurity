#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"  // The server's IP address
#define PORT 8081              // The port the server is listening on
#define BUFFER_SIZE 1024

// Function to display menu and get the user's choice
int display_menu() {
    char input[10];
    printf("\n----- Menu -----\n");
    printf("1. Send a file to the server\n");
    printf("2. Calculator\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");

    // Read input as a string
    fgets(input, sizeof(input), stdin);

    // Check if the input is valid
    if (strlen(input) != 2 || (input[0] < '1' || input[0] > '3') || input[1] != '\n') {
        printf("Invalid input, please enter a single number (1, 2, or 3).\n");
        return -1;  // Indicate invalid choice
    }

    // Convert the valid character to an integer
    return input[0] - '0';  // Convert '1', '2', '3' to 1, 2, 3
}

// Function to send the entire file to the server
int file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1; // File exists
    }
    return 0; // File does not exist
}

void send_file(int sock, const char *filename) {
    FILE *file;
    int size;

    // Open the file to send (binary mode)
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("File open failed");
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);  // Reset file pointer

    // Send the file size
    printf("Sending file size for %s\n", filename);
    write(sock, &size, sizeof(size));

    // Send the file as a byte array
    printf("Sending file %s as byte array\n", filename);
    char send_buffer[1024];
    int bytes_read;
    while ((bytes_read = fread(send_buffer, 1, sizeof(send_buffer), file)) > 0) {
        write(sock, send_buffer, bytes_read);
    }

    fclose(file);

    // Print type of file
    if (strstr(filename, ".jpg") != NULL) {
        printf("File %s sent successfully. File type: JPG (1)\n", filename);
    } else if (strstr(filename, ".docx") != NULL) {
        printf("File %s sent successfully. File type: DOCX (2)\n", filename);
    } else {
        printf("File %s sent successfully. File type: Unknown\n", filename);
    }
    int received_bytes;
    read(sock, &received_bytes, sizeof(received_bytes));
    printf("Received %d bytes for file %s\n", received_bytes, filename);

}

int read_integer(const char *prompt) {
    char buffer[100];

    while (1) {
        printf("%s", prompt);
        fgets(buffer, sizeof(buffer), stdin);

        // Remove the newline character if present
        buffer[strcspn(buffer, "\n")] = '\0';

        // Check if the input is numeric
        int is_numeric = 1;  // Flag to check if the string is numeric
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (buffer[i] < '0' || buffer[i] > '9') {
                is_numeric = 0;  // Not numeric
                break;
            }
        }

        // If the input is numeric, convert it to an integer
        if (is_numeric) {
            return atoi(buffer);  // Convert to integer and return
        } else {
            printf("Invalid input. Please enter a valid number.\n");
        }
    }
}


// Function to read a valid operation from the user
void read_operation(char *operation) {
    while (1) {
        printf("Enter operation (add, subtract, multiply, divide) or type 'stop' to exit: ");
        scanf("%s", operation);
        if (strcmp(operation, "add") == 0 || 
            strcmp(operation, "subtract") == 0 || 
            strcmp(operation, "multiply") == 0 || 
            strcmp(operation, "divide") == 0 || 
            strcmp(operation, "stop") == 0) {
            break;  // Valid operation entered
        } else {
            printf("Invalid operation. Please enter 'add', 'subtract', 'multiply', 'divide', or 'stop'.\n");
        }
    }
}

void Calculator(int sock) { 
    ssize_t bytes_sent;
    int num1, num2;
    char buffer[BUFFER_SIZE];
    char operation[10];  // Declare the operation variable
    char result_buffer[BUFFER_SIZE];  // Buffer to hold the result
    long total_bytes_sent = 0;  // Counter for total bytes sent

    num1 = read_integer("Calculator: enter num1: ");
    sprintf(buffer, "%d\n", num1);
    bytes_sent = send(sock, buffer, strlen(buffer), 0);
    if (bytes_sent == -1) {
        perror("Error sending number1");
    } else { 
        total_bytes_sent += bytes_sent;
        printf("Number 1 sent: %d (sent %zd bytes)\n", num1, bytes_sent);
    }

    num2 = read_integer("Calculator: enter num2: ");
    sprintf(buffer, "%d\n", num2);
    bytes_sent = send(sock, buffer, strlen(buffer), 0);
    if (bytes_sent == -1) {
        perror("Error sending number2");
    } else { 
        total_bytes_sent += bytes_sent;
        printf("Number 2 sent: %d (sent %zd bytes)\n", num2, bytes_sent);
    }

    // Wait for server response
    memset(buffer, 0, BUFFER_SIZE);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server response: %s\n", buffer);
    
    while (1) {
        read_operation(operation);  // Get a valid operation

        // Check for stop command
        if (strcmp(operation, "stop") == 0) {
            write(sock, operation, strlen(operation) + 1);  // Send stop command
            total_bytes_sent += strlen(operation) + 1;  // Count stop command bytes
            break;  // Exit the loop
        }

        bytes_sent = write(sock, operation, strlen(operation) + 1);  // Send the operation
        if (bytes_sent != -1) {
            total_bytes_sent += bytes_sent;  // Update total bytes sent
        }

        // Read the result from the server
        memset(result_buffer, 0, sizeof(result_buffer));
        read(sock, result_buffer, sizeof(result_buffer));

        // Convert the received result to float for display
        float result = atof(result_buffer);
        printf("Result: %f\n", result);
    }

    // Print total bytes sent at the end of calculations
    printf("Total bytes sent during calculator operations: %ld\n", total_bytes_sent);
}



int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int choice;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to the server failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server at %s:%d\n", SERVER_IP, PORT);

    while (1) {
        // Display the menu and get the user's choice
        choice = display_menu();
        
        if (choice == -1) {
            continue;  // Invalid choice, go back to menu
        }

        sprintf(buffer, "%d", choice);
        send(sock, buffer, strlen(buffer), 0);

        // Use switch to handle the user's choice
        switch (choice) {
            case 1:
                // Send a file to the server as a whole
                 for (int i = 0; i < 3; i++) {
                  char filename[256];
                  while (1) {
                  printf("Enter file name %d: ", i + 1);
                  scanf("%s", filename);

            // Check if the file exists
                if (file_exists(filename)) {
                  send_file(sock, filename);
                  break; // Exit the loop if the file exists
            }   else {
                  printf("Error: File '%s' does not exist. Please try again.\n", filename);
            }
        }
    }
    
               int received_bytes;
               double file_time_taken;
               read(sock, &received_bytes, sizeof(received_bytes));
               read(sock, &file_time_taken, sizeof(file_time_taken));

    // Print the received data
                printf("Received %d bytes for files\n", received_bytes);
                printf("Time taken for file: %.2f milliseconds\n", file_time_taken);
                break;

            case 2:
                // Start the calculator
                Calculator(sock);
                
                
                break;

            case 3:
                // Send "exit" to the server and close the connection
                strcpy(buffer, "exit\n");
                send(sock, buffer, strlen(buffer), 0);
                printf("Disconnecting from the server.\n");
                close(sock);  // Close the socket before exiting
                return 0;
        }
        // Clear the buffer before receiving new data
        memset(buffer, 0, sizeof(buffer));
        int totalBytes;
        double timetaken;
        read(sock, &totalBytes, sizeof(totalBytes));
        read(sock, &timetaken, sizeof(timetaken));
        printf("total bytes for program Received %d bytes for files\n", totalBytes);
        printf("Time taken for file: %.2f milliseconds\n", timetaken);
        
    }
    
     
    // Close the socket if the loop is exited
    close(sock);
    return 0;
}

