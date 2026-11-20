#include "trains.h"
#include "simulation_state.h"
#include "grid.h"
#include "switches.h"
#include <cstdlib>
#include<iostream>
using namespace std;
// ============================================================================
// TRAINS.CPP - Train logic
// ============================================================================

// Storage for planned moves (for collisions).

// Previous positions (to detect switch entry).

// ----------------------------------------------------------------------------
// SPAWN TRAINS FOR CURRENT TICK
// ----------------------------------------------------------------------------
// Activate trains scheduled for this tick.
// ----------------------------------------------------------------------------
void spawnTrainsForTick() {
    for(int k=0;k<t_train;k++){
        if(!trainActive[k] && active_coordinates[k][0]!=-1 && tick>=trains[k][0]){
            cout<<"+++";
            trainActive[k]=1;
            active_coordinates[k][0]=trains[k][1];
            active_coordinates[k][1]=trains[k][2];
            active_direc[k]=trains[k][3];
            active_colour[k]=trains[k][4];
            prev_coordinates[k][0]=trains[k][1];
            prev_coordinates[k][1]=trains[k][2];
            pdist=10000;
            for(int i=0;i<100;i++)
            {
            for(int j=0;j<100;j++)
            {
            if(grid[i][j]=='D'){  
            dist = abs(active_coordinates[k][0] - j) + abs(active_coordinates[k][1] - i);
            if(pdist>dist) {
            pdist=dist;
            fdest[k][0]=j;
            fdest[k][1]=i;
            }
            }
            }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// DETERMINE NEXT POSITION for a train
// ----------------------------------------------------------------------------
// Compute next position/direction from current tile and rules.
// ----------------------------------------------------------------------------
bool determineNextPosition(int k){
if(next_direc[k]==0){
next_coor[k][0]=active_coordinates[k][0];
next_coor[k][1]=active_coordinates[k][1]-1;
}
if(next_direc[k]==1){
next_coor[k][0]=active_coordinates[k][0]+1;
next_coor[k][1]=active_coordinates[k][1];
}
if(next_direc[k]==2){
next_coor[k][0]=active_coordinates[k][0];
next_coor[k][1]=active_coordinates[k][1]+1;
}
if(next_direc[k]==3){
next_coor[k][0]=active_coordinates[k][0]-1;
next_coor[k][1]=active_coordinates[k][1];
}
return 1;
}

// ----------------------------------------------------------------------------
// GET NEXT DIRECTION based on current tile and direction
// ----------------------------------------------------------------------------
// Return new direction after entering the tile.
// ----------------------------------------------------------------------------
int getNextDirection(int k) {
next_direc[k]=active_direc[k];
if(grid[active_coordinates[k][1]][active_coordinates[k][0]]=='-'||grid[active_coordinates[k][1]][active_coordinates[k][0]]=='|'){
next_direc[k]=active_direc[k];
}
if(grid[active_coordinates[k][1]][active_coordinates[k][0]]=='\\'){
if(active_direc[k]== 0) next_direc[k] = 3;
else if(active_direc[k]==1) next_direc[k] = 2;
else if(active_direc[k]==2) next_direc[k] = 1;
else if(active_direc[k]==3) next_direc[k] = 0;
}
if(grid[active_coordinates[k][1]][active_coordinates[k][0]]=='/'){
if(active_direc[k]== 0) {
next_direc[k] = 1;
}
else if(active_direc[k]==1) {
next_direc[k] = 0;
}
else if(active_direc[k]==2) {
next_direc[k] = 3;
}
else if(active_direc[k]==3) {
next_direc[k] = 2;
}
}
if(grid[active_coordinates[k][1]][active_coordinates[k][0]]=='+'){
next_direc[k]=active_direc[k];
}
if(grid[active_coordinates[k][1]][active_coordinates[k][0]]>=65 && grid[active_coordinates[k][1]][active_coordinates[k][0]]<=90){
int I=grid[active_coordinates[k][1]][active_coordinates[k][0]]-'A';
if(numbers[I][0]==0){
next_direc[k]=active_direc[k];}
if(numbers[I][0]==1){
if(active_direc[k]== 0) {
next_direc[k] = 1;
}
else if(active_direc[k]==1) {
next_direc[k] = 2;
}
else if(active_direc[k]==2) {
next_direc[k] = 1;
}
else if(active_direc[k]==3) {
next_direc[k] = 2;
}
}
}
return 1;
}
// ----------------------------------------------------------------------------
// SMART ROUTING AT CROSSING - Route train to its matched destination
// ----------------------------------------------------------------------------
// Choose best direction at '+' toward destination.
// ----------------------------------------------------------------------------
int getSmartDirectionAtCrossing(int trainIdx) {
return active_direc[trainIdx];
}

// ----------------------------------------------------------------------------
// DETERMINE ALL ROUTES (PHASE 2)
// ----------------------------------------------------------------------------
// Fill next positions/directions for all trains.
// ----------------------------------------------------------------------------
void determineAllRoutes() {
//it tooks the active train coordinate , and see on that train's coordinate if it is - or | it do nothing if / or \ it turns 90 degree the train checks switch state and wrt state lets the train go
for(int k=0;k<t_train;k++){
        if(trainActive[k]){
            getNextDirection(k);
            determineNextPosition(k);
        }}
}
// ----------------------------------------------------------------------------
// MOVE ALL TRAINS (PHASE 5)
// ----------------------------------------------------------------------------
// Move trains; resolve collisions and apply effects.
// ----------------------------------------------------------------------------
void moveAllTrains() {
if (halt_s){
return; 
}
for(int i=0; i<t_train; i++) {
if(!trainActive[i])
continue;
if(!isInBounds(next_coor[i][1],next_coor[i][0]) || !isTrackTile(next_coor[i][1],next_coor[i][0])) {
trainActive[i]=false;
active_coordinates[i][0]=-1;
active_coordinates[i][1]=-1;
crash++;
continue;
}
prev_coordinates[i][0]=active_coordinates[i][0];
prev_coordinates[i][1]=active_coordinates[i][1];
active_coordinates[i][0]=next_coor[i][0];
active_coordinates[i][1]=next_coor[i][1];
active_direc[i]=next_direc[i];
}
}

// ----------------------------------------------------------------------------
// DETECT COLLISIONS WITH PRIORITY SYSTEM
// ----------------------------------------------------------------------------
// Resolve same-tile, swap, and crossing conflicts.
// ----------------------------------------------------------------------------
void detectCollisions() {
for(int i=0;i<30;i++){
    if(trainActive[i]){
        for(int j=i+1;j<30;j++){
                cdist[i]=abs(active_coordinates[i][0] - fdest[i][0]) + abs(active_coordinates[i][1] - fdest[i][1]);
                cdist[j]=abs(active_coordinates[j][0] - fdest[j][0]) + abs(active_coordinates[j][1] - fdest[j][1]);
                    if(trainActive[j]){
                        if(next_coor[i][0]==next_coor[j][0] && next_coor[i][1]==next_coor[j][1]){
                            conflict=1;
                                if(cdist[i]>cdist[j]){
                                    next_coor[j][0]=active_coordinates[j][0];
                                    next_coor[j][1]=active_coordinates[j][1];
                                }
                                else if(cdist[j]>cdist[i]){
                                    next_coor[i][0]=active_coordinates[i][0];
                                    next_coor[i][1]=active_coordinates[i][1];

                                }
                                else {trainActive[j]=0;
                                      trainActive[i]=0;
                                      active_coordinates[i][0]=-1;
                                      active_coordinates[i][1]=-1;
                                      active_coordinates[j][0]=-1;
                                      active_coordinates[j][1]=-1;
                                      crash+=2;
                                      break;
                             }
                        }
                if(next_coor[i][0]==active_coordinates[j][0] && next_coor[i][1]==active_coordinates[j][1] && next_coor[j][0]==active_coordinates[i][0] && next_coor[j][1]==active_coordinates[i][1] ){
                    conflict=1;
                    if(cdist[i]>cdist[j]){
                        next_coor[j][0]=active_coordinates[j][0];
                        next_coor[j][1]=active_coordinates[j][1];
                    }
                    else if(cdist[j]>cdist[i]){
                        next_coor[i][0]=active_coordinates[i][0];
                        next_coor[i][1]=active_coordinates[i][1];
                    }
                    else {trainActive[j]=0;
                          trainActive[i]=0;
                          active_coordinates[i][0]=-1;
                          active_coordinates[i][1]=-1;
                          active_coordinates[j][0]=-1;
                          active_coordinates[j][1]=-1;
                          crash+=2;
                          break;}
                }
            }
        }
    }
}
}
// ----------------------------------------------------------------------------
// CHECK ARRIVALS
// ----------------------------------------------------------------------------
// Mark trains that reached destinations.
// ----------------------------------------------------------------------------
void checkArrivals() {
for(int i=0; i<t_train; i++) {
if(!trainActive[i])
continue;
int x=active_coordinates[i][0];
int y=active_coordinates[i][1];
if(grid[y][x]=='D') {
if(x==fdest[i][0] && y==fdest[i][1]) {
trainActive[i]=false;
active_coordinates[i][0]=-1;
active_coordinates[i][1]=-1;
reach++;
}
}
}
}
// ----------------------------------------------------------------------------
// APPLY EMERGENCY HALT
// ----------------------------------------------------------------------------
// Apply halt to trains in the active zone.
// ----------------------------------------------------------------------------
void applyEmergencyHalt() {
if(!halt_s) {
halt_s=true;
halt_t=3;
}
}

// ----------------------------------------------------------------------------
// UPDATE EMERGENCY HALT
// ----------------------------------------------------------------------------
// Decrement timer and disable when done.
// ----------------------------------------------------------------------------
void updateEmergencyHalt() {
if(halt_s) {
if(halt_t>0) {
halt_t--;
} else {
halt_s=false;
}
}
}