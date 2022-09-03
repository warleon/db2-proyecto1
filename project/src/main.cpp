
#include <crow_all.h>

#include <iostream>

int main(int argc, char** argv) {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")([]() { return "Hello world"; });

  app.port(80).multithreaded().run();
  return 0;
}