#include "concore.hpp"
//instead of below, this file will be C++ equivalent of cpymat.py
#include <vector>
#include <string>
#include <chrono>
#include <iomanip> //setprecision

using namespace std;

struct ElapsedRecord {
    long long readElapsed;
    long long writeElapsed;
};

int main()
{
  Concore concore;
  concore.delay = 0.01;
  int Nsim = 100;
  string init_simtime_u = "[0.0,0.0,0.0]";
  string init_simtime_ym = "[0.0,0.0,0.0]";
  int minElapsed = 10000000;
  int maxElapsed = 0;
  int sumElapsed  = 0;

  vector<double> u = concore.initval(init_simtime_u);
  auto wallclock1 = chrono::high_resolution_clock::now();

  vector<ElapsedRecord> elapsedRecords;

  vector<double> ym;
  while(concore.simtime<Nsim){
    while (concore.unchanged()){
      ym = concore.read(1,"ym",init_simtime_ym);
    }

    auto wallclock3 = chrono::high_resolution_clock::now();
    auto duration_read = chrono::duration_cast<chrono::nanoseconds>(wallclock3 - wallclock1);

    u[0]  = ym[0]+1;

    cout<<"ym="<<ym[0]<<" u="<<u[0]<<endl;
    concore.write(1,"u",u);

    auto wallclock2 = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(wallclock2 - wallclock1);
    auto duration_write = chrono::duration_cast<chrono::nanoseconds>(wallclock2 - wallclock3);
    int elapsed = duration.count();
    sumElapsed += elapsed;
    wallclock1 = wallclock2;
    minElapsed = min(minElapsed, elapsed);
    maxElapsed = max(maxElapsed, elapsed);
    elapsedRecords.push_back({duration_read.count(), duration_write.count()});
  }

  concore.write(1,"u",init_simtime_u);
  cout<<"retry="<<concore.retrycount<<endl;

  cout<<"min="<<minElapsed<<endl;
  cout<<"avg="<<(sumElapsed/Nsim)<<endl;
  cout<<"max="<<maxElapsed<<endl;

  ofstream outputFile("elapsed_times_cz.csv");
  if (outputFile.is_open()) {
      outputFile << "Read Elapsed,Write Elapsed\n"; // CSV header
      for (const auto &record : elapsedRecords) {
          outputFile << record.readElapsed << "," << record.writeElapsed << "\n";
      }
      outputFile.close();
      cout << "Recorded data dumped into 'elapsed_times.csv'" << endl;
  } else {
      cerr << "Unable to open file for writing." << endl;
  }

}

