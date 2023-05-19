#include <boost/asio.hpp>
#include <cstdarg>
#include <iostream>

#include "server.h"
#include "sqlite3.h"

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: async_tcp_join_server <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    Server server(io_context, std::atoi(argv[1]));

    io_context.run();
  } catch (const std::exception &ex) {
    std::cerr << "Exception: " << ex.what() << "\n";
  }

  return 0;
}
