# 3PC

Implementation of 3 Phase Commit Protocol with a termination protocol.

-------------------------------------------------------------------------------
REQUIREMENTS
-------------------------------------------------------------------------------

1. A UNIX operating system.

2. GCC or clang (To compile the C++ code).

3. GNU Make.

-------------------------------------------------------------------------------
COMPILING THE CODE
-------------------------------------------------------------------------------

1. At the root directory (the directory containing this README), execute the
   "make" command.

2. There are three executables - server, server\_fail, and client.

-------------------------------------------------------------------------------
EXECUTING THE CODE
-------------------------------------------------------------------------------

1. The code requires 3 instances of the server program running in separate
   console instances or 2 instances of the server program and 1 instance of the
   server\_fail program. The server program takes an integer as an argument,
   which represents its ID (ID's are 0, 1, 2). The server\_fail program takes
   an integer (its ID, same as the server program), an integer representing the
   transaction ID it should fail at, and an integer which tells us when it
   should fail. This integer is 0 if it should fail after receiving the
   START\_VOTE message, or 1 if it should fail after receiving the PRE\_COMMIT
   message.
   Open 3 separate consoles/terminals and execute the commands "./server 0",
   "./server 1", "./server 2" separately. There is no check as of now for
   conflicting server ID's. In order to shut the server, enter "Ctrl+C" in the
   console/terminal window.

2. To execute the client(s), decide the number of clients first. We show a
   sample execution of 2 clients.
   Each client takes 4 arguments - starting transaction ID, difference between
   two transaction IDs, time between transactions (in milliseconds), time after
   which the client should stop (in seconds).
   To execute 2 clients, execute the command:
   "./client 1 2 200 10 & ./client 2 2 100 10 && fg". This executes two clients
   simultaneously.
   The first client generates transaction ID's 1,3,5,... and generates a new
   transaction every 200ms and ends after 10s have passed.
   The second client generates transaction ID's 2,4,6... and generates a new
   transaction every 100ms and ends after 10s have passed.
   IMPORTANT - In order to simplify the design of the server, we assume that the
   client provides "unique" transaction ID's. Hence the first two arguments to
   the client.

-------------------------------------------------------------------------------
SERVER OUTPUT
-------------------------------------------------------------------------------

The server outputs the messages it receives and the actions it is taking.
An example is:- "[20]Site 2 got COMMIT from Site 1". The number in square
brackets refers to the transaction ID. The message it received is in capitals.
After the server commits or aborts, it displays a similar message with either
"COMMITTED" or "ABORTED" in the output.

-------------------------------------------------------------------------------
CLIENT OUTPUT
-------------------------------------------------------------------------------

The client output contains the transaction ID and the site it sends the
transaction to. An example is:- 
"Transaction ID: 194, Site ID :0, message: TRANSACTION,194,1231,0"
The message part of the output is what is actually sent to the server.

-------------------------------------------------------------------------------
NOTE
-------------------------------------------------------------------------------

1. Between consecutive runs of the server programs (stopping all servers and 
   starting them again), we observed that the client failed to connect to the
   server and crashed. We could not find the source of this problem, though it
   could be due to the sockets not being released correctly. Giving around half
   a minute to a minute between executions stops this from happening.

2. The client gives a floating point exception in some rare cases. We will try
   to solve this error before the presentation (happens because of the timing
   code).

Contribution

Shikhar Singh - 50%
Rohit Rangan - 50%
