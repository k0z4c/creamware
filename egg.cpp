#include<filesystem>
#include<iostream>
#include<vector>
#include<string>

auto printErr = [](const char* msg)
  { cout << "[x] " << msg << endl; }

namespace fs = std::filesystem;
using namespace std;

typedef Node N;

class Node {
  fs::path value;
  bool is_visited;
  vector<fs::path> links;

  public:
    Node(fs::path p){
      value = p;
      is_visited = false;
      links = // populate this stuff...maybe on heap ? need to sleep... cya 
    }

    fs::path getLink(){
      return links.pop_first();
    }
}

int main(int argc, char[] argv){

  const char* entryPoint = getenv("HOMEPATH");

  fs::path(entrypoint);
  if(!entrypoint){
    printErr("HOMEPATH not found...");
  }

  fs::path(curr);
  vector<fs::path> q; q.push_back(entrypoint);
  for (const auto & entry : fs::directory_iterator(path)){
    curr = q.pop_first();
    if(curr.is_visited) continue;

    curr.is_visited = true;
    if(curr.is_directory())
      for (const auto & currEntry : fs::directory_iterator(curr))
        q.push_back(currEntry); 

  }

}
