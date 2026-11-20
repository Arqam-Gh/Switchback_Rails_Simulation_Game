#include "switches.h"
#include "simulation_state.h"
#include "grid.h"
#include <iostream>
#include <cstdlib>
using namespace std;

// ============================================================================
// SWITCHES.CPP - Switch management
// ============================================================================

void updateSwitchCounters() {
for(int i=0 ; i<t_train ; i++) {
if(!trainActive[i])
continue;
int C_X=active_coordinates[i][0];
int C_Y=active_coordinates[i][1];
int P_X=prev_coordinates[i][0];
int P_Y=prev_coordinates[i][1];
char tile=grid[C_Y][C_X];
bool entered=(C_X!=P_X || C_Y!=P_Y);
bool onSwitch=isSwitchTile(C_Y,C_X);
if(onSwitch && entered) {
int I=getSwitchIndex(tile);
if(I==-1 || !switch_exists[I])
continue;
int Dir=active_direc[i];
if(dir[I] == "GLOBAL") {
c_numbers[I][1]++;
cout<<"Switch "<<(char)('A'+ I)<<" Counter= "<<c_numbers[I][1]<<endl;
}
else {
if(Dir == 0) {
c_numbers[I][1]++;
cout<<"Switch "<<(char)('A'+ I)<<" Up Counter= "<<c_numbers[I][1]<<endl;
}
else if(Dir == 1) {
c_numbers[I][2]++;
cout<<"Switch "<<(char)('A'+ I)<<" Right Counter= "<<c_numbers[I][2]<<endl;
}
else if(Dir == 2) {
c_numbers[I][3]++;
cout<<"Switch "<<(char)('A'+ I)<<" Down Counter= "<<c_numbers[I][3]<<endl;
}
else if(Dir == 3) {
c_numbers[I][4]++;
cout<<"Switch "<<(char)('A'+ I)<<" Left Counter= "<<c_numbers[I][4]<<endl;
}
}}}}
// ----------------------------------------------------------------------------
// UPDATE SWITCH COUNTERS
// ----------------------------------------------------------------------------


void queueSwitchFlips() {
for(int i=0; i<26 ;i++) {
if(!switch_exists[i])
continue;
if(dir[i] == "GLOBAL") {
int count=c_numbers[i][1];
int needed=numbers[i][1];
if(count>=needed) {
switchFlipPending[i]=true;
cout<<"Switch "<<(char)('A'+ i)<<" Time to flip."<<endl;
}}
else {
bool move_up=(c_numbers[i][1] >= numbers[i][1]);
bool move_right=(c_numbers[i][2] >= numbers[i][2]);
bool move_down=(c_numbers[i][3] >= numbers[i][3]);
bool move_left=(c_numbers[i][4] >= numbers[i][4]);
if(move_up || move_right || move_down || move_left) {
switchFlipPending[i]=true;
cout<<"Switch "<<(char)('A'+ i)<<" Time to flip."<<endl;
}}}
}
// ----------------------------------------------------------------------------
// QUEUE SWITCH FLIPS
// ----------------------------------------------------------------------------


void applyDeferredFlips() {
for(int i=0; i<26 ;i++) {
if(!switch_exists[i])
continue;
if(switchFlipPending[i]) {
int prev_s=numbers[i][0];
numbers[i][0]=1-prev_s;
for(int k=0; k<6 ;k++) {
c_numbers[i][k]=0;
}
switchFlipPending[i]=false;
cout<<"Switch "<<(char)('A'+ i)<<" Flipped."<<endl;
}}
}
// ----------------------------------------------------------------------------
// APPLY DEFERRED FLIPS
// ----------------------------------------------------------------------------


void updateSignalLights() {
for(int i=0; i<26 ;i++) {
if(!switch_exists[i])
continue;
int currentSignal=0;
for(int j=0; j<t_train ;j++) {
if(!trainActive[j])
continue;
int trainX=active_coordinates[j][0];
int trainY=active_coordinates[j][1];
for(int y=0; y<rows ;y++) {
for(int x=0; x<columns ;x++) {
if(grid[y][x] == ('A'+ i)) {
int distX=abs(trainX-x);
int distY=abs(trainY-y);
int totalDist=distX+distY;
if(totalDist <= 1) {
currentSignal=2;
}
else if(totalDist <= 3 && currentSignal != 2) {
currentSignal=1;
}}}}}}
// signals[i] = currentSignal; // Uncomment if you add signals array
}
// ----------------------------------------------------------------------------
// UPDATE SIGNAL LIGHTS
// ----------------------------------------------------------------------------


void toggleSwitchState(int I) {
if(I<0 || I>=26)
return;
if(!switch_exists[I])
return;
numbers[I][0]=1-numbers[I][0];
cout<<"Switch "<<(char)('A'+ I)<<" manually toggled."<<endl;
}
// ----------------------------------------------------------------------------
// TOGGLE SWITCH STATE (Manual)
// ----------------------------------------------------------------------------


int getSwitchStateForDirection(int I, int enterDir) {
if(I<0 || I >= 26)
return enterDir;
if(!switch_exists[I])
return enterDir;
int state=numbers[I][0];
if(state == 0)
return enterDir;
else {
return (enterDir+1)%4;
}
}
// ----------------------------------------------------------------------------
// GET SWITCH STATE FOR DIRECTION
// ----------------------------------------------------------------------------