// Shortest Path (Dijkstra) Demo
//
// Perry Kivolowitz
// Assistant Professor, Computer Science
// Carthage College

#include <iostream>
#include <set>
#include <fstream>
#include <istream>
#include <vector>
#include <iomanip>

using namespace std;

// Adapted from:
// http://www.cs.cornell.edu/~wdtseng/icpc/notes/graph_part2.pdf

// This is a legacy of the Cornell code. This adaptation does not
// demand this maximum. The Cornell code did, as it used fixed sized
// arrays. This code uses resizable vectors.
const int max_nodes = 128;

// As currently written, the number of nodes is given as the
// first number in an ASCII file containing the graph. The remainder
// of the file contains a square matrix of this size.
int number_of_nodes;

vector<int> graph;

// This vector memorializes the minimum cost to reach each node
// at the current state of the algorithm. It is updated as the
// algorithm marches through the graph coming to rest only on
// the termination of dijkstra().
vector<int> dist;

// This vector is the "secret sauce" that provides a means of
// reconstructing the shortest paths computed within dijkstra().
// Each time a node is updated with a new lowest cost / shortest
// distance, the preceeding node is stored as well. This records
// how one got to the node being updated with a new best value.
//
// When dijkstra() is complete, a route from the source node
// is derived by starting from the destination node and working
// backwards using this vector.
//
// The vector will be initialized to -1. After dijkstra() completes
// only the source node will retain this value.
//
// The proof of the correctness of Dijkstra's algorithm makes use 
// of the notion of this vector as well as in effect, the only way
// to get to this node with the lowest cost is from that node, 
// otherwise, that node would be the previous_node of this node.
vector<int> previous_node;

// GraphGet() - the original code models graph as a fixed size 2D array.
// In this adaptation I have recast the 2D array as a single dimensional
// vector which can, of course, be of variable size.
//
// This function wraps the 1D vector behind a 2D array like interface.
//
// Parameters:
//	int u	- think of this as an row number.
//	int v	- think of this as a column number.
// Returns:
//	int		- the cost of the edge between u and v or -1 if no edge exists.
int GraphGet(int u, int v)
{
	return graph[number_of_nodes * u + v];
}

// GraphSet() - the original code models graph as a fixed size 2D array.
// In this adaptation I have recast the 2D array as a single dimensional
// vector which can, of course, be of variable size.
//
// This function wraps the 1D vector behind a 2D array like interface.
//
// Parameters:
//	int u	- think of this as an row number.
//	int v	- think of this as a column number.
//	int c	- the new cost to be associated with the vertices
// Returns:
//	none
void GraphSet(int u, int v, int c)
{
	// The original code's dense data structure should be replaced with
	// a sparse data structure. This has been "assigned" as a class thought
	// experiment.
	graph[number_of_nodes * u + v] = c;
}

// GraphSet() - the original code models graph as a fixed size 2D array.
// In this adaptation I have recast the 2D array as a single dimensional
// vector which can, of course, be of variable size.
//
// This version of the function is used when reading in the graph data
// and not during executation of Dijkstra's algorithm.
//
// Parameters:
//	int u	- think of this as an row number.
//	int v	- think of this as a column number.
//	int c	- the new cost to be associated with the vertices
// Returns:
//	none
void GraphSet(int i, int c)
{
	graph[i] = c;
}

// Compares 2 vertices first by distance and then by vertex number
// This code provided with the original Cornell document. The gist
// that vertices are ordered in the set of visited vertices by
// first comparing current best distances held at each of two nodes.
// If the distances are equal, the node number provides the tie
// breaker.
struct ltDist {
	bool operator()(int u, int v) const 
	{
		return make_pair(dist[u], u) < make_pair(dist[v], v);
	}
};

