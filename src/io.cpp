#include "io.h"
#include "simulation_state.h"
#include "grid.h"
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <string>
#include <sstream>
#include <iostream>
#include "trains.h"
#include "switches.h"
#ifdef _WIN32
#include <direct.h>
#define MAKE_DIR(d) _mkdir(d)
#else
#include <sys/stat.h>
#define MAKE_DIR(d) mkdir(d,0777)
#endif
using namespace std;
static ofstream traceFile;
static ofstream switchFile;
static ofstream signalFile;
// ============================================================================
// IO.CPP - Level I/O and logging
// ============================================================================

// ----------------------------------------------------------------------------
// LOAD LEVEL FILE
// ----------------------------------------------------------------------------
// Load a .lvl file into global state.
// ----------------------------------------------------------------------------
// Remove a trailing carriage return (level files saved on Windows) and spaces.
static void trimEnd(string &t) {
while(!t.empty() && (t[t.size()-1]=='\r' || t[t.size()-1]==' ' || t[t.size()-1]=='\t'))
t.erase(t.size()-1);
}

bool loadLevelFile(const char* fileName) {
ifstream file_to_read(fileName);
if(!file_to_read.is_open()) {
cout<<"Cannot open level file: "<<fileName<<endl;
return 0;
}
while(file_to_read>>word) {
if(word=="NAME:") {
file_to_read>>ws;
getline(file_to_read,name);
trimEnd(name);
}
else if(word=="ROWS:") {
file_to_read>>rows;
}
else if(word=="COLS:") {
file_to_read>>columns;
}
else if(word=="SEED:") {
file_to_read>>seed;
}
else if(word=="WEATHER:") {
file_to_read>>ws;
getline(file_to_read,weather);
trimEnd(weather);
for(size_t c=0; c<weather.size(); c++)
weather[c]=toupper(weather[c]);
}
else if(word=="MAP:"){
getline(file_to_read, h);
for(int i=0;i<100;i++)
{
streampos pos=file_to_read.tellg();
string line;
if(!getline(file_to_read,line))
break;
if(!line.empty() && line[line.size()-1]=='\r')
line.erase(line.size()-1);
if(line.empty())
break;
// the map also ends if the next section starts without a blank line
if(line.compare(0,9,"SWITCHES:")==0 || line.compare(0,7,"TRAINS:")==0) {
file_to_read.seekg(pos);
break;
}
for(size_t j=0; j<line.size() && j<100; j++)
grid[i][j]=line[j];
}
}
else if(word=="SWITCHES:") {
getline(file_to_read,h);
while(true) {
streampos pos=file_to_read.tellg();
string line;
if(!getline(file_to_read,line))
break;
trimEnd(line);
if(line.empty())
break;
if(line.compare(0,7,"TRAINS:")==0) {
file_to_read.seekg(pos);
break;
}
istringstream ss(line);
char letter;
string mode,l0,l1;
int v[5];
ss>>letter>>mode;
for(int j=0; j<5 ;j++)
ss>>v[j];
ss>>l0>>l1;
if(!ss || letter<'A' || letter>'Z' || (mode!="PER_DIR" && mode!="GLOBAL") || (v[0]!=0 && v[0]!=1)) {
cout<<"Bad switch line: "<<line<<endl;
return 0;
}
// switches are stored by their letter (A = 0 ... Z = 25)
int I=letter-'A';
first_letter[I]=letter;
dir[I]=mode;
for(int j=0; j<5 ;j++)
numbers[I][j]=v[j];
straight[I]=l0;
turn[I]=l1;
if(!switch_exists[I])
t_switch++;
switch_exists[I]=true;
}
}
else if(word=="TRAINS:") {
int a[5];
t_train=0;
while(t_train<30 && file_to_read>>a[0]>>a[1]>>a[2]>>a[3]>>a[4]) {
if(a[0]<0 || a[1]<0 || a[1]>=100 || a[2]<0 || a[2]>=100 || a[3]<0 || a[3]>3) {
cout<<"Bad train line: "<<a[0]<<" "<<a[1]<<" "<<a[2]<<" "<<a[3]<<" "<<a[4]<<endl;
return 0;
}
for(int j=0; j<5 ;j++)
trains[t_train][j]=a[j];
t_train++;
}
file_to_read.clear();
}
}
if(rows<=0 || rows>100 || columns<=0 || columns>100) {
cout<<"ROWS and COLS must be between 1 and 100"<<endl;
return 0;
}
return 1;
}


