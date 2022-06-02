#include <iostream>
#include <rpc/server.h>

void foo() { std::cout << "Hey, I'm a free function." << std::endl; }
int  add(int a, int b) { return a + b; }
int  main() {
  rpc::server srv(8080); // listen on TCP port 8080

  srv.bind("foo", &foo);
  srv.bind("add", &add);
  srv.bind("divide", [](float num, float den) {
    std::cout << "Hey, I'm a lambda!" << std::endl;
    return num / den;
	 });

  srv.run(); // blocking call, handlers run on this thread.
  return 0;
}