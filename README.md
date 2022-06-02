# ASIO-Microservices-Template
ASIO high performance microservice template is designed to demonstrate an authentication microservice in a simple and easy to use way. If you plan to develop massively scalable micro-services for your application and would like to minimise server costs then this template might help guide the way.

The code is designed to maximise server resources and minimise power consumption through performance and optimisation. The service has been designed to handle large numbers of synchronous client connections so theoretically it is possible to handle over 500K simultaneous client connections on a single server but this needs to be tested. Database persistence has not been added but different databases can be used to service async service calls.

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
