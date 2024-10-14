#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <stdexcept>
#define PORT "5000"
using boost::asio::ip::tcp;
using namespace std;




stringstream handle_request(const string& msg);