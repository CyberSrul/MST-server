#include "protocol.hpp"
#include "mst.hpp"
#include <sstream>



/*
    Global variables for an MST server.
    A weighted graph, a MST and statistics:
    min, max & avg distances in the tree.
    As well as total weight.
*/
Graph graph, tree;
float longest, shortes, avg, total;
bool updated = true;




void EditGraph(int vxnum, int ednum, bool add, istringstream& input)
{
    int src, dst;
    char delimiter;
    float weight;

    for (int itr = 0; itr < vxnum; itr++)
    {
        input >> src;
        add ? graph.addNode(src) : graph.removeNode(src);
    }
    for (int itr = 0; itr < ednum; itr++)
    {
        input >> src >> delimiter >> dst;

        if (add)
        {
            input >> delimiter >> weight;
            graph.connect(src, dst, weight);
        }
        else
        {
            graph.disconnect(src, dst);
            if (tree.connected(src, dst))
                updated = false;
        }
    }

    if (vxnum) updated = false;
}


void handle_request(const string& msg)
{
    stringstream response;
    istringstream request(msg);
    string cmd; request >> cmd;


    if (cmd == "NewGraph")
    {
        int vxnum, ednum;
        char delimeter;
        request >> vxnum >> delimeter >> ednum;
        EditGraph(vxnum, ednum, true, request);
        response << "Graph built";
    }
    if (cmd == "AddNode")
    {
        EditGraph(1, 0, true, request);
        response << "Node added";
    }
    if (cmd == "RemoveNode")
    {
        EditGraph(1, 0, false, request);
        response << "Node removed";
    }
    if (cmd == "Connect")
    {
        EditGraph(0, 1, true, request);
        response << "Vertecies connected";
    }
    if (cmd == "DisConnect")
    {
        EditGraph(0, 1, false, request);
        response << "Vertecies disconnected";
    }
    if (cmd == "MST")
    {
        if (! updated)
        {
            request >> cmd;

            try { tree = MST(cmd, graph); }
            catch(const std::exception& e) { response << e.what(); }

            updated = true;
        }

        response << "Total weight: " << total   << endl;
        response << "Min distance: " << shortes << endl;
        response << "Max distance: " << longest << endl;
        response << "Avg distance: " << avg;
    }

    response << endl;
}