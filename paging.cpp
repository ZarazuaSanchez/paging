/*
*   Edgar Zarazua PSID 1264354
*   USAGE:
*   $ g++ -std=c++11 hw3.cpp -o hw3
*   $ ./hw3 file=input.txt
*
*/

#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <queue>
#include <set>
using namespace std;

struct entry {
    string pid;
    string address;
    int frame;
};

string findFileName(string str){

    string filename = str.substr(str.find('=') + 1);
    return filename;
}

void printUpdatePaging(int frames[], int noFrames, int i,int prev){

    cout << "-----------------------------------" << endl;
    cout << "Frame" << setw(10) << "Page" << endl;
    cout << "-----------------------------------" << endl;

    for(int j=0;j<noFrames;j++){

        cout << j << setw(10);
        if(j==i){
            if(frames[i]==-1){
                cout << prev << " --> " << frames[i] << " (New Page) " << endl;
            }
            else{
                cout << prev << " --> " << "0x" << frames[i] << " (New Page) " << endl;
            }
        }
        else{
            if(frames[j]==-1){
                cout << frames[j] << endl;
            }
            else{
                cout << "0x" << frames[j] << endl;
            }
        }
    }

    cout << "+++++++++++++++++++++++++++++++++++" << endl;
}

void printHit(int frames[], int noFrames,int i){

    cout << "-----------------------------------" << endl;

    cout << "Frame" << setw(10) << "Page" << endl;
    cout << "-----------------------------------" << endl;

    for(int j=0;j<noFrames;j++){
        cout << j << setw(10);
        if(j==i){
            cout  << frames[j] << " * (HIT!)" << endl;
        }
        else{
            if(frames[j]==-1){
                cout << frames[j] << endl;
            }
            else{
                cout << "0x" << frames[j] << endl;
            }
        }

    }

    cout << "+++++++++++++++++++++++++++++++++++" << endl;
}

void fifoPaging(ifstream& input, int frames[], int noFrames, map<string, vector<int>> pgTables){

    string pair;
    string address;
    string pid;
    int pgNo;
    queue<int> fifo;
    bool found;
    bool full = false;

    while(getline(input,pair)){

        stringstream ss(pair);
        ss >> pid;
        ss >> address;

        if(address.find("0x")!=string::npos){
            pgNo = stoi(address.substr(2,1), 0, 16);

            cout << endl << "Page Request for " << "(" << pid << "," << address <<  ")" << endl;

            bool hit =false;

            int prev;
            //search for page in frame
            for(int j=0;j<noFrames;j++){
                if(frames[j]==pgNo){
                    hit = true;
                    printHit(frames,noFrames,j);
                    break;
                }
            }

            if(!hit){

                if(fifo.empty()){
                    for(int j=0;j<noFrames;j++){
                        if(frames[j]==-1){
                            prev = frames[j];
                            frames[j] = pgNo;
                            pgTables[pid][pgNo] = j;
                            fifo.push(j);
                            printUpdatePaging(frames, noFrames, j, prev);
                            break;
                        }
                    }
                }
                else{
                    if(!full){

                        for(int j=0;j<noFrames;j++){
                            if(frames[j]==-1){
                                if(j==(noFrames-1)){
                                    full = true;
                                }
                                prev = frames[j];
                                frames[j] = pgNo;
                                pgTables[pid][pgNo] = j;
                                fifo.push(j);
                                printUpdatePaging(frames, noFrames, j, prev);
                                break;
                            }
                        }
                    }
                    else{

                        int fi = fifo.front();
                        fifo.pop();
                        prev = frames[fi];
                        frames[fi] = pgNo;
                        pgTables[pid][pgNo] = fi;
                        fifo.push(fi);
                        printUpdatePaging(frames, noFrames, fi, prev);
                    }
                }
            }

        }
        else if (address=="-1"){
            cout << pid << " TERMINATED" << endl;
        }
    }

    return;
}

int chooseAlgo(){

    cout << "Here!" << endl;

    int whichalgo;
    set<int> algos = {1};

    cout << "Choose which the paging algorithm to use" << endl;

    while(true){
        cout << "1. FIFO" << endl;
        cin >> whichalgo;

        if(algos.find(whichalgo)!=algos.end()){
            return whichalgo;
        } else{
            cout << "Choose which the paging algorithm from those listed" << endl;
        }
    }

}

int main(int argc, char const *argv[]) {

    string rawinput(argv[1]);
    string filename = findFileName(rawinput);
    ifstream input(filename);

    int noFrames; //No. of page frames in main memory
    input >> noFrames;
    int pagesize; //ex. 16 --> 2^16 words
    input >> pagesize;
    int fifoPgFrames;
    input >> fifoPgFrames;
    int lruXLookAhead;
    input >> lruXLookAhead;
    int minPool;
    input >> minPool;
    int maxPool;
    input >> maxPool;
    int totNumProc;
    input >> totNumProc;

    map<string,int> processes; //map<pid, size>

    string pid;
    int psize;
    for(int i=0;i<totNumProc;i++){
        input >> pid;
        input >> psize;
        processes.insert({pid,psize});
    }

    int frames[noFrames]; //main memory

    for(int i=0;i<noFrames;i++){
        frames[i] = -1;
    }

    map<string, vector<int>> pgTables; //pgTable[pid][address] = <frame>

    for(auto it=processes.begin();it!=processes.end();it++){

        pid = it->first;
        psize = processes[pid];
        vector<int> eachPgTable;
        for(int i=0;i<psize;i++){
            eachPgTable.push_back(-1);
        }
        pgTables[pid] = eachPgTable;
    }

    int whichalgo;

    whichalgo = chooseAlgo();

    if(whichalgo==1){
        fifoPaging(input, frames, noFrames, pgTables);
    }

    return 0;

}
