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
  cout << entryPoint << endl;
  FsObj curr = FsObj(entryPoint);
  
  cout << "created a filesystem object" << endl;
  
  queue<FsObj> q; q.push(curr);
  
  cout << "Pushed !" << endl ; 
  while(!q.empty()){
    cout << "the size of the queue " << q.size() << endl;
    curr = q.front();
    cout << "extracted " << curr.value << endl;
    if(curr.is_visited) continue;
    
    curr.is_visited = true;
    curr.loadLinks();
    for(auto & link : curr.links)
       q.push(link);
    q.pop();
  }
}
