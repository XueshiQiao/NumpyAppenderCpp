#ifndef __NUMPY_APPENDER_HPP__
#define __NUMPY_APPENDER_HPP__

#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>

namespace numpy_appender {

class NumpyAppender {
 public:
  using Shape = std::vector<size_t>;

  enum Order {
    kCOrder,
    // kFortranOrder // not supported yet
  };

  struct Options {
    Options() : order(kCOrder), header_size(kDefaultHeaderSize) {}

    Order order{kCOrder};
    uint16_t header_size{kDefaultHeaderSize};  // must be divided by 64
  private:
    constexpr static uint16_t kDefaultHeaderSize = 128;
  };

  NumpyAppender(std::string file_name, Shape shape, Options options = Options())
      : file_name_(std::move(file_name)),
        shape_(std::move(shape)),
        options_(std::move(options)) {
    shape_item_size_ = 1;
    for (size_t i = 0; i < shape_.size(); ++i) {
      if (i > 0) {
        shape_item_size_ *= shape_[i];
      }
    }
  }

  ~NumpyAppender() { Close(); }

  bool Open() {
    if (file_) {
      std::cerr << "File already open" << std::endl;
      return false;
    }
    file_ = std::make_unique<std::ofstream>();
    // set mode be binary and write
    file_->open(file_name_, std::ios::binary | std::ios::out);
    // file_->open(file_name_, std::ios::binary);
    if (!file_->is_open()) {
      std::cerr << "Error opening file " << file_name_ << std::endl;
      return false;
    }
    return true;
  }

  void Close() {
    if (file_) {
      file_->seekp(0, std::ios_base::beg);
      WriteHeader();
      file_->close();
      file_.reset();
    }
  }

  void WriteHeader() {
    WriteData("\x93NUMPY", 6);
    uint8_t major_version = 0x01;
    uint8_t minor_version = 0x00;
    WriteData(&major_version, 1);
    WriteData(&minor_version, 1);
    std::stringstream ss;
    ss << "{'descr': '|u1', 'fortran_order': "
        << (options_.order == kCOrder ? "False" : "True")
        << ", 'shape': (";
    for (size_t i = 0; i < shape_.size(); ++i) {
      ss << std::to_string(shape_[i]);
      if (i != shape_.size() - 1) {
        ss << ", ";
      }
    }
    ss << "), }";
    std::string header = ss.str();
    size_t total_header_size = 128;  // must be divided by 64
    uint16_t header_size = total_header_size - 10;
    uint16_t header_padding = header_size - header.size();

    // host to little endian
    // uint16_t little_endian_header_size = (header_size >> 8) | (header_size <<
    // 8); should be little endian

    // write little endian of "header_size" using WriteData

    WriteData(reinterpret_cast<uint8_t *>(&header_size), sizeof(header_size));
    WriteData(header.c_str(), header.size());
    char newline = '\n';
    char padding_char = 0x20;
    for (size_t i = 0; i < header_padding; ++i) {
      if (i != header_padding - 1) {
        WriteData(&padding_char, 1);
      } else {
        WriteData(&newline, 1);
      }
    }
  }

  bool AppendNumpyItem(const uint8_t *data, size_t size) {
    if (size != shape_item_size_) {
      std::cerr << "Data size not match" << std::endl;
      return false;
    }
    shape_[0] += 1;
    return WriteData(data, size);
  }

  bool WriteData(const uint8_t *data, size_t size) {
    return WriteData(reinterpret_cast<const char *>(data), size);
  }

  bool WriteData(const char *data, size_t size) {
    if (!file_) {
      std::cerr << "File not open" << std::endl;
      return false;
    }
    file_->write(data, size);
    return true;
  }

private:
  std::string file_name_;
  Shape shape_;
  Options options_;
  size_t shape_item_size_{0};
  std::unique_ptr<std::ofstream> file_;
};

}  // namespace numpy_appender

#endif  // __NUMPY_APPENDER_HPP__
