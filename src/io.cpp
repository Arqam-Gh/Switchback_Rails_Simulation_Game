#include "io.h"
#include "simulation_state.h"
#include "grid.h"
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
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
bool loadLevelFile() {
ifstream file_to_read("complex_network.lvl");
// MAP READ
//Switches read
while(file_to_read>>word) {
if(word=="NAME:") {
getline(file_to_read,name);
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
getline(file_to_read,weather);
}
else if(word=="MAP:"){
getline(file_to_read, h);
for(int i=0;i<100;i++)
{
for(int j=0;j<100;j++)
{
    file_to_read.get(grid[i][j]);
    if(grid[i][j]=='\n'){
    break;
    }
}
if(grid[i][0]=='\n'){
break;}
}
for(int a=0;a<100;a++)
for(int b=0;b<100;b++)
if(grid[a][b]=='\n')
grid[a][b]=' ';
}

else if(word=="SWITCHES:") {
getline(file_to_read,h);
while(t_switch<26) {
file_to_read.get(first_letter[t_switch]);
if(first_letter[t_switch]=='\n'){
break;
}
file_to_read>>dir[t_switch];
for(int j=0; j<5 ;j++) {
file_to_read>>numbers[t_switch][j];
}
file_to_read>>straight[t_switch];
file_to_read>>turn[t_switch];
switch_exists[t_switch]=true;
getline(file_to_read,h);
t_switch++;
}}
else if(word=="TRAINS:") {
for(int i=0; i<30 ;i++) {
for(int j=0; j<5 ;j++) {
file_to_read>>trains[i][j];
}
if(trains[i][0]==0 && trains[i][1]==0 && trains[i][2]==0){
t_train=i;
break;
}
}
}
}
return 1;
}


// ----------------------------------------------------------------------------
// LOG TRAIN TRACE
// ----------------------------------------------------------------------------
void logTrainTrace() {
if(traceFile.is_open()) {
for(int i=0; i<t_train; i++) {
if(trainActive[i]) {
traceFile << tick << "," << i << "," 
<< active_coordinates[i][0] << "," 
<< active_coordinates[i][1] << "," 
<< active_direc[i] << ",ACTIVE\n";
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
<< dir[i] << "," << numbers[i][0] << "\n";
}
}
}
}

// ----------------------------------------------------------------------------
// LOG SIGNAL STATE
// ----------------------------------------------------------------------------
void logSignalState() {
if(signalFile.is_open()) {
for(int i=0; i<26 ;i++) {
if(!switch_exists[i])
continue;
string signal="GREEN";
for(int j=0; j<t_train ;j++) {
if(!trainActive[j])
continue;
int tx=active_coordinates[j][0];
int ty=active_coordinates[j][1];
for(int y=0; y<rows ;y++) {
for(int x=0; x<columns ;x++) {
if(grid[y][x] == ('A'+ i)) {
int dist=abs(tx-x)+abs(ty-y);
if(dist <= 1) {
signal="RED";
}
else if(dist <= 3 && signal != "RED") {
signal="YELLOW";
}
}
}
}
}
signalFile << tick << "," << (char)('A'+i) << "," << signal << "\n";
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
metricsFile << "Total Trains: " << t_train << "\n";
metricsFile << "Total Ticks: " << tick << "\n";
metricsFile.close();
}
}
