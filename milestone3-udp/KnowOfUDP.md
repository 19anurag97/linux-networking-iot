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

**Stop-and-Wait ARQ Basics**
- Each packet carries a sequence number (0 or 1).
- The receiver sends back an ACK with the same sequence number.
- The sender waits for the ACK before sending the next packet.
- If the ACK doesn’t arrive within a timeout → retransmit.
- This prevents duplicates and ensures ordered delivery.

*Purpose*
- This is the **classic Stop-and-Wait ARQ design**:
- Only two sequence numbers are needed (0 and 1).
- They prevent duplicates: if the server sees the same sequence number twice, it knows the packet was retransmitted.
- Once the correct ACK is received, the client toggles to the other sequence number for the next packet.