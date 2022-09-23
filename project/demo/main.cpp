#define CROW_STATIC_DIRECTORY "/static/"
#include <crow_all.h>

#include <filesystem>
#include <hashTable.hpp>
#include <iostream>

int main(int argc, char** argv) {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")
  ([]() { return "hola"; });

  CROW_ROUTE(app, "/add/").methods("POST"_method)([](const crow::request& req) {
    return req.body;
  });
  CROW_ROUTE(app, "/search/")
      .methods("POST"_method)(
          [](const crow::request& req) { return req.body; });
  CROW_ROUTE(app, "/delete/")
      .methods("POST"_method)(
          [](const crow::request& req) { return req.body; });
  CROW_ROUTE(app, "/index/")
      .methods("POST"_method)(
          [](const crow::request& req) { return req.body; });

  app.port(80).multithreaded().run();
  return 0;
}