#define CROW_STATIC_DIRECTORY "/static/"
#include <crow_all.h>

#include <filesystem>
#include <hashTable.hpp>
#include <iostream>

int main(int argc, char** argv) {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")
  ([]() { return "hola"; });

  app.port(80).multithreaded().run();
  return 0;
}