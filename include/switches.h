#ifndef SWITCHES_H
#define SWITCHES_H
#include<string>
using namespace std;
// ============================================================================
// SWITCHES.H - Switch logic
// ============================================================================
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
// ----------------------------------------------------------------------------
// SWITCH COUNTER UPDATE
// ----------------------------------------------------------------------------
// Increment counters when trains enter switches.
void updateSwitchCounters();

// ----------------------------------------------------------------------------
// FLIP QUEUE
// ----------------------------------------------------------------------------
// Queue flips when counters reach K.
void queueSwitchFlips();

// ----------------------------------------------------------------------------
// DEFERRED FLIP
// ----------------------------------------------------------------------------
// Apply queued flips after movement.
void applyDeferredFlips();

// ----------------------------------------------------------------------------
// SIGNAL CALCULATION
// ----------------------------------------------------------------------------
// Update switch signal colors.
void updateSignalLights();

// ----------------------------------------------------------------------------
// SWITCH TOGGLE (for manual control / editing)
// ----------------------------------------------------------------------------
// Manually toggle a switch state.
void toggleSwitchState(int I);

// ----------------------------------------------------------------------------
// HELPER FUNCTIONS
// ----------------------------------------------------------------------------
// Get state for a given direction.
int getSwitchStateForDirection(int I, int enterDir);

#endif
