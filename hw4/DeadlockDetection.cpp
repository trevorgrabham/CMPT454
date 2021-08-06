#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <chrono>
#include <ctime>

using namespace std;

enum LockType {shared, exclusive, none};
enum ActionChar {R, W, X, S, C, A};

class LockRequest {
    Transaction* requestor;
    LockType lockType;
};

class Transaction {
    unsigned id;
    unsigned numberOfLocks;         // doubles as the length of lockedResources
    Resource** lockedResources;     // null if we do not hold any locks currently
    long double timeStarted;        
};

class Resource {
    string resourceName;
    unsigned numLocks;       // 0 if not locked, also used as length of owner
    Transaction** owner;     // null if not locked
    LockType lockType;      
    queue<LockRequest*> requestQ;
};

class Action {
    Transaction* transaction;
    Resource* resource;
    ActionChar actionChar;
};


// Globals
map<string, Resource*> resources;
map<unsigned, Transaction*> transactions; 
queue<Action*> actions;
// End globals


// read in the text file as one long string
// split on white space
// for each Transaction
//     split on the colon
//     for the first part, remove the T and read in the transaction number
//     create a transaction with the transaction number, starting at 0 seconds until we start the processing
//     add the transaction to the global list
//     for the second part, read the first letter, and store it as the action
//     remove the brackets from the second part
//     store the remaining second part as the resource name
//     create and add the resource to the global list
// we should now have a list of transactions, in order, so we store them in a queue
// for each item in the queue
//     if the transaction time = 0 (lookup using map.find)
//         start the timer
//     if action != X or S
//         continue
//     if action = S
//         lookup action->resource->resourceName in resource table
//         if the resource doesn't have an exclusive lock
//             add action->transaction to the action->resource->
//         else
