#include <iostream>

#include "mesh.h"
#include "qem.h"
#include <time.h>

// http://en.wikipedia.org/wiki/ANSI_escape_code
// http://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
const std::string
    boldblacktty("\033[1;30m");              // tell tty to switch to bold black
const std::string lightredtty("\033[1;31m"); // tell tty to switch to bold red
const std::string
    lightgreentty("\033[1;32m"); // tell tty to switch to bright green
const std::string redtty("\033[0;31m");
const std::string greentty("\033[0;32m");
const std::string bluetty("\033[34m"); // tell tty to switch to blue
const std::string lightbluetty("\033[1;34m");
const std::string lightcyantty("\033[1;36m");
const std::string cyantty("\033[0;36m");
const std::string purpletty("\033[0;35m");
const std::string lightpurpletty("\033[1;35m");
const std::string orangetty("\033[0;33m");
const std::string yellowtty("\033[1;33m");
const std::string
    magentatty("\033[1;35m"); // tell tty to switch to bright magenta
const std::string
    yellowbgtty("\033[1;43m"); // tell tty to switch to bright yellow background
const std::string underlinetty("\033[4m"); // tell tty to switch to underline
const std::string deftty("\033[0m"); // tell tty to switch back to default color

// source:
// http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
timespec diff(timespec start, timespec end) {
  timespec temp;
  if ((end.tv_nsec - start.tv_nsec) < 0) {
    temp.tv_sec = end.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
  }
  return temp;
}

long long int getMilliseconds(timespec t) {
  return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

long long int getNanoseconds(timespec t) {
  return (1000000000 * t.tv_sec) + t.tv_nsec;
}

int main(int argc, char **argv) {
  if (argc < 5) {
    std::cerr << std::endl
              << "Usage:  ./mesh-simplification <input file> <simplification "
                 "fraction> <no of blocks> <no of threads>\n"
              << std::endl;
    exit(1);
  }

  char *inputFile = argv[1];
  float simplificationFraction = atof(argv[2]);
  int noOfBlocks = atoi(argv[3]);
  int noOfThreads = atoi(argv[4]);

  if (simplificationFraction >= 1.0f) {
    std::cerr << std::endl
              << "Error:  Simplification fraction should be less than 1.0.\n"
              << std::endl;
    exit(2);
  }

  std::cout << std::endl;
  std::cout << "Input File              : " << inputFile << std::endl;
  std::cout << "Simplification Fraction : " << simplificationFraction
            << std::endl;
  std::cout << "Number Of Blocks        : " << noOfBlocks << std::endl;
  std::cout << "Number Of Threads       : " << noOfThreads << std::endl;

  timespec t0, t1, t;
  clock_gettime(CLOCK_REALTIME, &t0);
  Mesh *mesh = new Mesh(inputFile);
  QuadricErrorMetrics::simplify(mesh, simplificationFraction, noOfBlocks,
                                noOfThreads);
  clock_gettime(CLOCK_REALTIME, &t1);
  t = diff(t0, t1);
  std::cout << lightgreentty << "TOTAL TIME: " << getMilliseconds(t) << " ms"
            << deftty << std::endl;
  mesh->saveAsOFF("tmp.off");

  return 0;
}