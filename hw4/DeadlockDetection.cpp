#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <chrono>
#include <ctime>
#include <queue>
#include <algorithm>

using namespace std;


// Classes and Enums
// ==============================================================================================================
enum LockType {shared, exclusive, none};

class Transaction;
class Resource;

class LockRequest {
public:
    Transaction* requestor;
    LockType lockType;
    LockRequest(Transaction* requestor, LockType lockType);
};

class Transaction {
public:   
    bool done;
    unsigned id;
    unsigned numberOfLocks;         // doubles as the length of lockedResources
    vector<Resource*> lockedResources;   
    chrono::milliseconds timeStarted; 
    Transaction(unsigned transactionID);    
};

class Resource {
public:
    string resourceName;
    unsigned numLocks;       // 0 if not locked, also used as length of owner
    vector<Transaction*> owner;     // null if not locked
    LockType lockType;      
    queue<LockRequest*> requestQ;
    Resource(string resourceName);
    ~Resource();
};

class Action {
public:
    Transaction* transaction;
    Resource* resource;
    char actionChar;
    Action(Transaction* transation, Resource* resource, char actionChar);
};

class Graph {
    unsigned size;
    vector<vector<unsigned>> adjacencyList;
    void reachableFrom(unsigned source, unsigned numSteps, set<unsigned>* reachable);
public:
    Graph(unsigned n);
    void addEdge(unsigned s, unsigned d);
    void removeEdgesTo(unsigned d);
    vector<unsigned>* hasCycle();
};
// ==============================================================================================================
// End classes and enums




// class methods
// ==============================================================================================================
Transaction::Transaction(unsigned transactionID){
    this->id = transactionID;
    this->lockedResources = vector<Resource*>();
    this->numberOfLocks = 0;
    this->timeStarted = (chrono::milliseconds)0;
    this->done = false;
}

Resource::Resource(string resourceName){
    this->resourceName = resourceName;
    this->lockType = none;
    this->numLocks = 0;
    this->owner = vector<Transaction*>();
    this->requestQ = queue<LockRequest*>();
}

Resource::~Resource(){
    LockRequest* temp;
    while(!this->requestQ.empty()){
        temp = this->requestQ.front();
        this->requestQ.pop();
        delete temp;
    }
}

LockRequest::LockRequest(Transaction* requestor, LockType lockType){
    this->requestor = requestor;
    this->lockType = lockType;
}

Action::Action(Transaction* transaction, Resource* resource, char actionChar){
    this->transaction = transaction;
    this->resource = resource;
    this->actionChar = actionChar;
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
        return res;
    } else {
        return res;
    }
}
// ==============================================================================================================
// End class methods


// Globals
// ==============================================================================================================
map<string, Resource*> resources;
map<unsigned, Transaction*> transactions; 
queue<Action*> actions;
const string inputFile = "input.txt";
// ==============================================================================================================
// End globals


// Function defentions
// ==============================================================================================================
string charsToString(char* chars){
    string s = chars;
    return s;
}

void parseInput(string input){
    string transactionDelim = ":";
    string resourceDelim = ")";
    string token;
    size_t index = 0;
    vector<string> transactionTokens = vector<string>();
    vector<string> resourceTokens = vector<string>();
    vector<string> actionTokens = vector<string>();
    
    while((index = input.find(transactionDelim)) != string::npos){
        // find the transaction token and remove it from the input string
        token = input.substr(0,index);
        transactionTokens.push_back(string(token));
        input.erase(0,index+transactionDelim.length());
        // special case if we have a commit or abort
        if(input[0] == 'A' || input[0] == 'C'){
            token = input.substr(0,1);
            token.append("(\n)");                       // using the newline character as a special character, because we have already removed all of the whitespace, we know that if we see one, then it was on purpose
                                                        // we use this so that we have our actions being built properly
            resourceTokens.push_back(string(token));
            input.erase(0,1);
            continue;
        }
        index = input.find(resourceDelim);
        
        if(index == string::npos){
            cout << "Invalid input format";
            return;
        }
        // find the resource token and remove it from the input string
        token = input.substr(0,index+1);
        resourceTokens.push_back(string(token));
        input.erase(0,index+resourceDelim.length());
    }

    for(unsigned i=0;i<transactionTokens.size();i++){
        transactionTokens[i].erase(0,1);
    }
    for(unsigned i=0;i<resourceTokens.size();i++){
        resourceTokens[i].erase(resourceTokens[i].length()-1,1);
        actionTokens.push_back(resourceTokens[i].substr(0,1));
        resourceTokens[i].erase(0,2);
    }

    for(unsigned i=0;i<resourceTokens.size();i++){
        unsigned transactionId = stoi(transactionTokens[i]);
        if(transactions.count(transactionId) == 0){
            transactions.insert(pair<unsigned, Transaction*>(transactionId, new Transaction(transactionId)));
        }
        if(resourceTokens[i] != "\n" && resources.count(resourceTokens[i]) == 0){
            resources.insert(pair<string, Resource*>(resourceTokens[i], new Resource(resourceTokens[i])));
        }
        if(resourceTokens[i] != "\n"){
            actions.push(new Action(transactions[transactionId], resources[resourceTokens[i]], actionTokens[i][0]));
        } else {
            actions.push(new Action(transactions[transactionId], NULL, actionTokens[i][0]));
        }
    }
}
// ==============================================================================================================
// End function definitions




