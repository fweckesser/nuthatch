#pragma once

#include <future>

#include "asio.hpp"

class dispatch_queue;

using asio::ip::tcp;

class async_session
  : public std::enable_shared_from_this<async_session>
{
public:
    async_session(tcp::socket socket, dispatch_queue* queue);
    ~async_session();
    void start();

private:
    void readRequest();
    void writeResponse();

    enum { max_length = 1024 };
    tcp::socket socket_;
    std::string buffer_;
    std::promise<std::string> taskPromise_;
    std::future<std::string> taskFuture_;
    dispatch_queue* queue_;
    unsigned session_;
};

