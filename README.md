# ASIO Async Microservice Boilerplate for C++
Finding ASIO async microservice examples for C++ online can be challenging. When I started learning ASIO, the example provided by the gRPC project demonstrated a single ASIO method implementation, not an entire microservice. The STL version was too complex but thanks to Hàn Long https://github.com/chungphb/grpc-cpp for his ideas, this boilerplate code was born.

If you are new to ASIO Async, try building the example code found on Hans github before trying this code.

This ASIO high performance microservice should be used as the basis for building your own ASIO microservice in C++. But why C++, when it's easier to code in other languages? If you plan to develop massively scalable microservices and would like to minimise server resourcesthen C++ is arguably the most efficient in terms of speed and memory efficiency.

The service has been designed to handle large numbers of synchronous client connections so theoretically it is possible to handle over 500K simultaneous client connections on a single server (with appropriate memory and SSD IO) but this needs to be properly tested. Database persistence has not been added but different databases can be used to service async service calls.

Language: C++
Threading: Asynchronous input/output - ASIO
Remote procedure calls: gRPC https://grpc.io/
Data transfer: protocol buffers https://developers.google.com/protocol-buffers

gRPC was chosen for it's lightweight performance and scalability. Protocol buffers was chosen for performance and interface versioning features.

Next steps
- Test load and capacity
- Implement database persistence
- Implement authentication rules

Ideal client for this technology
- Google Flutter with gRPC and protocolbuffers https://flutter.dev/
