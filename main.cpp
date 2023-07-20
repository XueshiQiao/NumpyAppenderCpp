#include <_types/_uint8_t.h>
#include <iostream>
#include <sys/types.h>

#include "numpy_appender.hpp"

int main() {
  std::cout << "Hello, World!" << std::endl;
  std::string file_name = "/Users/joey/Downloads/test.npy";
  size_t i = 0, j = 3, k = 4;
  std::vector<size_t> shape = {i, j, k};
  numpy_appender::NumpyAppender appender(file_name, shape);
  appender.Open();
  appender.WriteHeader();
  uint8_t data = 0;
  std::vector<uint8_t> data_vec(j * k);
  // for (int x = 0; x < i; x++) {
    for (int y = 0; y < j; y++) {
      for (int z = 0; z < k; z++) {
        data_vec[data] = data;
        data++;
      }
    }
  // }
  appender.AppendNumpyItem(data_vec.data(), data_vec.size());
  appender.AppendNumpyItem(data_vec.data(), data_vec.size());
  appender.AppendNumpyItem(data_vec.data(), data_vec.size());
  appender.AppendNumpyItem(data_vec.data(), data_vec.size());
  appender.AppendNumpyItem(data_vec.data(), data_vec.size());

  appender.Close();

  return 0;
}
