#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

#include "ThreadPool.h"

#define DEBUG

#ifdef DEBUG
#include <iostream>
#include <ctime>
#include <iomanip>
#include <chrono>
std::time_t t;
std::mutex printMtx;
#endif

/* Global variables, Look at their usage in main() */
size_t image_height;
size_t image_width;
int image_maxShades;
int inputImage[1000][1000];
int outputImage[1000][1000];
size_t num_threads;
size_t chunkSize;
size_t maxChunk;

void dispatch_threads() {
  const int maskX[3][3] = { { -1, 0, 1 },{ -2, 0, 2 },{ -1, 0, 1 } };
  const int maskY[3][3] = { { 1, 2, 1 },{ 0, 0, 0 },{ -1, -2, -1 } };

  XNRW::ThreadPool tp(num_threads);

  for (size_t i = 0; i < maxChunk; i++) {
    tp.addTask([maskX, maskY, i](size_t start, size_t end) {

#ifdef DEBUG
      {
        std::lock_guard<std::mutex> l(printMtx);
        t = time(nullptr);
        std::cout << "[" << std::put_time(std::localtime(&t), "%T") << "]" << ": Thread " << std::this_thread::get_id() << ": Processing chunk #" << i + 1 << ", range: [" << start << ", " << end << ")" << std::endl;
      }
#endif

      for (size_t x = start; x < end; x++) {
        for (size_t y = 0; y < image_width; y++) {
          int sum = 0, sumX = 0, sumY = 0;

          if (x != 0 and x != image_height - 1 and y != 0 and y != image_width - 1) {
            for (int m = -1; m <= 1; m++) {
              for (int n = -1; n <= 1; n++) {
                sumX += inputImage[x + m][y + n] * maskX[m + 1][n + 1];
                sumY += inputImage[x + m][y + n] * maskY[m + 1][n + 1];
              }
            }

            sum = abs(sumX) + abs(sumY);
          }

          if (sum < 0)
            sum = 0;

          if (sum > 255)
            sum = 255;

          outputImage[x][y] = sum;
        }
      }
    }, i * chunkSize, (i + 1) * chunkSize > image_height ?
      image_height : (i + 1) * chunkSize);
  }

  tp.wait();
}

/* ****************Need not to change the function below ***************** */

int main(int argc, char* argv[]) {
  if (argc != 5) {
    std::cout << "ERROR: Incorrect number of arguments. Format is: <Input image filename> <Output image filename> <Threads#> <Chunk size>" << std::endl;
    return 0;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cout << "ERROR: Could not open file " << argv[1] << std::endl;
    return 0;
  }
  num_threads = std::atoi(argv[3]);
  chunkSize = std::atoi(argv[4]);

  /* ******Reading image into 2-D array below******** */

  std::string workString;
  /* Remove comments '#' and check image format */
  while (std::getline(file, workString)) {
    if (workString.at(0) != '#') {
      if (workString.at(1) != '2') {
        std::cout << "Input image is not a valid PGM image" << std::endl;
        return 0;
      }
      else {
        break;
      }
    }
    else {
      continue;
    }
  }
  /* Check image size */
  while (std::getline(file, workString)) {
    if (workString.at(0) != '#') {
      std::stringstream stream(workString);
      int n;
      stream >> n;
      image_width = n;
      stream >> n;
      image_height = n;
      break;
    }
    else {
      continue;
    }
  }

  /* Check image max shades */
  while (std::getline(file, workString)) {
    if (workString.at(0) != '#') {
      std::stringstream stream(workString);
      stream >> image_maxShades;
      break;
    }
    else {
      continue;
    }
  }
  /* Fill input image matrix */
  int pixel_val;
  for (size_t i = 0; i < image_height; i++) {
    if (std::getline(file, workString) && workString.at(0) != '#') {
      std::stringstream stream(workString);
      for (size_t j = 0; j < image_width; j++) {
        if (!stream)
          break;
        stream >> pixel_val;
        inputImage[i][j] = pixel_val;
      }
    }
    else {
      continue;
    }
  }

  /* maxChunk is total number of chunks to process */
  maxChunk = ceil((float)image_height / chunkSize);

  std::cout << "Detect edges in " << argv[1] << " using " << num_threads << " threads" << std::endl;

#ifdef DEBUG
  std::cout << maxChunk << " chunks of size " << chunkSize << "\n==========================\n" << std::endl;
#endif

  /************ Function that creates threads and manage dynamic allocation of chunks *********/

#ifdef DEBUG
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
#endif

  dispatch_threads();

#ifdef DEBUG
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  {
    std::lock_guard<std::mutex> l(printMtx);
    std::cout << "\n==========================\nElapsed time: " << elapsed_seconds.count() << "s" << std::endl;
  }
#endif

  /* ********Start writing output to your file************ */
  std::ofstream ofile(argv[2]);
  if (ofile.is_open()) {
    ofile << "P2" << "\n" << image_width << " " << image_height << "\n" << image_maxShades << "\n";
    for (size_t i = 0; i < image_height; i++) {
      for (size_t j = 0; j < image_width; j++) {
        ofile << outputImage[i][j] << " ";
      }
      ofile << "\n";
    }
  }
  else {
    std::cout << "ERROR: Could not open output file " << argv[2] << std::endl;
    return 0;
  }
  return 0;
}
