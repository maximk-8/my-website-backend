#include "Graph.h"
#include <fstream>
#include <sstream>
#include <map>
#include <tuple>
#include <queue>
#include <algorithm>
#include <utility>
#include <climits>
#include <iostream>

unsigned int numNodes = 0;
struct Node {
    string name;
    map<string, double> neighbors;
};
map<string, Node*> Nodes;

Graph::Graph(const char* const & edgelist_csv_fn) {
    ifstream my_file(edgelist_csv_fn);
    string line;
    while(getline(my_file, line)) {
        istringstream ss(line);
        string first, second, third;
        Node* n;
        getline(ss, first, ',');
        getline(ss, second, ',');
        getline(ss, third, '\n');

        if (Nodes.count(first) == 0) {
            n = new Node {
                .name = first,
                .neighbors = map<string, double>()
            };
            numNodes++;
            if (n->neighbors.count(second) == 0) {
                n->neighbors.insert({second, stod(third)});
            }
            Nodes.insert({n->name, n});
            if (Nodes.count(second) == 0) {
                Node* n2 = new Node {
                    .name = second,
                    .neighbors = map<string, double>()
                };
                numNodes++;
                if (n2->neighbors.count(first) == 0) {
                    n2->neighbors.insert({first, stod(third)});
                }
                Nodes.insert({n2->name, n2});
            }
            else {
                Node* n2 = Nodes[second];
                if (n2->neighbors.count(first) == 0) {
                    n2->neighbors.insert({first, stod(third)});
                }
            }
        }
        else {
            n = Nodes[first];
            if (n->neighbors.count(second) == 0) {
                n->neighbors.insert({second, stod(third)});
            }
            if (Nodes.count(second) == 0) {
                Node* n2 = new Node {
                        .name = second,
                        .neighbors = map<string, double>()
                };
                numNodes++;
                if (n2->neighbors.count(first) == 0) {
                    n2->neighbors.insert({first, stod(third)});
                }
                Nodes.insert({n2->name, n2});
            }
            else {
                Node* n2 = Nodes[second];
                if (n2->neighbors.count(first) == 0) {
                    n2->neighbors.insert({first, stod(third)});
                }
            }
        }
    }
    my_file.close();
}

Graph::~Graph() {
    for (auto i : Nodes) {
        delete i.second;
    }
}

vector<string> Graph::shortest_path_unweighted(string const & start_label, string const & end_label) {
    vector<string> path = vector<string>();
    queue<pair<Node*,Node*>> q = queue<pair<Node*,Node*>>();
    map<string, Node*> visited = map<string, Node*>();
    map<Node*, Node*> relationship = map<Node*, Node*>();
    Node* parent = nullptr;
    Node* curr;
    Node* n;
    bool found = false;
    q.push(make_pair(Nodes[start_label], nullptr));

    while (!q.empty()) {
        curr = q.front().first;
        parent = q.front().second;
        q.pop();
        if (curr->name == end_label) {
            found = true;
            visited.insert({curr->name, curr});
            relationship.insert({curr, parent});
            break;
        }
        if (visited.count(curr->name) == 0) {
            visited.insert({curr->name, curr});
            relationship.insert({curr, parent});
            for (auto i : curr->neighbors) {
                if (visited.count(i.first) == 0) {
                    q.push(make_pair(Nodes[i.first], curr));
                }
            }
        }
    }
    if (!found) {
        return path;
    }
    else {
        n = Nodes[end_label];
        while (n->name != start_label) {
            path.push_back(n->name);
            n = Nodes[relationship[n]->name];
        }
        path.push_back(start_label);
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<tuple<string,string,double>> Graph::shortest_path_weighted(string const & start_label, string const & end_label) {
    map<Node*, double> distances = map<Node*, double>();
    for (auto i : Nodes) {
        distances.insert({i.second, INT_MAX});
    }
    distances[Nodes[start_label]] = 0;
    priority_queue<tuple<double, Node*, Node*>, vector<tuple<double, Node*, Node*>>, greater<tuple<double, Node*, Node*>>> pq;
    map<string, Node*> visited = map<string, Node*>();
    map<Node*, Node*> relationship = map<Node*, Node*>();
    Node* parent = nullptr;
    Node* curr;
    Node* n;
    bool found = false;
    double dist;
    pq.push(make_tuple(0, Nodes[start_label], nullptr));

    while (!pq.empty()) {
        dist = get<0>(pq.top());
        curr = get<1>(pq.top());
        parent = get<2>(pq.top());
        pq.pop();
        if (curr->name == end_label) {
            found = true;
            visited.insert({curr->name, curr});
            relationship.insert({curr, parent});
            break;
        }
        if (visited.count(curr->name) == 0) {
            visited.insert({curr->name, curr});
            relationship.insert({curr, parent});
            for (auto i : curr->neighbors) {
                if ((dist + i.second) < distances[Nodes[i.first]]) {
                    distances[Nodes[i.first]] = dist + i.second;
                    pq.push(make_tuple(dist + i.second, Nodes[i.first], curr));
                }
            }
        }
    }
    vector<tuple<string,string,double>> path = vector<tuple<string,string,double>>();
    if (!found) {
        return path;
    }
    else {
        if (start_label == end_label) {
            path.push_back(make_tuple(start_label, end_label, -1));
            return path;
        }
        n = Nodes[end_label];
        while (n->name != start_label) {
            path.push_back(make_tuple(relationship[n]->name, n->name, n->neighbors[relationship[n]->name]));
            n = Nodes[relationship[n]->name];
        }
    }
    reverse(path.begin(), path.end());
    return path;
}

int main (int argc, char* argv[]) {
    Graph g = Graph(argv[1]);
    string term = argv[2];
 
    if (term == "unweighted") {
        vector<string> path = g.shortest_path_unweighted(argv[3], argv[4]);
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i];
            if (i != path.size() - 1) {
                cout << ",";
            }
        }
        cout << endl;
    } else {
        vector<tuple<string,string,double>> path = g.shortest_path_weighted(argv[3], argv[4]);
        for (size_t i = 0; i < path.size(); ++i) {
            cout << get<0>(path[i]);
            if (i != path.size() - 1) {
                cout << ",";
            }
        }
        if (!path.empty()) {
            cout << "," << get<1>(path.back());
        }
        cout << endl;
    }

    return 0;
}

/*
void saveGraph() {
    std::ofstream file("graph.txt");
    for (const auto& pair : Nodes) {
        file << pair.first << " ";
        for (const auto& neighbor : pair.second->neighbors) {
            file << neighbor.first << " " << neighbor.second << " ";
        }
        file << "\n";
    }
}

void loadGraph() {
    std::ifstream file("graph.txt");
    std::string line;
    while (getline(file, line)) {
        std::istringstream ss(line);
        std::string name;
        ss >> name;
        Node* node = new Node(name, std::map<std::string, double>());
        std::string neighbor;
        double weight;
        while (ss >> neighbor >> weight) {
            node->neighbors[neighbor] = weight;
        }
        Nodes[name] = node;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <edgelist_csv_fn> <rebuild>\n";
        return 1;
    }
    if (std::stoi(argv[2]) == 0) {
        Graph graph(argv[1]);
        saveGraph();
    } else {
        loadGraph();
    }
    // Rest of your code...
}*/