#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

enum LockType {shared, exclusive};

class Lock {
    LockType type;
    Transaction* owner;
};

class Transaction {
    unsigned id;
    unsigned numberOfLocks;
    unsigned timeStarted;
};

class Resource {
    LockType* locks;

}

// read in the text file as one long string
// split on white space
// for each Transaction
//     split on the colon
//     for the first part, remove the T and read in the transaction number
//     search our 
