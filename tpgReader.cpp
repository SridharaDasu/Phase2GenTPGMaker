#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {

  // Ignore till the bell
  cin.ignore(0xFFFFFFFF, '\a');

  char s[1024];
  while(cin.getline(s, 1024)) {
    cout << s << endl;
  }

}
