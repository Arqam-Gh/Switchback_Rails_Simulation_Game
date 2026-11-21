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
switchFlips++;
cout<<"Switch "<<(char)('A'+ i)<<" Flipped."<<endl;
}}
}
// ----------------------------------------------------------------------------
// APPLY DEFERRED FLIPS
// ----------------------------------------------------------------------------


// Signal rules from the manual, for the switch group a train is on or about to enter:
//  GREEN  - the tile ahead along the planned route is free
//  YELLOW - another train is within two tiles ahead
//  RED    - the tile ahead is blocked or occupied, or the train must wait because
//           of a collision this tick
// The colour of a group is the worst colour of its trains. A group with no train is GREEN.
void updateSignalLights() {
int dx[4]={0,1,0,-1};
int dy[4]={-1,0,1,0};
for(int i=0; i<26 ;i++) {
signalsOld[i]=signals[i];
signals[i]=0;
}
for(int t=0; t<t_train ;t++) {
if(!trainActive[t])
continue;
int x=active_coordinates[t][0];
int y=active_coordinates[t][1];
int d=next_direc[t];
int nx=x+dx[d];
int ny=y+dy[d];
int I=-1;
int a1x=0;
int a1y=0;
if(isSwitchTile(y,x)) {
I=grid[y][x]-'A';
a1x=nx;
a1y=ny;
}
else if(isInBounds(ny,nx) && isSwitchTile(ny,nx)) {
I=grid[ny][nx]-'A';
a1x=nx+dx[d];
a1y=ny+dy[d];
}
if(I<0 || !switch_exists[I])
continue;
// the destination tile of the train itself is not a signal
int sx=(isSwitchTile(y,x)) ? x : nx;
int sy=(isSwitchTile(y,x)) ? y : ny;
if(sx==fdest[t][0] && sy==fdest[t][1])
continue;
int a2x=a1x+dx[d];
int a2y=a1y+dy[d];
int sig=0;
if(blocked[t] || !isInBounds(a1y,a1x) || !isTrackTile(a1y,a1x))
sig=2;
for(int u=0; u<t_train ;u++) {
if(u==t || !trainActive[u])
continue;
int ux=active_coordinates[u][0];
int uy=active_coordinates[u][1];
bool stationary=(next_coor[u][0]==ux && next_coor[u][1]==uy);
if(ux==a1x && uy==a1y) {
if(stationary)
sig=2;
else if(sig<1)
sig=1;
}
else if(ux==a2x && uy==a2y && sig<1) {
sig=1;
}
}
if(sig>signals[I])
signals[I]=sig;
}
}
// ----------------------------------------------------------------------------
// UPDATE SIGNAL LIGHTS
// ----------------------------------------------------------------------------

// In FOG the displayed colour is the one from the previous tick. The real
// colours (and the train logic) are not delayed.
void updateSignalDisplay() {
for(int i=0; i<26 ;i++) {
if(weather=="FOG")
signalShown[i]=signalsOld[i];
else
signalShown[i]=signals[i];
}
}

const char* signalName(int s) {
if(s==2) return "RED";
if(s==1) return "YELLOW";
return "GREEN";
}


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