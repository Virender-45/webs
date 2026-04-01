# 💻 C++ TCP Server using Winsock

## 📌 Overview

This project is a simple TCP server built in C++ using the Winsock library on Windows.
It demonstrates the basic steps of socket programming including initialization, binding, listening, accepting client connections, and sending/receiving data.

---

## 🚀 Features

* Initialize Winsock (WSAStartup)
* Create a TCP socket
* Bind socket to a port
* Listen for incoming connections
* Accept client connections
* Receive data from client
* Send response to client
* Proper cleanup of resources

---

## 🛠️ Technologies Used

* C++
* Winsock2 (Windows Socket API)

---

## 📂 How It Works

1. Initialize Winsock
2. Create a server socket
3. Bind socket to a specific port (e.g., 54000)
4. Listen for incoming client connections
5. Accept a connection
6. Receive message from client
7. Send response back to client

---

## ▶️ How to Run

### 1. Clone the repository

```bash
git clone https://github.com/your-username/your-repo-name.git
cd your-repo-name
```

### 2. Open in Visual Studio

* Open `.sln` file (if available)
* Or create a new C++ project and paste the code

### 3. Build and Run

* Press **Ctrl + F5** to run the server

---

## 🔌 Testing the Server

You can connect using:

* Telnet:

```bash
telnet 127.0.0.1 54000
```

* Or create your own client using C++

---

## ⚠️ Notes

* The server is blocking (waits for client connection)
* Works on Windows only (uses Winsock)
* Make sure port is not already in use

---

## 📌 Future Improvements

* Handle multiple clients (multithreading)
* Add error handling improvements
* Build a client-server chat application
* Convert to non-blocking or async server

---

## 👨‍💻 Author

Virender

---

## 📜 License

This project is open-source and available under the MIT License.
