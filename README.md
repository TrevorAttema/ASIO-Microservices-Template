# ASIO-Microservices-Template
ASIO high performance microservice template is designed to demonstrate an authentication microservice in a simple and easy to use way.
Database persistence has not been added but different databases can be used to service async service calls

The code is designed to maximise server resources and minimise power consumption through performance and optimisation. The service has been designed to handle large numbers of synchronous client connections so theoretically it is possible to handle over 500K simultaneous client connections on a single server but this needs to be tested.

Language: C++
Threading: Asynchronous input/output - ASIO
Remote procedure calls: gRPC https://grpc.io/
Data transfer: protocol buffers https://developers.google.com/protocol-buffers

Next steps
- Test load and capacity
- Implement database persistence
- Implement authentication rules

Ideal client for this technology
- Google Flutter https://flutter.dev/
