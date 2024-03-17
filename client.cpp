#include <iostream>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/op/tcp.hpp>
#include <array>
#include <string>

using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::resolver resolv{ioservice};
tcp::socket tcp_socket{ioservice};
std::array<char, 4096> bytes;

void read_handler(const boost::system::error_code &ec, std::size_t bytes_transferred) {
  if (!ec) {
    std::cout.write(bytes.data(), bytes_transferred);
    tcp_socket.async_read_some(buffer(bytes), read_handler);
  } else {
    std::cerr << "Read error: " << ec.message() << std::endl;
  }
}

void connect_handler(const boost::system::error_code &ec) {
  if (!ec) {
    std::string request = "GET / HTTP/1.1\r\nHost: google.com\r\n\r\n";
    write(tcp_socket, buffer(request));
    tcp_socket.async_read_some(buffer(bytes), read_handler);
  } else {
    std::cerr << "Connection error: " << ec.message() << std::endl;
  }
}

void resolve_handler(const boost::system::error_code &ec, tcp::resolver::iterator it) {
  if (!ec) {
    tcp_socket.async_connect(*it, connect_handler);
  } else {
    std::cerr << "Resolve error: " << ec.message() << std::endl;
  }
}

int main() {
  tcp::resolver::query query{"google.com", "80"};
  resolv.async_resolve(query, resolve_handler);
  ioservice.run();
  return 0;
}
