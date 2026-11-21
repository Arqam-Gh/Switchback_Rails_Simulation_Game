#include "simulation_state.h"
#include <string>
using namespace std;
// ============================================================================
// SIMULATION_STATE.CPP - Global state definitions
// ============================================================================

// ----------------------------------------------------------------------------
// GRID
// ----------------------------------------------------------------------------
char grid[100][100];
string h;
// ----------------------------------------------------------------------------
// TRAINS
// ----------------------------------------------------------------------------
int rows,columns,seed,trains[30][5],Rows;
string name,weather;
bool trainActive[30];
int tick;
int active_coordinates[30][2];
int t_train;
int active_direc[30];
int active_colour[30];
int prev_coordinates[30][2];
int halt_timer[27];
int halt_end[27];
int delay_t[30];
int rain_moves[30];
bool held[30];
bool blocked[30];
int final_state[30];
bool final_logged[30];
int final_pos[30][2];
int signalsOld[27];
int signalShown[27];
int waitTotal;
int switchFlips;
int signalViolations;
int bufferCount;
int energyUsed;
int crash;
int reach;
// ----------------------------------------------------------------------------
// SWITCHES
// ----------------------------------------------------------------------------
int P_X;
int P_Y;
int C_X;
int C_Y;
char first_letter[27];
string dir[27];
int numbers[27][5];
string straight[27];
string turn[27];
int t_switch;
string word;
string temp;
bool switch_exists[27];
int c_numbers[27][6];
bool switchFlipPending[27];
int signals[27];
// ----------------------------------------------------------------------------
// SPAWN AND DESTINATION POINTS
// ----------------------------------------------------------------------------
int pdist,dist;
int fdest[30][2];
int next_coor[30][2];
int next_direc[30];
int pre_coordinates[30][2];
int cdist[30];
int atc;
// ----------------------------------------------------------------------------
// SIMULATION PARAMETERS
// ----------------------------------------------------------------------------
bool conflict;
// ----------------------------------------------------------------------------
// METRICS
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EMERGENCY HALT
// ----------------------------------------------------------------------------

// ============================================================================
// INITIALIZE SIMULATION STATE
// ============================================================================
// ----------------------------------------------------------------------------
// Resets all global simulation state.
// ----------------------------------------------------------------------------
// Called before loading a new level.
// ----------------------------------------------------------------------------
void initializeSimulationState() {
for(int a=0;a<100;a++)
for(int b=0;b<100;b++)
grid[a][b]=' ';
for(int a=0;a<30;a++) {
for(int b=0;b<2;b++) {
active_coordinates[a][b]=0;
prev_coordinates[a][b]=0;
pre_coordinates[a][b]=0;
next_coor[a][b]=0;
fdest[a][b]=0;
}
for(int b=0;b<5;b++)
trains[a][b]=0;
trainActive[a]=false;
active_direc[a]=0;
active_colour[a]=0;
next_direc[a]=0;
cdist[a]=0;
delay_t[a]=0;
rain_moves[a]=0;
held[a]=false;
blocked[a]=false;
final_state[a]=0;
final_logged[a]=false;
final_pos[a][0]=0;
final_pos[a][1]=0;
}
for(int a=0;a<27;a++) {
for(int b=0;b<5;b++)
numbers[a][b]=0;
for(int b=0;b<6;b++)
c_numbers[a][b]=0;
first_letter[a]=0;
dir[a]="";
straight[a]="";
turn[a]="";
switch_exists[a]=false;
switchFlipPending[a]=false;
signals[a]=0;
signalsOld[a]=0;
signalShown[a]=0;
halt_timer[a]=0;
halt_end[a]=-10;
}
pdist=10000;
dist=0;
P_X=0;
P_Y=0;
C_X=0;
C_Y=0;
t_switch=0;
word="";
temp="";
rows=0;
columns=0;
seed=0;
Rows=0;
name="";
weather="";
tick=0;
t_train=0;
waitTotal=0;
switchFlips=0;
signalViolations=0;
bufferCount=0;
energyUsed=0;
crash=0;
reach=0;
conflict=false;
h="";
atc=0;
}
