#include <iostream>

#include "asio.hpp"
#include "async_server.h"

using asio::ip::tcp;

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: async_tcp_echo_server <port>\n";
    return 1;
  }

  try
  {
    std::cout << "TID: " << std::this_thread::get_id() << ":\tMAIN" << std::endl;
    asio::io_context io_context;
    async_server s(io_context, std::atoi(argv[1]));
    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
