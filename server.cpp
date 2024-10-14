#include <thread>
#include "protocol.hpp"



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

        else if (error) throw runtime_error("Failed to receive message");

        request.resize(length);

        try { response = handle_request(request).str(); }
        catch(exception& e) { response = "Format Error \n";}

        boost::asio::write(socket, boost::asio::buffer(response));
    }

    socket.close();
}

int main()
{
    boost::asio::io_context io_context;

    // Server class, IPv6
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v6(), atoi(PORT)));

    cout << "waiting for connections" << endl;

    while (true)
    {
        // Open sockets for clients
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        // Launch a new thread to handle the client
        try { thread(handle_client, move(socket)).detach(); }
        catch (exception& e) { cerr << "Exception: " << e.what() << endl; }
    }

    return 0;
}