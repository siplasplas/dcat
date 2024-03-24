#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "DirectoryReader.h"
#include "DBWriter.h"
#include "DBReader.h"

using namespace std;

class Stopwatch {
public:
  chrono::time_point<chrono::high_resolution_clock> a, b;
  void start() { a = chrono::high_resolution_clock::now(); }
  void stop() { b = chrono::high_resolution_clock::now(); }
  double duration() {
      chrono::duration<double> elapsed_seconds = b - a;
      return elapsed_seconds.count();
  }
};

void readMount() {
    FILE *fp;
    char path[1035];

    fp = popen("mount", "r");
    if (fp == nullptr) {
        printf("Error while running mount\n");
        exit(1);
    }
    while (fgets(path, sizeof(path)-1, fp) != nullptr) {
        if (strstr(path, "/dev/sdb1") != nullptr) {
            printf("found partition: %s", path);
        }
    }
    pclose(fp);
}

int main() {
    DBWriter dbWriter;
    DirectoryReader dirReader(dbWriter);
    Stopwatch st;
    st.start();
    dirReader.readAndStore("/home/andrzej");
    //dbWriter.rebuild();
    st.stop();
    printf("\nduration = %f\n", st.duration());
    //DBReader dbReader;
}
