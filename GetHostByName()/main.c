// C program to display hostname
// and IP address
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h> 

// Returns host information corresponding to host name

int main(int argc, char *argv[])
{
    if(argc < 2) {
    	printf("usage:%s <hostname>\n",argv[0]);
    	return 1;
}
    //for time execuetion time 
    clock_t start,end;
    start = clock();

    double exeTime;

    char *IPbuffer;
    struct hostent *host_entry;

    char *hostname = argv[1];

    





    host_entry = gethostbyname(hostname);
    if (host_entry == NULL)
    {
        herror("gethostbyname");
        return 1;
    }

 
    // To convert an Internet network
    // address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*)
                        host_entry->h_addr_list[0]));
 
    
     if (IPbuffer == NULL)
    {
        fprintf(stderr, "inet_ntoa failed\n");

        return 1;
    }

    printf("Hostname: %s\n", hostname); printf("Host IP: %s\n", IPbuffer);
    // Print aliases
    printf("Aliases:\n");
    for (char **alias = host_entry->h_aliases; *alias != NULL; alias++) {
        printf("  %s\n", *alias);
    }

    // Print address type
    printf("Address type: %s\n", (host_entry->h_addrtype == AF_INET) ? "AF_INET" : "Unknown");

    // Print address length
    printf("Address length: %d bytes\n", host_entry->h_length);

    // Print all IP addresses
    printf("IP Addresses:\n");
    for (char **addr = host_entry->h_addr_list; *addr != NULL; addr++) {
        printf("  %s\n", inet_ntoa(*((struct in_addr*)*addr)));
    }


    end = clock();
    exeTime = ((double) (end-start))/ CLOCKS_PER_SEC;
    printf("execuetion time: %f seconds\n", exeTime);
    return 0;
}
