#include <vector>
#include <chrono>
#include <fstream>
#include <iostream>

#include "transaction_logger.h"

transaction_logger::transaction_logger() 
{
    log_file_ = std::fstream("file.binary", std::ios::out | std::ios::binary);
}

transaction_logger::~transaction_logger() 
{
    log_file_.close();
}

bool transaction_logger::log(std::vector<unsigned char> data)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    lock_.lock();
    log_file_.write((char*)&data[0], data.size());
    log_file_.flush();
    lock_.unlock();
    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "LOG: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << std::endl;
    return true;
}

