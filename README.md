# Linux Networking Projects in C for IoT

(A) *milestone1-tcp-basic* : This folder contains TCP client/server implementation.
                           -> Server contains a timeout of 2 minutes.
                              If this timeout limit is reached and no connection from client is accepted. 
                              Then, server code will close the socket descriptor and shows an error message.


(B) *milestone2-echo-server* : Server echoes back whatever client sends.
                             -> A simple username/password authentication step is added.
                             -> **Token-Based Authentication** added. *libjwt* included.

(C) *milestone3-udp*     : Implement client-server using UDP sockets.
                           -> Added classic Stop-and-Wait ARQ design. Since UDP itself is unreliable
                              (no retransmissions, no ordering guarantees), Stop-and-Wait ARQ can be implemented
                              at the application layer to add reliability.