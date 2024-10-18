#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <thread>
#include <iostream>
#include <sstream>
#include <mutex>
#include "protocol.hpp"
#include "mst.hpp"
#define WORKERS 20




/* Globals */

Graph graph;
mutex graph_mutex;




/* Active objects */

class MSTFactory
{
public:

    Graph BuildTree(const string& algo)
    {
        lock_guard<mutex> lock(graph_mutex);

        return MST(algo, graph);
    }
};


class TreeAnalyser
{
public:

    string GetStats(const Graph& tree)
    {
        ostringstream response;
        response << "Total weight: " << total_weight(tree) << endl
                 << "Min distance: " << min_edge(tree)     << endl
                 << "Max distance: " << max_distance(tree) << endl
                 << "Avg distance: " << avg_distance(tree);
        return response.str();
    }
};




/* The Pipe-Line */

string MSTPipeLine(const string& algo)
{
    static MSTFactory mstf;
    static TreeAnalyser analyser;

    return analyser.GetStats(mstf.BuildTree(algo));
}




/* Networking */

void EditGraph(int vxnum, int ednum, bool add, istringstream& input)
{
    lock_guard<mutex> lock(graph_mutex);

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


stringstream handle_request(const string& msg)
{
    stringstream response;
    istringstream request(msg);
    string cmd; request >> cmd;

    if (cmd == "NewGraph")
    {
        int vxnum, ednum;
        char delimiter;
        request >> vxnum >> delimiter >> ednum;
        graph = Graph(vxnum);
        EditGraph(0, ednum, true, request);
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
        request >> cmd;

        try{ response << MSTPipeLine(cmd); }
        catch (const exception& e) { response << e.what(); }
    }
    else
    {
        response << "Command not recognized";
    }

    response << endl;
    return response;
}


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