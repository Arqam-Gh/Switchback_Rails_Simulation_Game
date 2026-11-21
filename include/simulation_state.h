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
// Emergency halt: one timer for each switch group (A-Z), ticks left
extern int halt_timer[27];
// Tick when the last halt of that switch group ended
extern int halt_end[27];
// Ticks a train must still wait (safety buffer or rain)
extern int delay_t[30];
extern int rain_moves[30];
// held: train waits on its tile this tick. blocked: its move was blocked by a conflict
extern bool held[30];
extern bool blocked[30];
// How a train ended: 0 still running, 1 delivered, 2 crashed
extern int final_state[30];
extern bool final_logged[30];
extern int final_pos[30][2];
// Signal colours: 0 green, 1 yellow, 2 red
extern int signalsOld[27];
extern int signalShown[27];
// Counters for metrics.txt
extern int waitTotal;
extern int switchFlips;
extern int signalViolations;
extern int bufferCount;
extern int energyUsed;
// Fixed rules from the manual
const int HALT_TICKS=3;
// Rain: a train waits one extra tick after this many moves (the manual gives no number)
const int RAIN_N=5;
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