#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

#include "request_handler.h"

using boost::asio::ip::tcp;
// namespace tc = boost::asio::ip;

class Session : public std::enable_shared_from_this<Session> {
 public:
  Session(tcp::socket socket)
      : socket_(std::move(socket)),
        _rh(),
        _data(128, ' ') // TODO все что не влезло будет отправлено в следующий такт
		// Как можно принимать все что есть за один раз, или как правильно принимать?
		{}

  void start() {
    do_read();
  }

 private:
  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        boost::asio::buffer(_data),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            _cur_command.append(_data);
            std::string aswer = _rh.exec_query(std::move(_cur_command));
            _cur_command.clear();
            _data.assign(128, ' ');
            do_write(std::move(aswer));
          }
        });
  }

  void do_write(std::string aswer) {
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_, boost::asio::buffer(aswer),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            do_read();
          }
        });
  }

  tcp::socket socket_;
  RequestHandler _rh;
  std::string _data;
  //  enum { max_length = 1024 };
  //  char data_[max_length];
  std::string _cur_command;
};

class Server {
 public:
  Server(boost::asio::io_context &io_context, uint port)
      : _acc(io_context, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
  }

 private:
  void do_accept() {
    _acc.async_accept([this](boost::system::error_code ec, tcp::socket sock) {
      if (!ec) {
        std::make_shared<Session>(std::move(sock))->start();
      }
      do_accept();
    });
  }
  boost::asio::ip::tcp::acceptor _acc;
};
