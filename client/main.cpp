#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace ba = boost::asio;
using ba::ip::tcp;

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
    while (true) {
      enum { length = 40 };
      char data[length];
	  // TODO все что не влезло будет принято в следующий такт.
      // Как можно принимать все что есть за один раз, или как правильно принимать?
	  // в условии написано признак конца '\n', но ведь в ответе типа  
	  //> INTERSECTION
      //< 3,violation,proposal неявно /n
      //< 4,quality,example неявно /n
      //< 5,precision,lake неявно /n
      //< OK  неявно /n
	  // соответственно если выводить до символа '\n' то оставшаяся часть 
	  // будет получена только после следующуго запроса пользователя
	  // и этот смещние будет постоянно
	  // как правильно надо получать сообщения
      size_t len = _sock.read_some(ba::buffer(data));
      std::cout << std::string{data, len};
	  // TODO работает потому что для тестовых примеров ответ влезает в 40 или по
      // последний символ в сообщении не выпадает на '\n'
      if (std::string{data, len}.back() == '\n') {
        break;
      }
    }
    do_write();
  }

 private:
  std::string _answer;
  tcp::socket _sock;
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
