#include<filesystem>
#include<iostream>
#include<list>
#include<queue>
#include<string>

namespace fs = std::filesystem;
using namespace std;

auto printErr = [](const char* msg)
  { cout << "[x] " << msg << endl; };

class Node {
  list<fs::path> links;
  
  public:
    fs::path value;
    bool is_visited;
    Node(fs::path p){
      value = p;
      is_visited = false;
    }

    fs::path getLink(){
      return links.front();
    }
    
    void loadLinks(){
      if(is_directory(value))
        for (const auto & link : fs::directory_iterator(value))
          links.push_back(link);
      }
};

typedef Node FsObj;

int main(int argc, char** argv){

  const char* entryPoint = getenv("HOMEPATH");
  fs::path(entrypoint);
  //if(!entrypoint){
    //printErr("HOMEPATH not found...");
  //}


  FsObj curr = FsObj(entrypoint);
  queue<FsObj> q; q.push(entrypoint);
  
  while(!q.empty())
    // ecxtract node from the queeu the first
    if(curr.is_visited) continue;
    curr.is_visited = true;
    for(auto & link : in curr.links)
       q.push(link);
    q.pop(); //delete element from the queue ! 

  }

}
