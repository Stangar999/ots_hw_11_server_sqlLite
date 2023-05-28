#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

#include "request_handler.h"

using boost::asio::ip::tcp;
namespace ba = boost::asio;

class Session : public std::enable_shared_from_this<Session> {
 public:
  Session(tcp::socket socket)
      : socket_(std::move(socket)),
        _rh() {
  }

  void start() {
    do_read();
  }

 private:
  void do_read() {
    auto self(shared_from_this());
    ba::async_read(
        socket_, s_buf,
        std::bind(&Session::completion_condition, this, std::placeholders::_1,
                  std::placeholders::_2),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            std::string aswer = _rh.exec_query(std::move(_cur_command));
            _cur_command.clear();
            do_write(std::move(aswer));
          }
        });
  }

  std::size_t completion_condition(const boost::system::error_code &error,
                                   std::size_t bytes_transferred) {
    std::istream is(&s_buf);
    std::string tmp;
    tmp.resize(8);
    _cur_command += _path_next_command;
    _path_next_command.clear();
    while (is.readsome(tmp.data(), 8)) {
      std::string dmp = std::string(tmp.data(), is.gcount());
      int end_pos = dmp.find("\\n");
      if (end_pos >= 0) {
        _cur_command += std::string(dmp.data(), end_pos + 2);
        _path_next_command = dmp.substr(end_pos + 2);
        return 0;
      }
      _cur_command += dmp;
    }
    return 8;
  }

  //  void do_read() {
  //    auto self(shared_from_this());
  //    socket_.async_read(
  //        boost::asio::buffer(_data),
  //        [this, self](boost::system::error_code ec, std::size_t length) {
  //          if (!ec) {
  //            _cur_command.append(_data);
  //            std::string aswer = _rh.exec_query(std::move(_cur_command));
  //            _cur_command.clear();
  //            _data.assign(128, ' ');
  //            do_write(std::move(aswer));
  //          }
  //        });
  //  }

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
  ba::streambuf s_buf;
  std::string _cur_command;
  std::string _path_next_command;
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
