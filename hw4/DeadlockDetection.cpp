#include <fstream>
#include <iostream>
#include <math.h>
#include <string>

using namespace std;

enum LockType {shared, exclusive};

class Lock {
    string resource;
    LockType type;
};

class Transaction {
    unsigned id;
    Lock* resourceLocks;
    unsigned numberOfLocks;
};

class Resource {
    LockType* locks
}


