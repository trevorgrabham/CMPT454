#include <vector>
#include <set>
#include <iostream>

using namespace std;

class Graph {
    unsigned size;
    vector<vector<unsigned>> adjacencyList;
    void reachableFrom(unsigned source, unsigned numSteps, set<unsigned>* reachable);
public:
    Graph(unsigned n);
    void addEdge(unsigned s, unsigned d);
    void removeEdge(unsigned s, unsigned d);
    void removeEdgesTo(unsigned d);
    vector<unsigned>* hasCycle();
    void displayGraph();
};

void Graph::displayGraph(){
    for(unsigned i=0;i<this->size;i++){
        for(unsigned d: this->adjacencyList[i]){
            cout << i+1 << " --> " << d+1 << endl;
        }
    }
}

Graph::Graph(unsigned n){
    this->adjacencyList = vector<vector<unsigned>> (n, vector<unsigned>());
    this->size = n;
}

void Graph::addEdge(unsigned s, unsigned d) {
    this->adjacencyList[s-1].push_back(d-1);
}


void Graph::removeEdgesTo(unsigned d){
    for(unsigned i=0;i<this->size;i++){
        for(unsigned j=0;j<this->adjacencyList[i].size();j++){
            if(this->adjacencyList[i][j] == d-1){
                this->adjacencyList[i].erase(this->adjacencyList[i].begin()+j);
            }
        }
    }
}

void Graph::reachableFrom(unsigned source, unsigned numSteps, set<unsigned>* reachable){
    if( numSteps >= this->size) {
        return;
    }
    for(auto neighbour: this->adjacencyList[source]){
        reachable->insert(neighbour);
        this->reachableFrom(neighbour, numSteps+1, reachable);
    }
}

vector<unsigned>* Graph::hasCycle(){
    vector<unsigned>* res = new vector<unsigned>();
    vector<set<unsigned>> reachable = vector<set<unsigned>>(this->size, set<unsigned>());
    for(unsigned i = 0;i<this->size;i++){       // for each vertex
        this->reachableFrom(i, 0, &reachable[i]);
    }
    for(unsigned i = 0;i<this->size;i++){       // for each vertex
        if(reachable[i].count(i) != 0){
            res->push_back(i+1);
        }
    }
    if(res->size() <= 0){
        return NULL;
    } else {
        return res;
    }
}

int main(){
    Graph g(6);
    g.addEdge(1,2);
    g.addEdge(2,3);
    g.addEdge(3,1);
    g.addEdge(3,2);
    g.addEdge(4,3);
    g.addEdge(5,6);
    g.addEdge(6,5);
    g.addEdge(2,1);
    g.addEdge(2,2);
    g.addEdge(4,2);
    g.addEdge(5,2);
    g.addEdge(6,2);
    g.displayGraph();
    cout << endl << endl << endl;
    g.removeEdgesTo(2);
    g.removeEdgesTo(10);
    g.displayGraph();
    vector<unsigned>* cyclicVertices = g.hasCycle();
    cout << "Cyclic vertices include: ";
    for(auto v: *cyclicVertices){
        cout << v << " ";
    }
    cout << endl;
}