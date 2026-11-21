#ifndef SIMULATION_STATE_H
#define SIMULATION_STATE_H
#include <string>
using namespace std;
// ============================================================================
// SIMULATION_STATE.H - Global constants and state
// ============================================================================
// Global constants and arrays used by the game.
// ============================================================================

// ----------------------------------------------------------------------------
// GRID CONSTANTS
// ----------------------------------------------------------------------------
extern char grid[100][100];
extern string h;
extern char ttt;
// ----------------------------------------------------------------------------
// TRAIN CONSTANTS
// ----------------------------------------------------------------------------
extern int rows,columns,seed,trains[30][5],Rows;

// ----------------------------------------------------------------------------
// SWITCH CONSTANTS
// ----------------------------------------------------------------------------
extern int P_X;
extern int P_Y;
extern int C_X;
extern int C_Y;
extern char first_letter[27];
extern string dir[27];
extern int numbers[27][5];
extern string straight[27];
extern string turn[27];
extern int t_switch;
extern string word;
extern string temp;
extern bool switch_exists[27];
extern int c_numbers[27][6];
extern bool switchFlipPending[27];
extern int signals[27];
// ----------------------------------------------------------------------------
// WEATHER CONSTANTS
// ----------------------------------------------------------------------------
extern string name,weather;

// ----------------------------------------------------------------------------
// SIGNAL CONSTANTS
// ----------------------------------------------------------------------------




// ----------------------------------------------------------------------------
// GLOBAL STATE: GRID
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// GLOBAL STATE: TRAINS
// ----------------------------------------------------------------------------
extern bool trainActive[30];
extern int t_train;
extern int active_coordinates[30][2];
extern int prev_coordinates[30][2];
extern int active_direc[30];
extern int active_colour[30];
extern bool halt_s;
extern int halt_t;
extern int halt_end_tick;
extern int crash;
extern int reach;
extern int atc;
// ----------------------------------------------------------------------------
// GLOBAL STATE: SWITCHES (A-Z mapped to 0-25)
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// GLOBAL STATE: SPAWN POINTS
// ----------------------------------------------------------------------------
extern int pre_coordinates[30][2];

// ----------------------------------------------------------------------------
// GLOBAL STATE: DESTINATION POINTS
// ----------------------------------------------------------------------------
extern int pdist,dist;
extern int fdest[30][2];
extern int next_coor[30][2];
extern int next_direc[30];
// ----------------------------------------------------------------------------
// GLOBAL STATE: SIMULATION PARAMETERS
// ----------------------------------------------------------------------------
extern bool conflict;
extern int cdist[30];
// ----------------------------------------------------------------------------
// GLOBAL STATE: METRICS
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// GLOBAL STATE: EMERGENCY HALT
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// INITIALIZATION FUNCTION
// ----------------------------------------------------------------------------
// Resets all state before loading a new level.
extern int tick;
void initializeSimulationState();

#endif