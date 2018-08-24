#include <iostream>

#include "asio.hpp"
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "async_session.h"
#include "dispatch_queue.h"

using asio::ip::tcp;
using namespace rapidjson;

async_session::async_session(tcp::socket socket, dispatch_queue* queue)
    : socket_(std::move(socket)), queue_(queue)

{
    buffer_.resize(max_length);
    std::cout << "TID: " << std::this_thread::get_id() << "\tCONSTRUCTOR" << std::endl;
}

async_session::~async_session()
{
    std::cout << "TID: " << std::this_thread::get_id() << "\tDESTRUCTOR" << std::endl;
}

void async_session::start()
{
    readRequest();
}

void async_session::readRequest()
{
    buffer_.erase();
    buffer_.resize(max_length);
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(buffer_, max_length),
        [this, self](std::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            std::cout << "TID: " << std::this_thread::get_id() << buffer_.c_str() << std::endl;
            taskPromise_ = std::promise<std::string>();
            taskFuture_ = taskPromise_.get_future();
            queue_->dispatch([this] 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::cout << "TID: " << std::this_thread::get_id() << "\tREAD: " << buffer_.c_str() << std::endl;
                Document document;
                if (document.Parse(buffer_.c_str()).HasParseError())
                {
                    rapidjson::StringBuffer s;
                    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
                    writer.StartObject();
                    writer.Key("error");
                    writer.String(GetParseError_En(document.GetParseError()));
                    writer.Key("offset");
                    writer.Uint64(document.GetErrorOffset());
                    writer.EndObject();
                    taskPromise_.set_value(s.GetString());
                }
                else
                {
                    rapidjson::StringBuffer s;
                    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
                    document.Accept(writer);
                    taskPromise_.set_value(s.GetString());
                } 
            });
            buffer_ = taskFuture_.get();
            writeResponse();
          }
        });
}

void async_session::writeResponse()
{
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(buffer_, buffer_.length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            std::replace(buffer_.begin(), buffer_.end(), '\n', ' ');
            std::cout << "TID: " << std::this_thread::get_id() << "\tWRITE: " << buffer_.c_str() << std::endl;
            readRequest();
          }
        });
}