int main() {
    // input file setup
    ifstream inStream;
    inStream.open(inputFile);
    if(!inStream.is_open()){
        cout << "Unable to open file " << inputFile << endl;
        return 1;
    }
    string buffer;
    string input = "";

    // read in from input file
    while(getline(inStream, buffer)){
        input.append(buffer);
    }
    inStream.close();

    // remove the white space
    input.erase(remove_if(input.begin(), input.end(), [](unsigned char const c){return isspace(c);}), input.end());
    parseInput(input);

    // create the conflict graph
    Graph conflictGraph = Graph(transactions.size());

    // start commencing actions
    Action* action;
    cout << "We have " << actions.size() << " actions" << endl;
    while(!actions.empty()){
        action = actions.front();
        // if this is the first time that we are dealing with this transaction, start the clock on it (so we can use this to break a tie)
        if(action->transaction->timeStarted == (chrono::milliseconds)0){
            action->transaction->timeStarted = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
        }
        cout << "Action char is: " << action->actionChar << endl;
        switch(action->actionChar){
            case 'R':                 
            case 'W':
                // both of these, do nothing
                break;
            case 'C':
            case 'A':
                conflictGraph.removeEdgesTo(action->transaction->id);
                // release all of the locks it had
                for(unsigned i=0;i<action->transaction->numberOfLocks;i++){
                    if(action->transaction->lockedResources[i]->numLocks == 1){
                        action->transaction->lockedResources[i]->owner.clear();
                    } else {
                        unsigned i = 0;
                        while(action->transaction->lockedResources[i]->owner[i] != action->transaction){
                            i++;
                        }
                        action->transaction->lockedResources[i]->owner.erase(action->transaction->lockedResources[i]->owner.begin()+i);
                    }
                    action->transaction->lockedResources[i]->numLocks--;
                    action->transaction->lockedResources[i]->lockType = none;
                }
                // set the transaction to done so that we know to skip it if it pops up in a queue later
                action->transaction->done = true;
                break;
            case 'X':
                if(action->resource->numLocks != 0){
                    for(unsigned i=0;i<action->resource->requestQ.size();i++){
                        LockRequest* temp = action->resource->requestQ.front();
                        action->resource->requestQ.pop();
                        action->resource->requestQ.push(temp);
                        conflictGraph.addEdge(action->transaction->id, temp->requestor->id);
                        cout << "Adding edge: " << action->transaction->id << " --> " << temp->requestor->id << endl;
                    }
                    for(unsigned i=0;i<action->resource->numLocks;i++){
                        conflictGraph.addEdge(action->transaction->id, action->resource->owner[i]->id);
                        cout << "Adding edge: " << action->transaction->id << " --> " << action->resource->owner[i]->id << endl;
                    }
                    action->resource->requestQ.push(new LockRequest(action->transaction, exclusive));
                } else {
                    cout << action->resource->resourceName << " has been locked by " << action->transaction->id << endl;
                    action->resource->numLocks++;
                    action->resource->owner.push_back(action->transaction);
                    action->resource->lockType = exclusive;
                }
                break;
            case 'S':
                if(action->resource->lockType == exclusive){
                    for(unsigned i=0;i<action->resource->requestQ.size();i++){
                        LockRequest* temp = action->resource->requestQ.front();
                        action->resource->requestQ.pop();
                        action->resource->requestQ.push(temp);
                        conflictGraph.addEdge(action->transaction->id, temp->requestor->id);
                        cout << "Adding edge: " << action->transaction->id << " --> " << temp->requestor->id << endl;
                    }
                    action->resource->requestQ.push(new LockRequest(action->transaction, shared));
                    conflictGraph.addEdge(action->transaction->id, action->resource->owner[0]->id);
                    cout << "Adding edge: " << action->transaction->id << " --> " << action->resource->owner[0]->id << endl;
                } else {
                    cout << action->resource->resourceName << " has been locked by " << action->transaction->id << endl;
                    action->resource->numLocks++;
                    action->resource->owner.push_back(action->transaction);
                    action->resource->lockType = shared;
                }
                break;
        }
        actions.pop();
    }
    vector<unsigned>* candidates = conflictGraph.hasCycle();
    chrono::milliseconds minTime;
    if(candidates->size() > 0){
        minTime = transactions[(*candidates)[0]]->timeStarted;
        unsigned minLocks = resources.size();
        Transaction* toAbort;
        for(unsigned i=0;i<candidates->size();i++){
            if(transactions[(*candidates)[0]]->numberOfLocks < minLocks || (transactions[(*candidates)[0]]->numberOfLocks == minLocks && transactions[(*candidates)[0]]->timeStarted < minTime)){
                minLocks = transactions[(*candidates)[0]]->numberOfLocks;
                toAbort = transactions[(*candidates)[0]];
                minTime = transactions[(*candidates)[0]]->timeStarted;
            }
        }
        conflictGraph.removeEdgesTo(toAbort->id);
        // release all of the locks it had
        for(unsigned i=0;i<toAbort->numberOfLocks;i++){
            if(toAbort->lockedResources[i]->numLocks == 1){
                toAbort->lockedResources[i]->owner.clear();
            } else {
                unsigned i = 0;
                while(toAbort->lockedResources[i]->owner[i] != toAbort){
                    i++;
                }
                toAbort->lockedResources[i]->owner.erase(toAbort->lockedResources[i]->owner.begin()+i);
            }
            toAbort->lockedResources[i]->numLocks--;
            toAbort->lockedResources[i]->lockType = none;
        }
        toAbort->done = true;
        cout << "T" << toAbort->id;
    } else {
        cout << "OK";
    }
}