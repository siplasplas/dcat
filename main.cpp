#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <string>
#include <linux/limits.h>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

class Stopwatch
{
public:
  chrono::time_point<chrono::high_resolution_clock> a, b;
  void start() { a = chrono::high_resolution_clock::now(); }
  void stop() { b = chrono::high_resolution_clock::now(); }
  double duration()
  {
      chrono::duration<double> elapsed_seconds = b - a;
      return elapsed_seconds.count();
  }
};

struct DirContent {
  vector<string> dirs;
  vector<string> files;
};

DirContent readDir(const string& dirPath) {
    DIR *dir;
    struct dirent *ent;
    struct stat file_stat{};
    DirContent dirContent;

    if ((dir = opendir(dirPath.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                if (lstat((dirPath+"/"+ent->d_name).c_str(), &file_stat) == 0) {
                    if (S_ISDIR(file_stat.st_mode))
                        dirContent.dirs.emplace_back(ent->d_name);
                    else
                        dirContent.files.emplace_back(ent->d_name);
                }
            }
        }
        closedir(dir);
    } else {
        perror("");
        return {};
    }
    sort(dirContent.dirs.begin(), dirContent.dirs.end());
    return dirContent;
}

void readDirRecur(const string& dirPath) {
    auto content = readDir(dirPath);
    for (auto &dname: content.dirs)
        readDirRecur(dirPath+"/"+dname);
}



int main() {
    Stopwatch st;
    st.start();
    readDirRecur("/media/andrzej/Crucial X8/wazne/github");
    st.stop();
    printf("\nduration = %f\n", st.duration());
}
