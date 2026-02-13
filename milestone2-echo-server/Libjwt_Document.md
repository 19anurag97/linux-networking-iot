***JSON Web Token (JWT)*** is an open standard (RFC 7519\) for securely transmitting information between two parties — typically a client and a server.

A JWT has three parts, separated by dots (.):

1\. Header  
Defines the type (JWT) and the algorithm used to sign the token (e.g., HS256).

2\. Payload  
Contains the actual claims — such as user ID, expiration time, and roles.

3\. Signature  
Ensures integrity by signing the header and payload using a secret or public/private key pair.

*What are Tokens and Why Are They Needed?*

If an authentication server simply sent user data as plain JSON, malicious users could modify fields (like the user ID) before sending it back to the server.

Tokens solve this problem by **encapsulating and cryptographically signing data**, ensuring the receiver can trust its authenticity.

🔎 Encapsulation

* A **JWT (JSON Web Token)** is essentially a container that *encapsulates* data (called **claims**) in a structured way:  
  * **Header** → metadata (e.g., algorithm used, token type).  
  * **Payload** → the actual data being transmitted (user ID, role, expiry time).  
  * **Signature** → proof that the token hasn’t been tampered with.  
* Encapsulation means the claims are bundled into a single token string (`header.payload.signature`) that can be passed around easily between client and server.  
* Example: Instead of sending multiple fields separately, you send one compact token that contains everything.

🔒 Cryptographic Signing

* After encapsulating, the token is **signed** using a cryptographic algorithm:  
  * **Symmetric (HMAC/HS256)** → same secret key used to sign and verify.  
  * **Asymmetric (RSA/ECDSA)** → private key signs, public key verifies.  
* Signing produces the **Signature** part of the JWT.  
* Purpose:  
  * **Integrity** → ensures the payload hasn’t been altered.  
  * **Authenticity** → proves the token was issued by a trusted server.  
* Without signing, anyone could forge or modify tokens. With signing, any change to the payload invalidates the signature.

✅ Analogy

Think of a JWT like a **sealed envelope**:

* **Encapsulation**: You put the message (claims) inside the envelope.  
* **Signing**: You stamp it with a wax seal (cryptographic signature).  
   If someone tampers with the contents, the seal breaks and verification fails

*How Do JWTs Work?*

JWT authentication typically follows this flow:

User logs in — Authentication server validates credentials  
.  
JWT issued — Server signs and returns a JWT containing claims.

Client stores token — Usually in HttpOnly cookies or secure storage.

Requests authenticated — Client includes JWT in headers (e.g., Authorization: Bearer \<token\>).

Server verifies JWT — Using its secret or public key, checks signature validity and claim expiry.

*🔑 JWT API Functions (from libjwt)*

Here’s what each API does in your current code:

\- **jwt\_new(\&jwt)**  
Creates a new JWT object in memory.

\- **jwt\_add\_grant(jwt, "sub", USERNAME)**  
Adds a claim (here, subject \= username). Claims are key-value pairs inside the JWT payload.

\- **jwt\_add\_grant\_int(jwt, "exp", expiry\_time)**  
Adds an integer claim (expiry timestamp). This is critical for token expiration.

\- **jwt\_set\_alg(jwt, JWT\_ALG\_HS256, secret, secret\_len)**  
Sets the signing algorithm (HS256 \= HMAC-SHA256) and secret key. This ensures tokens can’t be forged.

\- **jwt\_encode\_str(jwt)**  
Encodes the JWT into a string (header.payload.signature) that can be sent to the client.

\- **jwt\_decode(\&jwt, token, secret, secret\_len)**  
Decodes and verifies a JWT string using the secret key. Returns 0 if valid, non-zero if invalid.

\- **jwt\_get\_grant\_int(jwt, "exp")**  
Retrieves an integer claim (like expiry) from the JWT payload.

\- **jwt\_free(jwt)**  
Frees memory used by the JWT object.

*Can we capture JWT data in wireshark pcap?*

Yes — JWTs are just text-based tokens (Base64-encoded JSON), so if they’re sent over the network in **plain HTTP** or inside unencrypted protocols, Wireshark can absolutely capture them in a `.pcap` file. You’ll see them in the packet payload, often in the `Authorization: Bearer` header or embedded in JSON messages.

🔎 When JWTs are Visible in Wireshark

* **Unencrypted traffic (HTTP)**: JWTs are fully visible in packet captures.  
* **WebSocket or custom TCP protocols**: If you send JWTs as part of your message, Wireshark will show them in the payload.  
* **REST APIs without TLS**: The `Authorization` header will contain the JWT in clear text.

🔒 When JWTs Are NOT Visible

* **Encrypted traffic (HTTPS/TLS)**: Wireshark can capture packets, but the payload is encrypted. You won’t see the JWT unless you decrypt TLS with the server’s private key (rare in production).  
* **VPN or secure tunnels**: Same as TLS — payload is hidden.

