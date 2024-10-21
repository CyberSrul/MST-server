#include "../protocol.hpp"




int main()
{
    boost::asio::io_context io_context;

    // Resolve server address and port
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("localhost", PORT);

    // Connect to the server
    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);

    while (true)
    {
        // send
        string message(BUFSIZ, 0);
        cout << "Enter a request for the server:" << endl;
        getline(cin, message);
        if (message == "END") break;
        boost::asio::write(socket, boost::asio::buffer(message));

        // recv
        string response(BUFSIZ, 0);
        boost::system::error_code error;
        size_t bytes_received = socket.read_some(boost::asio::buffer(response), error);

        if (error == boost::asio::error::eof) throw runtime_error("Server quit");

        else if (error) throw runtime_error("Failed to receive message");

        response.resize(bytes_received);
        cout << "Server replied: " << response << endl;
    }

    return 0;
}