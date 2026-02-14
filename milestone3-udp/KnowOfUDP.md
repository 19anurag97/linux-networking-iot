**Key Differences from TCP**
+ No listen() or accept() → UDP is connectionless.  
+ sendto() / recvfrom() instead of send() / recv().  
+ Each message must include the destination address.  
+ No guarantee of delivery, ordering, or duplication protection (that’s TCP’s job).

*Implement 1*:
+ UDP client-server to include a basic **reliability layer with timeouts and retransmissions**. 
+ This simulates how protocols like TFTP or custom IoT stacks build reliability on top of UDP.


*Concept:*
- UDP doesn’t guarantee delivery, so we add:
- Acknowledgments (ACKs): Server replies with an ACK for each message.
- Timeouts: Client waits for ACK; if none arrives within X seconds, it retransmits.
- Retries: Client retries a limited number of times before giving up.
