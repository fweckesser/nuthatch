#include <vector>
#include <random>
#include <numeric>
#include <iostream>
#include <algorithm>

#include "transaction_logger.h"

using namespace std;

std::vector<unsigned char> GenerateData(std::size_t bytes)
{
    std::vector<unsigned char> data(bytes);
    std::iota(data.begin(), data.end(), 0);
    std::shuffle(data.begin(), data.end(), std::mt19937{ std::random_device{}() });
    return data;
}

int main(int argc, char** argv) 
{
  /*
  if (argc != 2)
  {
    std::cerr << "Usage: tlog <log count>\n";
    return 1;
  }
  */
  try
  {
      transaction_logger tl;
      for (int i {0}; i < 1024; ++i)
      {
          auto data = GenerateData(65535);
          tl.log(data);
      }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

