#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <thread>
#include <mutex>
#include <iostream>
#include <sstream>
#include "protocol.hpp"
#include "mst.hpp"
#define WORKERS 20




/* Globals */

Graph graph;
mutex graph_mutex;




/* The Pipe-Line */

void EditGraph(int vxnum, int ednum, bool add, istringstream& input)
{
    int src, dst;
    char delimiter;
    float weight;

    // vertices
    for (int itr = 0; itr < vxnum; itr++) {
        input >> src;
        add ? graph.addNode(src) : graph.removeNode(src);
    }

    // edges
    for (int itr = 0; itr < ednum; itr++) {
        input >> src >> delimiter >> dst;

        if (add)
        {
            input >> delimiter >> weight;
            graph.connect(src, dst, weight);
        }
        else
        {
            graph.disconnect(src, dst);
        }
    }
}


string MSTStats(const string& algo)
{
    Graph tree = MST(algo, graph);

    graph_mutex.unlock();

    ostringstream response;
    response << "Total weight: " << total_weight(tree) << endl
             << "Min distance: " << min_edge(tree)     << endl
             << "Max distance: " << max_distance(tree) << endl
             << "Avg distance: " << avg_distance(tree) << endl;
    return response.str();
}


stringstream request_pipe_line(const string& msg)
{
    graph_mutex.lock();

    stringstream response;
    istringstream request(msg);
    string cmd; request >> cmd;

    auto proccess_mst_cmd = [&request, &cmd, &response]()
    {
        request >> cmd;

        try{ response << MSTStats(cmd); }
        catch (const exception& e) { response << e.what(); }
    };

    if (cmd == "NewGraph")
    {
        int vxnum, ednum;
        char delimiter;
        request >> vxnum >> delimiter >> ednum;

        graph = Graph(vxnum);
        EditGraph(0, ednum, true, request);
        response << "Graph built";
    }
    if (cmd == "RandomGraph")
    {
        int size;
        float density;
        char delimiter;
        request >> size >> delimiter >> density;
        
        graph = RandomGraph(size, density);
        response << "Graph built";
    }
    else if (cmd == "AddNode")
    {
        EditGraph(1, 0, true, request);
        response << "Node added";
    }
    else if (cmd == "RemoveNode")
    {
        EditGraph(1, 0, false, request);
        response << "Node removed";
    }
    else if (cmd == "Connect")
    {
        EditGraph(0, 1, true, request);
        response << "Vertices connected";
    }
    else if (cmd == "DisConnect")
    {
        EditGraph(0, 1, false, request);
        response << "Vertices disconnected";
    }
    else if (cmd == "MST")
    {
        proccess_mst_cmd();
        return response;
    }
    else
    {
        response << "Command not recognized";
    }

    response << endl;

    // Possibility for the client to compute MST immediately
    // on its latest graph version with no interaptions
    if (request >> cmd && cmd == "MST")
    {
        proccess_mst_cmd();
        return response;
    }

    graph_mutex.unlock();
    return response;
}




/* Networking */

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

        try { response = request_pipe_line(request).str(); }
        catch (exception& e) { response = "Format Error \n"; }

        boost::asio::write(socket, boost::asio::buffer(response));
    }

    socket.close();
}


int main()
{
    boost::asio::thread_pool thread_pool(WORKERS);

    boost::asio::io_context io_context;

    // TCP listener
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v6(), atoi(PORT)));

    function<void()> handle_connection;
    handle_connection = [&handle_connection, &io_context, &acceptor, &thread_pool]()
    {
        cout << "Waiting for connections..." << endl;

        // Astablish connection with a new client with its own socket
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        // new leader thread (leader-follower pattern)
        boost::asio::post(thread_pool, handle_connection);

        handle_client(move(socket));
    };

    handle_connection();
    thread_pool.join();

    return 0;
}