// ----------------------------------------------------------------------------
// INITIALIZE LOG FILES
// ----------------------------------------------------------------------------
void initializeLogFiles() {
MAKE_DIR("out");
traceFile.open("out/trace.csv");
switchFile.open("out/switches.csv");
signalFile.open("out/signals.csv");
if(traceFile.is_open()) traceFile << "Tick,TrainID,X,Y,Direction,State\n";
if(switchFile.is_open()) switchFile << "Tick,Switch,Mode,State\n";
if(signalFile.is_open()) signalFile << "Tick,Switch,Signal\n";
}

// ----------------------------------------------------------------------------
// LOG TRAIN TRACE
// ----------------------------------------------------------------------------
// One line per active train each tick (MOVING or WAITING). A train that was
// delivered or crashed gets one last line (DELIVERED or CRASHED).
// ----------------------------------------------------------------------------
void logTrainTrace() {
if(traceFile.is_open()) {
for(int i=0; i<t_train; i++) {
if(trainActive[i]) {
traceFile << tick << "," << i << "," 
<< active_coordinates[i][0] << "," 
<< active_coordinates[i][1] << "," 
<< directionName(active_direc[i]) << "," << (held[i] ? "WAITING" : "MOVING") << "\n";
}
else if(final_state[i]!=0 && !final_logged[i]) {
traceFile << tick << "," << i << ","
<< final_pos[i][0] << ","
<< final_pos[i][1] << ","
<< directionName(active_direc[i]) << "," << (final_state[i]==1 ? "DELIVERED" : "CRASHED") << "\n";
final_logged[i]=true;
}
}
}
}

// ----------------------------------------------------------------------------
// LOG SWITCH STATE
// ----------------------------------------------------------------------------
void logSwitchState() {
if(switchFile.is_open()) {
for(int i=0; i<26; i++) {
if(switch_exists[i]) {
switchFile << tick << "," << (char)('A'+i) << "," 
<< dir[i] << "," << (numbers[i][0]==0 ? straight[i] : turn[i]) << "\n";
}
}
}
}

// ----------------------------------------------------------------------------
// LOG SIGNAL STATE
// ----------------------------------------------------------------------------
// Writes the displayed colour (one tick late in FOG).
// ----------------------------------------------------------------------------
void logSignalState() {
if(signalFile.is_open()) {
for(int i=0; i<26 ;i++) {
if(!switch_exists[i])
continue;
signalFile << tick << "," << (char)('A'+i) << "," << signalName(signalShown[i]) << "\n";
}
}
}

// ----------------------------------------------------------------------------
// WRITE FINAL METRICS
// ----------------------------------------------------------------------------
void writeMetrics() {
if(traceFile.is_open()) traceFile.close();
if(switchFile.is_open()) switchFile.close();
if(signalFile.is_open()) signalFile.close();

ofstream metricsFile("out/metrics.txt");
if(metricsFile.is_open()) {
metricsFile << "Simulation Metrics\n";
metricsFile << "------------------\n";
metricsFile << "Level: " << name << "\n";
metricsFile << "Weather: " << weather << "\n";
metricsFile << "Total Trains: " << t_train << "\n";
metricsFile << "Total Ticks: " << tick << "\n";
metricsFile << "Delivered: " << reach << "\n";
metricsFile << "Crashed: " << crash << "\n";
if(tick>0)
metricsFile << "Throughput (delivered per 100 ticks): " << (reach*100.0)/tick << "\n";
if(t_train>0)
metricsFile << "Average Wait (idle ticks per train): " << (double)waitTotal/t_train << "\n";
metricsFile << "Signal Violations: " << signalViolations << "\n";
metricsFile << "Switch Flips: " << switchFlips << "\n";
metricsFile << "Safety Buffers: " << bufferCount << "\n";
metricsFile << "Energy Used (active train ticks): " << energyUsed << "\n";
if(bufferCount>0)
metricsFile << "Energy Efficiency (trains x ticks / buffers): " << ((double)t_train*tick)/bufferCount << "\n";
else
metricsFile << "Energy Efficiency (trains x ticks / buffers): not defined, no safety buffers\n";
metricsFile.close();
}
}
