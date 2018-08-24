#include <iostream>

#include "asio.hpp"

using asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 4)
    {
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port> <message>\n";
      return 1;
    }

    // create io context
    asio::io_context io_context;

    // set up the connection to the host port
    tcp::socket s(io_context);
    tcp::resolver resolver(io_context);
    asio::connect(s, resolver.resolve(argv[1], argv[2]));

    for (int i = 0; i < 2; ++i)
    {
        // write the requested string to the service
        size_t request_length = std::strlen(argv[3]);
        asio::write(s, asio::buffer(argv[3], request_length));

        // read back the response 
        std::string reply;
        reply.resize(max_length);
        asio::error_code error;
        size_t reply_length = s.read_some(asio::buffer(reply, max_length), error);

        std::cout << reply << std::endl;
    
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
