#pragma once

#include "asio.hpp"
#include "dispatch_queue.h"

using asio::ip::tcp;

class async_server
{
public:
    async_server(asio::io_context& io_context, short port);

private:
    void acceptConnection();
    tcp::acceptor acceptor_;
    std::unique_ptr<dispatch_queue> queue_;
};