void dijkstra(int s)
{
	// The algorithm is initialized by first setting all nodes' current
	// best cost to infinity so that any cost will be certain to be less
	// (providing the new best).
	//
	// Additionally, every node's sense of which node best leads to it
	// is initialized as -1 indicating it doesn't know how we got to it.
	// At the termination of this function, all entries in previous_node
	// will be set to a value other than -1 except for the source node.
	for (int i = 0; i < number_of_nodes; i++)
	{
		dist[i] = INT_MAX;
		previous_node[i] = -1;
	}

	// Having given all nodes a current best cost of infinity, reset 
	// the cost of the source node to zero indicating it costs nothing
	// to get to itself.
	dist[s] = 0;
	
	// Finally, add the source vertex / node to a set containing the
	// collection of nodes currently under consideration.
	set<int, ltDist> q;
	q.insert(s);

	// This completes the initialization of the algorithm.

	while (!q.empty())
	{
		// q is an ordered set where the order is determined by increasing
		// current best cost. The first entry of this non-empty set is the
		// node under consideration which has the lowest current best cost.
		int u = *q.begin();
		q.erase(q.begin());

		for (int v = 0; v < number_of_nodes; v++)
		{
			// This if statement is passed only if there is an edge between
			// nodes u and v. 
			if (GraphGet(u, v) != -1)
			{
				// Given that there is an edge between u and v, calculate a
				// speculative best cost by adding the current best cost to 
				// the current node (u) to the current best distance from u
				// to v. If this speculative cost is superior to the existing
				// best cost, update dist with the speculative value.
				int newDist = dist[u] + GraphGet(u, v);
				if (newDist < dist[v])
				{
					// Use of set in the Cornell code necessesitates this
					// code for updating the set of nodes under consideration.
					// The value in set cannot be updated directly as distance
					// is used in determining it's place within the set. As
					// distance is what it being changed, the node must be removed
					// if present. Then the value of dist is changed and the node
					// readded - likely landing in a different spot in the set.
					if (q.count(v))
					{
						q.erase(v);
					}

					dist[v] = newDist;

					// I added this to enable the reconstruction of routes not
					// just the shortest path computation as the original Cornell
					// code does.
					previous_node[v] = u;

					// Finally, the node is added back to the set as described above.
					q.insert(v);
				}
			}
		}
	}
}

int main(int argc, char * argv[])
{
	if (argc > 1)
	{
		ifstream in(argv[1]);
		int v;

		if (in.is_open())
		{
			cout << "Opened: " << argv[1] << " for reading." << endl;
			in >> number_of_nodes;
			cout << "Number of nodes: " << number_of_nodes << endl;
			// Modest sanity checking of the first value found in the graph file.
			if (number_of_nodes > 0 && number_of_nodes < max_nodes)
			{
				dist.resize(number_of_nodes);
				graph.resize(number_of_nodes * number_of_nodes);
				previous_node.resize(number_of_nodes);

				for (int i = 0; i < number_of_nodes * number_of_nodes; i++)
				{
					if (in.eof())
					{
						cerr << "The graph file is not well formed. An eof was reached too early." << endl;
						cerr << "Executation will continue with bogus data purely for entertainment value." << endl;
						break;
					}
					in >> v;
					GraphSet(i, v);
				}
				in.close();
				cout << "Connectivity table read." << endl;

				int src;
				cout << "Enter source node number [0 to " << number_of_nodes - 1 << "]: ";
				cin >> src;

				if (src < 0 || src >= number_of_nodes)
				{
					cerr << "Using 0 as source node." << endl;
					src = 0;
				}

				dijkstra(src);
				int w = 8;
				cout << right << setw(3 * w) << "Cum." << right << setw(w) << "Prev" << endl;
				cout << right << setw(w) << "From:";
				cout << right << setw(w) << "To:";
				cout << right << setw(w) << "Cost:";
				cout << right << setw(w) << "Node:" << endl;
				for (int i = 0; i < number_of_nodes; i++)
				{
					cout << right << setw(w) << src;
					cout << right << setw(w) << i;
					cout << right << setw(w) << dist[i];
					cout << right << setw(w) << previous_node[i];
					cout << ((previous_node[i] == -1) ? " >>-->" : "") << endl;
				}
			}
		}
	}
	system("pause");
}