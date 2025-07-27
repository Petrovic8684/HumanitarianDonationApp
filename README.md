# Humanitarian Donation Application ❤️‍🩹

## About

A client-server C application developed as a university assignment for the course **Computer Networks and Telecommunications**, designed for processing humanitarian donation payments.

## Features

- User registration and login system
- Secure payment input (card number, CVV, amount)
- Validations using regular expressions
- Storage and retrieval of payment history
- Total collected donation sum
- Multi-client support using threads on the server side

## Technologies

    - C (POSIX Sockets, Threads)
    - TCP/IP
    - Regex
    - File I/O

## How to Run

### Prerequisites

    - GCC or any C compiler

    - POSIX-compliant OS (Linux, macOS, WSL on Windows)

### Compilation

```bash
# Compile server
cd Server
make

# Compile client
cd ../Client
make
```

### Execution

```bash
# Start the server
cd ../Server
./server

# In another terminal, start the client:
cd ../Client
./client
# Make sure IP address and port in both client and server match
```

## Notes

    - All payments are stored in payment.txt

    - Total donation amount is tracked in totalsum.txt

    - Only registered users can view payment history

## License

This project is developed for academic purposes and is not intended for production use.
