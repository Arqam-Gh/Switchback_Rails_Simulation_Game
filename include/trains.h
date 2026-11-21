#ifndef TRAINS_H
#define TRAINS_H

// ============================================================================
// TRAINS.H - Train logic
// ============================================================================
extern bool trainActive[30];
extern int t_train;
extern int active_coordinates[30][2];
extern int prev_coordinates[30][2];
extern int active_direc[30];
extern int active_colour[30];
extern int pre_coordinates[30][2];
extern int pdist,dist;
extern int fdest[30][2];
extern int next_coor[30][2];
extern int next_direc[30];
extern bool conflict;
extern int cdist[30];
extern int tick;
extern int rows,columns,seed,trains[30][5],Rows;
extern char grid[100][100];
// ----------------------------------------------------------------------------
// TRAIN SPAWNING
// ----------------------------------------------------------------------------
// Spawn trains scheduled for the current tick.
void spawnTrainsForTick();

// ----------------------------------------------------------------------------
// TRAIN ROUTING
// ----------------------------------------------------------------------------
// Compute routes for all trains (Phase 2).
void determineAllRoutes();

// Compute next position/direction for a train.
bool determineNextPosition(int k);

// Get next direction on entering a tile.
int getNextDirection(int k);

// Choose best direction at a crossing.
int getSmartDirectionAtCrossing(int trainIdx);

// ----------------------------------------------------------------------------
// TRAIN MOVEMENT
// ----------------------------------------------------------------------------
// Move trains and handle collisions (Phase 5).
void moveAllTrains();

// ----------------------------------------------------------------------------
// COLLISION DETECTION
// ----------------------------------------------------------------------------
// Detect trains targeting the same tile/swap/crossing.
void detectCollisions();

// ----------------------------------------------------------------------------
// ARRIVALS
// ----------------------------------------------------------------------------
// Mark trains that reached destinations.
void checkArrivals();

// ----------------------------------------------------------------------------
// EMERGENCY HALT
// ----------------------------------------------------------------------------
// Apply emergency halt in active zone.
// Start a 3 tick halt on switch group I (0 = A). Trains in the 3x3 area around
// any tile of that group wait.
void applyEmergencyHalt(int I);

// End a train. state 1 = delivered, 2 = crashed.
void endTrain(int k, int state);

// Direction number to word (UP, RIGHT, DOWN, LEFT).
const char* directionName(int d);

// Update emergency halt timer.
void updateEmergencyHalt();

#endif
