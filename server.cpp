#include <boost/asio/thread_pool.hpp>
#include <thread>
#include <iostream>
#include "protocol.hpp"
#define WORKERS 20




void handle_client(tcp::socket socket) 
{
    cout << "Client connected from: " << socket.remote_endpoint().address() << endl;

    while (true) 
    {
        string request(BUFSIZ, 0), response;

        boost::system::error_code error;
        size_t length = socket.read_some(boost::asio::buffer(request), error);

        if (error == boost::asio::error::eof) 
        {
            cerr << "Client quit" << endl;
            break;
        } 
        else if (error) 
        {
            throw runtime_error("Failed to receive message");
        }

        request.resize(length);

        try{ response = handle_request(request).str(); } 
        catch (exception& e){ response = "Format Error\n"; }

        boost::asio::write(socket, boost::asio::buffer(response));
    }

    socket.close();
}

int main() 
{
    boost::asio::io_context io_context;
    
    boost::asio::thread_pool thread_pool(WORKERS);

    // Server class, IPv6
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v6(), atoi(PORT)));

    cout << "Waiting for connections..." << endl;

    while (true) 
    {
        // Astablish connection with a new client with its own socket
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        // Submit the work to the thread pool (Leader-Follower pattern)
        boost::asio::post(thread_pool, [socket = std::move(socket)]() mutable 
        {
            handle_client(std::move(socket));
        });
    }

    // Wait for all threads in the pool to finish
    thread_pool.join();

    return 0;
}