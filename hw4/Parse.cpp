#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include <algorithm>

using namespace std;

const string inputFile = "input.txt";
ifstream inStream;

string charsToString(char* chars){
    string s = chars;
    return s;
}

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

    // start to remove the tokens
    string transactionDelim = ":";
    string resourceDelim = ")";
    string token;
    size_t index = 0;
    vector<string> transactionTokens = vector<string>();
    vector<string> resourceTokens = vector<string>();
    vector<string> actionTokens = vector<string>();
    while((index = input.find(transactionDelim)) != string::npos){
        token = input.substr(0,index);
        transactionTokens.push_back(string(token));
        input.erase(0,index+transactionDelim.length());
        if(input[0] == 'A' || input[0] == 'C'){
            token = input.substr(0,1);
            token.append("(\n)");
            resourceTokens.push_back(string(token));
            input.erase(0,1);
            continue;
        }
        index = input.find(resourceDelim);
        if(index == string::npos){
            cout << "Invalid input format";
            return 1;
        }
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



    cout << "transactionTokens\n";
    for(auto token: transactionTokens){
        cout << token << endl;
    }
    cout << "\nactionTokens\n";
    for(auto token: actionTokens){
        cout << token << endl;
    }
    cout << "\nresourceTokens\n";
    for(auto token: resourceTokens){
        cout << token << endl;
    }

    return 0;
}


