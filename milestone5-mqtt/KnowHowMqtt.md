# MQTT and JWT Authentication

## 🔑 Does MQTT Use JWT for Authorization?
- **MQTT protocol itself** does not mandate JWT.
- **Modern brokers** (e.g., Mosquitto Pro, EMQX, ThingsBoard TBMQ) support JWT-based authentication as an **optional mechanism**.
- JWT allows integration with **OAuth 2.0** and identity providers.

### How JWT Fits into MQTT
- **Traditional MQTT** → Username/password or TLS certificates.
- **JWT Integration** → Broker accepts JWTs issued by IdP/OAuth 2.0 server.
- **Authorization** → Broker validates token, checks claims (roles, topic permissions).
- **Fallback** → TLS + certificates still widely used.

👉 **Takeaway**: MQTT doesn’t require JWT, but JWT is useful in **large-scale IoT systems** for centralized identity management and fine-grained authorization.

---

## 📖 Is JWT One-Time or Per-Message?
- JWT is **not attached to every MQTT message**.
- It is presented **once during the CONNECT handshake**.
- After validation, the session is authenticated and subsequent messages don’t carry JWT.

### Typical Flow
1. Client obtains JWT from IdP.
2. Client sends CONNECT packet with JWT (often in password field).
3. Broker validates JWT (signature, expiry, claims).
4. Session established → Publish/Subscribe without resending JWT.
5. If JWT expires → Client must reconnect with a fresh token.

### ⚠️ Overheads
- **Handshake overhead**: JWT validation heavier than static password.
- **No per-message overhead**: MQTT messages remain lightweight.
- **Session renewal overhead**: Short-lived tokens require frequent reconnects.

---

## 🔌 MQTT in Embedded Sensor Systems
### Client Role (Sensor Device)
- Sensor (ESP32, STM32, etc.) acts as MQTT client.
- Connects to broker via TCP/IP (Wi-Fi/Ethernet).
- Publishes sensor readings (temperature, humidity, voltage, etc.).

### Broker Role (Server)
- Receives published data.
- Manages subscriptions and forwards data.
- Decouples sensors from consumers.

### Subscriber Role (Application/Cloud)
- Subscribes to topics.
- Receives sensor data in real time for monitoring, logging, or control.

---

## 🎛 QoS Levels in Pub/Sub Model
### Who Chooses QoS?
- **Publisher** → Chooses QoS (0, 1, 2) when sending.
- **Subscriber** → Specifies maximum QoS it accepts.
- **Effective QoS** → Lower of publisher’s and subscriber’s QoS.

**Example**:  
Publisher sends QoS 2, Subscriber requests QoS 1 → Broker delivers at QoS 1.

### Broker Responsibilities
- **Message Routing** → Forwards published messages to subscribers.
- **QoS Enforcement**:
  - QoS 0 → Fire-and-forget.
  - QoS 1 → Stores until acknowledgment, may resend duplicates.
  - QoS 2 → Handshake ensures exactly-once delivery.
- **Session Management** → Tracks connections, subscriptions, retained messages.
- **Security** → Authentication, authorization, TLS encryption.
- **Last Will Messages** → Publishes predefined message if client disconnects unexpectedly.

---

## 🔄 Can Subscriber Send Data to Publisher?
- **Not directly** → All communication flows through the broker.
- Subscriber can publish its own message to a topic the publisher is subscribed to.
- Broker handles routing of this “response” message.

---

## 📌 Summary
- MQTT does **not require JWT**, but brokers support it for modern identity integration.
- JWT is used **once per connection**, not per message.
- MQTT enables **lightweight, reliable communication** between sensors and servers.
- QoS ensures **delivery guarantees** tailored to publisher/subscriber needs.
- Communication is always **broker-mediated**, even between publisher and subscriber.
