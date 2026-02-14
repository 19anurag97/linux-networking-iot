**Key Differences from TCP**
+ No listen() or accept() → UDP is connectionless.  
+ sendto() / recvfrom() instead of send() / recv().  
+ Each message must include the destination address.  
+ No guarantee of delivery, ordering, or duplication protection (that’s TCP’s job).

*Implementation*:
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

*Throughput vs Bandwidth vs Latency*
- Bandwidth: The maximum capacity of the link (like the width of a pipe).
- Latency: The time it takes for a packet to travel from sender to receiver (the delay).
- Throughput: The actual rate of successful data transfer (how much water flows through the pipe per second).

*Why Throughput Suffers in ARQ Design*
- On high‑bandwidth but high‑latency links (e.g., satellite connections, transcontinental fiber):
- Stop‑and‑Wait ARQ inefficiency: Only one packet is sent at a time, and the sender waits for an ACK before sending the next.
- If latency is 500 ms, each packet takes at least 1 second round‑trip before the next can be sent.
- Even if the link supports 1 Gbps, throughput is limited to the size of one packet per RTT.
- Bandwidth‑Delay Product (BDP): This is the amount of data that can be “in flight” at once.
- On high‑latency links, if you don’t fill the pipe with enough packets, the throughput will be far below the available bandwidth.
- *Result*: The network feels “slow” despite having high bandwidth, because the protocol doesn’t utilize the full capacity
