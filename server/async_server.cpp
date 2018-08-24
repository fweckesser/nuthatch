#include "asio.hpp"
#include "async_server.h"
#include "async_session.h"

using asio::ip::tcp;

async_server::async_server(asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      queue_(new dispatch_queue(4))

{
    acceptConnection();
}

void async_server::acceptConnection()
{
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<async_session>(std::move(socket), queue_.get())->start();
          }
          acceptConnection();
        });
}
