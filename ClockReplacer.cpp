#include <iostream>
#include <fstream>
#include <string>
#include <queue>

using namespace std;

ifstream ifile;
ofstream ofile;
string ifileName = "input.txt";
string ofileName = "output.txt";
int* pinCount;
int nFrames;
char action;
int bufferNum;
int candidate;
int current = 0;


int Candidate() {
    while(pinCount[current] > 0){
        current = ++current % nFrames;
    }
    return current+1;
}

void Pin(int n){
    pinCount[n-1]++;
    return;
}

void Unpin(int n){
    pinCount[n-1]--;
    return;
}


int main(){
    // open files
    ifile.open(ifileName);
    ofile.open(ofileName);
    // read in number of frames
    ifile >> nFrames;
    // initiate the pin counts
    pinCount = new int [nFrames];
    for(int i=0;i<nFrames;i++){
        pinCount[i] = 1;
    }    
    // while input file != eof, read in and execute actions
    while(!ifile.eof()){
        ifile >> action;
        if(action == 'R'){
            toBeReplaced = Candidate();
            ofile << toBeReplaced << " ";
            pinCount[toBeReplaced] = 1;
        } else if(action == 'U'){
            ifile >> bufferNum;
            Unpin(bufferNum);
        } else if(action == 'P'){
            ifile >> bufferNum;
            Pin(bufferNum);
        }
    }
    return 0;
}