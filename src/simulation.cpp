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
initializeLogFiles();
cout<<"\nSimulation Started\n";
cout<<"Level: "<<name<<" | Weather: "<<weather<<"\n";
cout<<"Press SPACE to start\n";
}

// ----------------------------------------------------------------------------
// SIMULATE ONE TICK
// ----------------------------------------------------------------------------
void printGrid(){
cout<<"\n========================================\n";
cout<<"Tick "<<tick;
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
//if(trainActive[y]=1){
//ttt=grid[active_coordinates[y][0]][active_coordinates[y][1]];
//grid[active_coordinates[y][0]][active_coordinates[y][1]]='>';
//}
if(!trainHere){
cout<<grid[y][x];
//grid[next_coor[y][0]][next_coor[y][1]]=ttt;
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
moveAllTrains();
applyDeferredFlips();
checkArrivals();
updateSignalLights();
updateEmergencyHalt();
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
