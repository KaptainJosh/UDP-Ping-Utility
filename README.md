# UDP-Ping-Utility
This project is a UDP ping utility that utilizes a client/server model through Linux sockets.

How to use:
1. The svr.c and cli.c should be run on two different computers/VMs on the same network. 
2. Compile and run svr.c using the syntax "./svr <port>" where port is the port number on the computer/VM that the server will be listening on.
3. Compile and run cli.c using the sytax "./cli <hostname> <port>" where hostname is the hostname of the computer/VM the svr.c file is running and port is the port number that the svr.c is running on.
4. At this point the client should begin pinging the server and will report on statistics when finished.
