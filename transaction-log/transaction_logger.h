#pragma once

#include <mutex>
#include <vector>
#include <fstream>

class transaction_logger {
public:
    transaction_logger();
    virtual ~transaction_logger();
    
    bool log(std::vector<unsigned char> data);

private:
    std::fstream log_file_;
    std::mutex lock_;
};

