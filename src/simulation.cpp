#include "simulation.h"
#include "simulation_state.h"
#include "trains.h"
#include "switches.h"
#include "io.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;


// ============================================================================
// SIMULATION.CPP - Implementation of main simulation logic
// ============================================================================

// ----------------------------------------------------------------------------
// INITIALIZE SIMULATION
// ----------------------------------------------------------------------------

void initializeSimulation() {
tick=0;
bufferCount=0;
for(int y=0; y<rows ;y++) {
for(int x=0; x<columns ;x++) {
if(grid[y][x]=='=')
bufferCount++;
}
}
initializeLogFiles();
cout<<"\nSimulation Started\n";
cout<<"Level: "<<name<<" | Weather: "<<weather<<"\n";
}

// ----------------------------------------------------------------------------
// SIMULATE ONE TICK
// ----------------------------------------------------------------------------
void printGrid(){
cout<<"\n========================================\n";
cout<<"Tick: "<<tick;
cout<<"\n========================================\n";
for(int y=0 ;y<rows ;y++){
for(int x=0;x<columns;x++){
bool trainHere=false;
for(int i=0; i<t_train ;i++){
if(trainActive[i]==true && active_coordinates[i][0]==x && active_coordinates[i][1]==y){
cout<<(char)('0'+(i%10));
trainHere=true;
break;
}
}
if(!trainHere){
cout<<grid[y][x];
}
}
cout<<"\n";
}
cout<<"\nActive Trains: ";
int activeCount=0;
for(int i=0; i<t_train ;i++){
if(trainActive[i]==true){
cout<<i<<" ";
activeCount++;
}
}
if(activeCount==0){
cout<<"None";
}
cout<<"\n";
for(int i=0; i<t_train ;i++){
if(trainActive[i]==true){
cout<<"Train "<<i<<" at ("<<active_coordinates[i][0]<<","<<active_coordinates[i][1]<<") moving "<<directionName(active_direc[i]);
if(held[i])
cout<<" (waiting)";
cout<<"\n";
}
}
for(int i=0; i<26 ;i++){
if(switch_exists[i]){
cout<<"Switch "<<(char)('A'+i)<<": "<<(numbers[i][0]==0 ? straight[i] : turn[i])<<" | signal "<<signalName(signalShown[i])<<"\n";
}
}
cout<<"Delivered: "<<reach<<"/"<<t_train;
cout<<" | Crashed: "<<crash<<"\n";
}

void simulateOneTick() {
tick++;
cout<<"\n=== TICK "<<tick<<" ===\n";
spawnTrainsForTick();
determineAllRoutes();
updateSwitchCounters();
queueSwitchFlips();
detectCollisions();
updateSignalLights();
moveAllTrains();
applyDeferredFlips();
checkArrivals();
updateEmergencyHalt();
updateSignalDisplay();
logTrainTrace();
logSwitchState();
logSignalState();
printGrid();

}

// ----------------------------------------------------------------------------
// CHECK IF SIMULATION IS COMPLETE
// ----------------------------------------------------------------------------

bool isSimulationComplete() {
atc=1;
for(int i=0; i<t_train ;i++) {
if(active_coordinates[i][0] != -1) {
atc=0;
}
}
if(atc==1) return 1;
else return 0;
}
