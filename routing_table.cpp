#include "as.h"
using namespace std;

//class RoutingItem {
//  int as_name; // Instead of using IP prefixes, we use AS name to indicate AS
//  unsigned char * path; //Format: "1 2 3 4"
//  int priority;
//};
void RoutingTable::addRoute(int as_name, int path_length,
unsigned char * path, int priority=0) {
  if (findRoute(as_name, path_length, path) == NULL) {
    RoutingItem item;
    item.as_name = as_name;
    item.path_length = path_length;
    item.path = path;
    item.priority = priority;
    item.next_hop = path[path_length - 1];

    RoutingTable::items[RoutingTable::size] = item;
    RoutingTable::size++;
  }
}

int RoutingTable::removeRoute(int destination, int path_length, unsigned char * path) {
  int deleted = 0;
  int index = indexOfRoute(destination, path_length, path);
  if (index != -1) {
    for(int i=index; i < RoutingTable::size; i++)
      RoutingTable::items[i] = RoutingTable::items[i+1];
    RoutingItem empty;
    RoutingTable::items[RoutingTable::size - 1] = empty;
    RoutingTable::size -= 1;
    deleted += 1;
  }
  return deleted;
}

int RoutingTable::removeRoute(int destination) {
  int deleted = 0;
  for(int i=0; i < RoutingTable::size; i++) {
    RoutingItem current = items[i];
    current.print();
    if (RoutingTable::containNode(current, destination) == 1){
      for(int j=i; j < RoutingTable::size; j++)
        RoutingTable::items[j] = RoutingTable::items[j+1];
      RoutingItem empty;
      RoutingTable::items[RoutingTable::size - 1] = empty;
      RoutingTable::size -= 1;
      deleted += 1;
      i -= 1; // because all remaining items are shifted to the left by 1
    }
  }
  return deleted;
}

int RoutingTable::containNode(RoutingItem item, int as_name) {
  for(int i = 0; i < item.path_length; i++) {
    if (as_name == (int)item.path[i]) {
      return 1;
    }
  }
  return 0;
}

int RoutingTable::indexOfRoute(int as_name,
int path_length, unsigned char * path) {
  for(int i=0; i < RoutingTable::size; i++) {
    RoutingItem current = items[i];
    if (as_name == current.as_name && current.path_length == path_length) {
      int flag = 0;
      for(int i = 0; i < path_length; i++) {
        if (path[i] != current.path[i]) {
          flag = 1;
          break;
        }
      }
      if (flag == 0) return i;
    }
  }
  return -1;
}

RoutingItem * RoutingTable::findRoute(int as_name,
int path_length, unsigned char * path) {
  for(int i=0; i < RoutingTable::size; i++) {
    RoutingItem current = items[i];
    if (as_name == current.as_name && current.path_length == path_length) {
      int flag = 0;
      for(int i = 0; i < path_length; i++) {
        if (path[i] != current.path[i]) {
          flag = 1;
          break;
        }
      }
      if (flag == 0) return &items[i];
    }
  }
  return NULL;
}

void RoutingTable::setRoutePriority(int as_name,
unsigned char * path, int priority) {
}
//RoutingItem * RoutingTable::getRoutes(int as_name){
//}
//RoutingItem * RoutingTable::getRouteByPath(int as_name,
//unsigned char * path){
//}
void RoutingTable::print_table() {
  cout << "=========Routing table content=========" << endl;
  for(int i=0; i < RoutingTable::size; i++) {
    RoutingItem item = items[i];
    cout << item.as_name << " : "; 
    for(int i = 0; i < item.path_length; i++) {
      cout << (int)item.path[i] << " ";
    }
    cout << ": " << item.priority << endl;
  }
}
