//reead me 

//-- queue works perfect here 
//--implement barber thread thaat accesss the queue and 
// this fully works 

/*
readme 
  
  to make look at results better 
  - currently prints time every second and generates a customer every every secnd
        - to stop print: for time 
        -   ctrl f : type "main" commnet print for time
        -   ctrl f : type "handle_____random --- comment out print for thread wait 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Shared resource limits
#define CHAIR_____LIMIT 3
#define SOFA_____LIMIT 3
#define STANDING_____LIMIT 5
#define MAX_____SIZE (SOFA_____LIMIT + STANDING_____LIMIT)
#define BARBER_____COUNT 3 // Number of threads generating random numbers
#define MAX_____CUSTOMERS 10000  // Adjust this based on your needs
long start_____times[MAX_____CUSTOMERS];  // Array to store waiting times for each customer
long stop_____times[MAX_____CUSTOMERS]; 
long waiting_____times[MAX_____CUSTOMERS]; 
int waiting_____time_____count = 0;  // Counter to track how many customers
pthread_mutex_t waiting_____time_____mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex to protect the waiting times array

int open_____flag=1;

int barb1 = 0;
int barb2 = 0;
int barb3 = 0;


int barb1_Tot = 0;
int barb2_tot= 0;
int barb3_total = 0;

int total_____waiting_____time = 0;  
int total_____customer=0;



pthread_mutex_t calc_____mutex = PTHREAD_MUTEX_INITIALIZER;  // Separate mutex for waiting time calculation






pthread_t queue[MAX_____SIZE];  
int total_____count = 0;       
pthread_t chair_____queue[CHAIR_____LIMIT];        
int chair_____queue_____count = 0; 
pthread_mutex_t queue_____mutex= PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t customer_____ready = PTHREAD_COND_INITIALIZER;
pthread_mutex_t chair_____queue_____mutex= PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t register_____mutex = PTHREAD_MUTEX_INITIALIZER; // Initialize mutex for register
int dropped_____customers = 0; // Global counter for dropped customers
pthread_mutex_t dropped_____mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for dropped counter



int barber_____status[BARBER_____COUNT] = {0}; 
pthread_mutex_t barber_____status_____mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for barber status



int current_____index = 0;  // Shared index for recording times
int current_____index2 =0;
pthread_mutex_t time_____mutex = PTHREAD_MUTEX_INITIALIZER;




#include <stdio.h>
#include <stdlib.h>


void process_____buffer() {
    char buffer[256];

    // Clear the buffer before using it
    memset(buffer, 0, sizeof(buffer));

    // Simulate usage of buffer
    printf("Buffer cleared and ready for use.\n");
}

void clearSharedmemory() {
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    char* str = (char*) shmat(shmid, NULL, 0);
    memset(str, 0, 1024); // Clear the shared memory
    shmdt(str);
    shmctl(shmid, IPC_RMID, NULL); // Remove shared memory
    printf("Shared memory cleared.\n");
}
void record_____start_____time(long long start) {
    pthread_mutex_lock(&time_____mutex);
    if (current_____index < MAX_____CUSTOMERS) {
        start_____times[current_____index] = start;  // Store the start time
        
        printf("\n--------- customer wait time----------------\n");
        printf("\nCustomer %d started waiting at %lld seconds.\n", current_____index, start);
      
        current_____index++;
        
       // printf("\nCustomer %d started waiting at %d seconds.\n", current_____index, start);
    }
    pthread_mutex_unlock(&time_____mutex);
}

void record_____stop_____time(long long stop) {
    pthread_mutex_lock(&time_____mutex);
    if (current_____index2 < MAX_____CUSTOMERS) {
        stop_____times[current_____index2] = stop;  
         printf("\n-------- customer wait time---------------\n");
        printf("Customer %d stopped waiting at %lld seconds.\n", current_____index2, stop);
        current_____index2++;  
    }
    pthread_mutex_unlock(&time_____mutex);
}



void* register1(int time, pthread_t barber_____id) {


    pthread_mutex_lock(&register_____mutex); // Lock the register for exclusive access
     printf("-------------- Customer has started Checking out--------\n");
    printf("Register is being used for %d seconds.\n", time);
    printf("---------------------\n");
    sleep(time); 
    
    printf("\n-------------------------- Customer has finished Checking out--------------------\n");
    printf("Register is now free.\n");
 
 printf("---------------------\n");
 
 
 
 
 //  pthread_mutex_unlock(&register_____mutex); // Unlock the register
    
     if (barber_____id == 1) {
       // pthread_mutex_lock(&barber1_____mutex); // Lock for Barber 1
        barb1++;
        barb1_Tot += 12;

        //pthread_mutex_unlock(&barber1_____mutex); // Unlock for Barber 1
       printf("Barber 1 is serving currently: he has now served %d customers.\n", barb1);
    } else if (barber_____id == 2) {
      //  pthread_mutex_lock(&barber2_____mutex); // Lock for Barber 2
        barb2++;
         barb2_tot += 12;
      //  pthread_mutex_unlock(&barber2_____mutex); // Unlock for Barber 2
        printf("Barber 2 is serving currently: he has now served %d customers.\n", barb2);
    } else if (barber_____id == 3) {
       // pthread_mutex_lock(&barber3_____mutex); // Lock for Barber 3
        barb3++;
         barb3_total += 12;
      //  pthread_mutex_unlock(&barber3_____mutex); // Unlock for Barber 3
        printf("Barber 3 is serving currently: has now served %d customers.\n", barb3);
    } else {
        printf("Invalid barber ID: %ld\n", barber_____id);
    }
    
    
    printf("---------------------------- Barber totals customers Checking out--------------------------------------\n");
     printf("Barber 1 has now served %d customers. with total price= %d \n", barb1,barb1_Tot);
      printf("Barber 2 has now served %d customers. with total price = %d\n", barb2, barb2_tot);
       printf("Barber 3 has now served %d customers. with total price = %d\n", barb3, barb3_total);
        
        printf("---------------------------------------------------------------------------------------------------\n");
    
    
    total_____customer++;

   pthread_mutex_unlock(&register_____mutex); // Unlock the register



    return NULL;
}










void print_____queue() {
  // pthread_mutex_lock(&queue_____mutex); // Lock the mutex
    printf("\n----------- Queue Status --------------\n");

    // Chair
     printf("Chair: ");
    for (int i = 0; i < CHAIR_____LIMIT; i++) {
        if (chair_____queue[i] != 0) {
            printf( "customer %lu      ", (unsigned long)chair_____queue[i]); // Print thread IDs
        }
    }
    printf("\n");
    
    
    
    printf("Sofa: ");
    for (int i = 0; i < SOFA_____LIMIT; i++) {
        if (queue[i] != 0) {
            printf(" customer %lu ", (unsigned long)queue[i]); // Print thread IDs
        }
    }
    printf("\n");

    // Standing
    printf("Standing: ");
    for (int i = SOFA_____LIMIT; i < MAX_____SIZE; i++) {
        if (i < total_____count) {
            printf("customer %lu ", (unsigned long)queue[i]);
        }
    }
    printf("\n");

    printf("---------------------\n");
  //  pthread_mutex_unlock(&queue_____mutex); // Unlock the mutex
}



void* monitor_____function(void* arg) {
    while (open_____flag) {
        pthread_mutex_lock(&queue_____mutex); // Lock the mutex to safely access the queue
         printf("Monitor is watching\n");

        // Check if there are customers in the chair section
        if (total_____count > 0 && total_____count <= CHAIR_____LIMIT) {
        printf("---------------------------------------Monitor is Broadcasting-----------------------------------\n");
            printf("Monitor: Customers in the chair section, notifying barbers.\n");
            pthread_cond_broadcast(&customer_____ready); // Notify all waiting barbers
        }
  printf("---------------------------------------------------------------------------------------------------\n");
        pthread_mutex_unlock(&queue_____mutex); // Unlock the mutex after signaling
        sleep(2); // Check every 2 seconds
    }
    printf("Monitor is done\n");
    return NULL;
    
}




int add_____to_____chair_____queue(pthread_t thread_____id) {
  // pthread_mutex_lock(&chair_____queue_____mutex); // Lock the chair queue mutex
    if (chair_____queue_____count < CHAIR_____LIMIT) {
        chair_____queue[chair_____queue_____count++] = thread_____id; // Add to chair queue
         
printf("\n-----------------------------------------------------------------\n");
        printf("added customer to queue %lu\n", (unsigned long)thread_____id);
       // pthread_mutex_unlock(&chair_____queue_____mutex); // Unlock the mutex
       
       
      // printf(" QUEUE EDIT: in add cust added customer to q%lu\n", (unsigned long)thread_____id);
    //   print_____queue();
        return 1;
    }
    printf("Chair queue is full for thread %lu\n", (unsigned long)thread_____id);
 // pthread_mutex_unlock(&chair_____queue_____mutex); // Unlock the mutex
    return 0;
}


void remove_____and_____shift_____queuec() {
pthread_mutex_lock(&chair_____queue_____mutex); 
    if (chair_____queue_____count > 0) {
        for (int i = 0; i < chair_____queue_____count - 1; i++) {
            chair_____queue[i] = chair_____queue[i + 1];
        }
        chair_____queue_____count--;
    }
    pthread_mutex_unlock(&chair_____queue_____mutex);
}

void calculate_____average_____waiting_____time(int total_____customers) {
    int total_____waiting_____time = 0;

    for (int i = 0; i < total_____customers; i++) {
        int waiting_____time = stop_____times[i] - start_____times[i];
        total_____waiting_____time += waiting_____time;

        printf("Customer %d waited for %d seconds.\n", i, waiting_____time);
    }

    double average_____waiting_____time = (double)total_____waiting_____time / total_____customers;
    printf("Average waiting time per customer: %.2f seconds.\n", average_____waiting_____time);
}



void remove_____and_____shift_____queue() {
    if (total_____count > 0) {
        printf("Before shifting: ");
        for (int i = 0; i < total_____count; i++) {
            printf("customer %lu    ", (unsigned long)queue[i]);
        }
        printf("\n");

        for (int i = 0; i < total_____count - 1; i++) {
            queue[i] = queue[i + 1];
        }

        queue[total_____count - 1] = 0; 
        total_____count--;

        printf("After shifting: ");
        for (int i = 0; i < total_____count; i++) {
            printf("customer %lu    ", (unsigned long)queue[i]);
        }
        printf("\n");
    }
}


void print_____barbershop_____state(int dropped_____customers) {


    int sofa_____count = 0;
    int standing_____count = 0;
    int total_____customers = 0; 
    pthread_mutex_lock(&queue_____mutex); 
    pthread_mutex_lock(&chair_____queue_____mutex); 
   // total_____customers = chair_____queue_____count + total_____count; 
    int busy_____barbers_____count = 0; 
 printf("\n  \n");
 printf("\n  \n");
  printf("\n  \n");
    printf("\n===+++++++++++++++++++++++++++++++ BARBER SHOP STATE ++++++++++++++++++++++++++++++++++++++++++===\n");

    // Print the state of each barber
    pthread_mutex_lock(&barber_____status_____mutex);


for (int i = 0; i < BARBER_____COUNT; i++) {
    if (barber_____status[i] == 1) {
        busy_____barbers_____count++; // Increment if barber is busy
    }

    printf("Barber Thread %d: %s\n", i + 1,
           (barber_____status[i] == 1) ? "Busy" : "Idle");
}



pthread_mutex_unlock(&barber_____status_____mutex);


    printf("Cashier: %s\n", (pthread_mutex_trylock(&register_____mutex) == 0) ? "Idle" : "Busy");
    if (pthread_mutex_unlock(&register_____mutex) != 0) {
        perror("Failed to unlock register mutex");
    }

 printf("\n  \n");


    printf("\n--- Barbershop Queue Status ---\n");

    // Print chair queue
    printf("Chair Queue (Barber chairs): ");
    for (int i = 0; i < CHAIR_____LIMIT; i++) {
        if (i < chair_____queue_____count) {
            printf("customer %lu   ", (unsigned long)chair_____queue[i]);
        }
    }
    printf("\n");

    // Print sofa queue
    printf("Sofa Queue: ");
    for (int i = 0; i < SOFA_____LIMIT; i++) {
        if (queue[i] != 0) {
            printf("customer %lu   ", (unsigned long)queue[i]);
            sofa_____count++; // Count sofa customers
        }
    }
    printf("\n");

    // Print standing queue
    printf("Standing Queue: ");
    for (int i = SOFA_____LIMIT; i < MAX_____SIZE; i++) {
        if (i < total_____count) {
            printf("customer %lu   ", (unsigned long)queue[i]);
            standing_____count++; // Count standing customers
        }
    }
    
    
    printf("\n");
    total_____customers = busy_____barbers_____count + sofa_____count + standing_____count;
    // Print summary
     printf("\n  \n");
 printf("\n  \n");
    printf("\n--------------------------------- Summary: in store status ----------------------------\n");
    printf("Total Customers in the shop: %d\n", total_____customers);
    printf("Seated customers: %d\n", busy_____barbers_____count);
    printf("Sofa Queue Count: %d\n", sofa_____count);
    printf("Standing Queue Count: %d\n", standing_____count);
    printf("Dropped Customers: %d\n", dropped_____customers);

    printf("======================================\n");
    
    
   
       printf("total customer served: %d customers.\n", total_____customer);
      
        
        printf("---------------------------------------------------------------------------------------------------\n");
        
        

    pthread_mutex_unlock(&queue_____mutex); // Unlock queue mutex
    pthread_mutex_unlock(&chair_____queue_____mutex); // Unlock chair queue mutex

    pthread_mutex_unlock(&chair_____queue_____mutex); // Unlock chair queue
    pthread_mutex_unlock(&queue_____mutex);       // Unlock main queue
    
     printf("\n \n");
      printf("\n  \n");
       printf("\n  \n");
}


int add_____to_____queue(pthread_t thread_____id) {
    pthread_mutex_lock(&queue_____mutex); // Lock the mutex
    if (total_____count < MAX_____SIZE) {
        queue[total_____count++] = thread_____id; // Add thread to the queue
        time_t seconds =time(NULL);
       long long starttime1 = (long long)seconds *1000;
    printf("Customer %lu started waiting at %lld seconds.\n", (unsigned long)thread_____id, starttime1);

        pthread_cond_broadcast(&customer_____ready);
      //  printf("broadcast sent");
       print_____queue();
        pthread_mutex_unlock(&queue_____mutex); // Unlock the mutex
        
        record_____start_____time(starttime1);
         // Print the queue after adding a new thread
       //  print_____queue();
        return 1;
    }
    pthread_mutex_lock(&dropped_____mutex);
        dropped_____customers++;
        pthread_mutex_unlock(&dropped_____mutex);
     printf("Sorry, we are full no space for customer %ld\n", (long)thread_____id);
     printf("total customer who left: %d Customer dropped.\n", dropped_____customers);
    pthread_mutex_unlock(&queue_____mutex); // Unlock the mutex
    return 0; // Queue is full
}

//Function to manage the queue and assign barbers if free
// Barber thread function

void* barber_____function(void* arg) {
    int barber_____id = *(int*)arg; // Barber ID for logging purposes
    free(arg); // Free dynamically allocated ID

    printf("Barber %d is ready to work.\n", barber_____id);

    while (1) {
        pthread_mutex_lock(&queue_____mutex);

        // Wait until there are customers in the queue
        while (total_____count == 0) {
            printf("Barber %d is waiting for customers.\n", barber_____id);
            pthread_cond_wait(&customer_____ready, &queue_____mutex); // Wait for a signal
        }
        
        printf("broadcast received\n");

        // Process the first customer in the queue
        pthread_t customer_____thread = queue[0]; // Get the first customer
        time_t seconds = time(NULL);
         long long stop  = (long long)seconds *1000;
    printf("Customer %lu stop waiting at %lld seconds.\n", (unsigned long) customer_____thread, stop);
   
        add_____to_____chair_____queue(customer_____thread);
        printf("Customer %lu moved to chair queue.\n", (unsigned long)customer_____thread);
       
        printf("Barber %d is removing customer thread %lu.\n", barber_____id, (unsigned long)customer_____thread);
        
        //remove_____and_____shift_____queue1();
        remove_____and_____shift_____queue();             // Update the queue by removing the customer
        print_____queue(); 
        printf("Barber %d is serving customer thread %lu.\n", barber_____id, (unsigned long)customer_____thread);
        
        pthread_mutex_unlock(&queue_____mutex); 
         record_____stop_____time(stop);
        
        
        pthread_mutex_lock(&barber_____status_____mutex);
        barber_____status[barber_____id - 1] = 1;
        pthread_mutex_unlock(&barber_____status_____mutex);
    //    print_____queue(); // Print the queue status after removing the customer
        // problem is this isnlt reoving the value from the queue 
      
   //   printf("Barber is serving customer threadkkkkkkkk \n");
      
        int totalBarbertime = (rand() % 11) + 10; // 10 to 20 seconds
   
   
   
  
    int heckle_____wait_____time2 = (rand() % 9) + 9; // heckling time
    int register_____wait_____time3 = rand() % (3-1+1) + 3;

    // Subtract the second random number from the first
    int haircut_____time = totalBarbertime - heckle_____wait_____time2 - register_____wait_____time3;

    // Simulate barber serving a customer
    sleep(haircut_____time); 
    
    
  
    
    printf("Barber %d finished serving customer thread %lu after %d seconds.\n", barber_____id, (unsigned long)customer_____thread, haircut_____time);
    

  
    
    register1(register_____wait_____time3, barber_____id);
    
    remove_____and_____shift_____queuec();  
    
   // calculate_____waiting_____time(customer_____thread);
    pthread_mutex_lock(&barber_____status_____mutex);
    barber_____status[barber_____id - 1] = 0;
    pthread_mutex_unlock(&barber_____status_____mutex);
  //  record_____stop_____time(stop);
 // calculate_____waiting_____time(customer_____thread);
 
 printf("\nBarber  is heckling ---\n");
    sleep(heckle_____wait_____time2);
    
     printf("\nBarber finished heckling ---\n");
    print_____barbershop_____state(dropped_____customers);
    
    
 
    }

    return NULL;
}





// Function to simulate a random wait time and add the thread to the queue
void* handle_____random_____number(void* arg) {
    int random_____number = *(int*)arg; // Get the random number (in minutes)
    free(arg); // Free the memory for the argument

    // Simulate waiting for 'random_____number' minutes
    printf("Thread %ld waiting for %d minutes before adding itself to the queue...\n", (long)pthread_self(), random_____number);
    sleep(random_____number * 60); // Wait for the specified time (convert minutes to seconds)

    // After waiting, add the thread to the queue
    
     printf("\n  \n");
        printf("\n-----------------------------------adding thread to queue-------------------------\n");
    printf("Thread %ld adding itself to the queue after waiting.\n", (long)pthread_self());
    add_____to_____queue(pthread_self()); // Add the thread to the queue (NULL placeholder, replaced later)

    return NULL;
}




int main(int argc, char* argv[]) {

    process_____buffer();
    clearSharedmemory() ;
   
    // Initialize time variables (start at 9:00:00 AM)
    int hour = 9;
    int minute = 0;
    int second = 0;

    // Default duration to 5:00 PM (8 hours from 9:00 AM)
   // int duration_____minutes = (17 - 9) * 60; // 480 minutes
    
    int duration_____minutes ; // 480 minutes

    // Check for command-line argument
    printf("\n Enter the time to run the  program for: ");
  
        if (scanf("%d", &duration_____minutes) != 1 || duration_____minutes <= 0) {
            printf("Invalid input. Duration must be a positive integer. program will run till 5:00 PM.\n");
            duration_____minutes = (17 - 9) * 60;
        }
    

    printf("Program will run for %d minutes.\n", duration_____minutes);

    // Seed the random number generator
    srand(time(NULL));

    pthread_t monitor_____thread;
    pthread_t barbers[BARBER_____COUNT];

    // Create the monitor thread
    pthread_create(&monitor_____thread, NULL, monitor_____function, NULL);

    // Create barber threads
    for (int i = 0; i < BARBER_____COUNT; i++) {
        int* barber_____id = malloc(sizeof(int));
        *barber_____id = i + 1;
        pthread_create(&barbers[i], NULL, barber_____function, barber_____id);
    }

    // Simulate the store being open
    
    int elapsed_____minutes = 0;
    pthread_t delay_____thread [30000];
    int dcout =0;
    while (elapsed_____minutes < duration_____minutes) {
        // Print the current time
        printf("Current time: %02d:%02d:%02d\n", hour, minute, second);

     
        int random_____number = rand() % (22-5+1) + 5;
        printf("Generated random number (wait time): %d minutes\n", random_____number);

        // Create a thread to handle the random delay and add itself to the queue
       // pthread_t delay_____thread;
        int* thread_____arg = malloc(sizeof(int));
        *thread_____arg = random_____number; // Pass the random wait time to the thread
        if (pthread_create(&delay_____thread[dcout], NULL, handle_____random_____number, thread_____arg) != 0) {
            perror("Failed to create delay thread");
        }
       // pthread_detach(delay_____thread);

        // Simulate passing time (increment second, minute, hour)
        sleep(1);
        second++;
        dcout++;
        if (second == 60) {
            second = 0;
            minute++;
            elapsed_____minutes++;
        }
        if (minute == 60) {
            minute = 0;
            hour++;
        }
    }

    printf("Store is closed at %02d:%02d:%02d.\n", hour, minute, second);
//    dcout++;
    
 //   open_____flag=0;
    
    for (int i = 0; i < dcout; i++) {  
        pthread_cancel(delay_____thread[i]);
    }
      calculate_____average_____waiting_____time(total_____customer);

   
    print_____barbershop_____state(dropped_____customers);
    
    
    printf("All customers have been served and the barbershop is closing: printing final register.\n");
    register1(0,0);

    // Clean up
   // pthread_cancel(monitor_____thread);
    // open_____flag=0;
    for (int i = 0; i < BARBER_____COUNT; i++) {
        pthread_join(barbers[i], NULL);
    }
    
     open_____flag=0;
    pthread_cancel(monitor_____thread);
     pthread_join(monitor_____thread, NULL);
     
 
  //  print_____barbershop_____state(dropped_____customers);
    printf("All customers have been served and the barbershop is closing.\n");
   // open_____flag=0;
    // pthread_cancel(monitor_____thread);
  //   pthread_join(monitor_____thread, NULL);
     
   // open_____flag=0;
    return 0;
}

