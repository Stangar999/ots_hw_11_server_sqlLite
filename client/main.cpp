#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace ba = boost::asio;
using ba::ip::tcp;

const std::string kOK = "OK\n";
const std::string kERR = "ERR";
const size_t kMinSize = kERR.size();

class Client {
 public:
  Client(tcp::socket sock)
      : _sock(std::move(sock)) {
    _answer.resize(4);
  }
  void do_write() {
    std::string query;
    std::getline(std::cin, query);
    _sock.async_write_some(
        ba::buffer(query),
        [this](const boost::system::error_code &ec, size_t) { do_read(); });
  }

  void do_read() {
    ba::read(_sock, s_buf,
             std::bind(&Client::completion_condition, this,
                       std::placeholders::_1, std::placeholders::_2));
    std::cout << _answer << std::endl;
    _answer.clear();
    do_write();
  }

  std::size_t completion_condition(const boost::system::error_code &error,
                                   std::size_t bytes_transferred) {
    std::istream is(&s_buf);
    std::string tmp;
    tmp.resize(8);
    while (is.readsome(tmp.data(), 8)) {
      _answer += std::string(tmp.data(), is.gcount());
    }
    if (_answer.size() > kMinSize) {
      if (_answer.substr(0, kERR.size()) == kERR && _answer.back() == '\n') {
        return 0;
      }
      if (_answer.substr(_answer.size() - kOK.size(), kOK.size()) == kOK) {
        return 0;
      }
    }
    return 8;
  }

 private:
  std::string _answer;
  tcp::socket _sock;
  ba::streambuf s_buf;
};

int main(int argc, char *argv[]) {
  ba::io_context io_context;
  tcp::endpoint ep(tcp::v4(), 9000);
  tcp::socket sock(io_context);
  sock.connect(ep);

  Client client(std::move(sock));
  client.do_write();

  io_context.run();
  return 0;
}

//    while (true) {
//      char _answer[4];
////      _answer.clear();
////      _answer.resize(4);
//      _sock.async_read_some(
//          ba::buffer(_answer),
//          [this](const boost::system::error_code &ec, size_t length) {
//            //
//            //std::cout << std::string{data, length};
//          });
//      if (std::string{_answer, length}.back() == '+') {
//        break;
//      }
//    }
//    do_write();
// do_write();
//    _answer.clear();
//    _answer.resize(4);
//    enum { length = 4 };
//    char data[length];
//    size_t len = _sock.read_some(ba::buffer(data));
//    std::cout << std::string{data, len};
//    if (std::string{data, len}.back() != '+') {
//      do_read();
//    } else {
//      std::cout << std::endl;
//    }

//             [this, data](const boost::system::error_code &ec, size_t
//             length) {
//               std::string str(data, length);
//               while (str.back() != '+') {
//                 std::cout << str << std::endl;
//                 do_read();
//               }

//               do_write();
//             });
//    _sock.async_read_some(ba::buffer(_answer),
//                          [this](const boost::system::error_code &ec,
//                          size_t) {
//                            std::cout << _answer << std::endl;
//                            do_write();
//                          });
//  }
