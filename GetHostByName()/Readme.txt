Retrieve given Machine information using GetHostbyname()
1. Gethostbyname explanation 
   1. The project explores the use of Gethostbyname  to get the ip address and name of a given machine for example given www.bestbuy.com, the machine outputs the hostname and ip address and other information.
   2. In computer networks, www.bestbuy.com is not a machine itself but rather a domain name that points to a web server. This web server is a machine  that hosts the website for Best Buy. When you access www.bestbuy.com, the request is directed to these servers, which then delivers the website content to your browser.
   3. What is gethost by name and how is it used:
      1. gethostbyname translates human-readable hostnames (like www.bestbuy.com) into IP addresses (like 93.184.216.34), looks up the hostname in a host database
      2. a networked application that needs to connect to a remote server. would use gethostbyname to resolve the server’s hostname to an IP address, which you can then use to establish a socket connection or When you enter a URL in a web browser, the browser uses functions like gethostbyname to resolve the domain name to an IP address before establishing a connection to the web server.
      3. The function prototype looks like this struct hostent *gethostbyname(const char *name);
      4. It takes the host name and returns a pointer to a hostent structure containing information about the host. If an error occurs, it returns NULL. the lookup is done in a hostname database
      5. The hostent structure contains information about a host, including its name and IP addresses
      6. The inet_ntoa function converts the network address to a human-readable string.
   4. Error checking for null hostent and no hostname given included


3. Installation instructions 
* In terminal 
* Download the gcc compiler to compile c files 
* Download nano to edit and create files 


4. How to run code 
* Download and unzip folder 
* Cd to folder directory 
  

* Run “make” 
  

* Run the executable with the given hostname: ./main www.bestbuy.com
  



5. Sources used: 
C Program to display hostname and IP address - GeeksforGeeks




6.  My output