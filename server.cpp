#include <thread>
#include "protocol.hpp"



void handle_client(tcp::socket socket)
{
    cout << "Client connected from: " << socket.remote_endpoint().address() << endl;

    while (true)
    {
        // Buffer for receiving data
        string buffer(1024, 0);

        // Reciving message from client
        boost::system::error_code error;
        size_t length = socket.read_some(boost::asio::buffer(buffer), error);

        if (error == boost::asio::error::eof)
        {
            cerr << "Client quit" << endl;
            break;
        }

        else if (error) throw runtime_error("Failed to receive message");

        buffer.resize(length);
        cout << "Message received: " << buffer << endl;

        // Echo message back to client
        boost::asio::write(socket, boost::asio::buffer(buffer));
        cout << "Message echoed back to client." << endl;
    }

    socket.close();
}

int main()
{
    boost::asio::io_context io_context;

    // Create acceptor to listen on port 12345
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v6(), atoi(PORT)));

    cout << "waiting for connections" << endl;

    while (true)
    {
        // Create a socket for the next client connection
        tcp::socket socket(io_context);

        // Block until we get a client connection
        acceptor.accept(socket);

        // Launch a new thread to handle the client
        try { thread(handle_client, move(socket)).detach(); }
        catch (exception& e) { cerr << "Exception: " << e.what() << "\n"; }
    }

    return 0;
}