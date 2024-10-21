#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <thread>
#include <mutex>
#include <iostream>
#include <sstream>
#include "protocol.hpp"
#include "../MST/mst.hpp"
#define WORKERS 20




/* Common Graph */

Graph graph;
mutex graph_mutex;



/* The Pipe-Line */

void EditGraph(int vxnum, int ednum, bool add, istringstream& input)
{
    int src, dst;
    char delimiter;
    float weight;

    // vertices
    for (int itr = 0; itr < vxnum; itr++)
    {
        if (! input >> src) throw invalid_argument("partial input");;
        add ? graph.addNode(src) : graph.removeNode(src);
    }

    // edges
    for (int itr = 0; itr < ednum; itr++)
    {
        if (! (input >> src >> delimiter >> dst))
            throw invalid_argument("partial input");

        if (add)
        {
            if (! (input >> delimiter >> weight))
                throw invalid_argument("partial input");

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


    if (cmd == "NewGraph")
    {
        int vxnum, ednum; char delimiter;
        if (! (request >> vxnum >> delimiter >> ednum))
            throw invalid_argument("partial input");

        graph = Graph(vxnum);
        EditGraph(0, ednum, true, request);
        response << "Graph built";
    }
    else if (cmd == "RandomGraph")
    {
        int size; float density; char delimiter;
        if (! (request >> size >> delimiter >> density))
            throw invalid_argument("partial input");
        
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
        if (! (request >> cmd)) throw invalid_argument("partial input");

        response << MSTStats(cmd);
        return response;
    }
    else
    {
        response << "Command not recognized";
    }
    // Possibility for the client to compute MST immediately
    // on its latest graph version with no interaptions
    if (request >> cmd && cmd == "MST")
    {
        if (! (request >> cmd)) throw invalid_argument("partial input");

        response << MSTStats(cmd);
        return response;
    }


    graph_mutex.unlock();
    response << endl;
    return response;
}




/* Networking */

void handle_client(tcp::socket socket)
{
    cout << "Client connected from: " << socket.remote_endpoint().address() << endl;

    while (true)
    {
        string request(BUFSIZ, 0), response(BUFSIZ, 0);

        boost::system::error_code error;
        size_t length = socket.read_some(boost::asio::buffer(request), error);

        if (error == boost::asio::error::eof)
        {
            cerr << "Client quit" << endl;
            break;
        }
        else if (error) throw runtime_error("Failed to receive message");

        request.resize(length);

        try{ response = request_pipe_line(request).str(); }
        catch (exception& e)
        {
            graph_mutex.unlock();
            response = e.what();
        }

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