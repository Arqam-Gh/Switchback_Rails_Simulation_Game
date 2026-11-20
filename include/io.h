#ifndef IO_H
#define IO_H

// ============================================================================
// IO.H - Level I/O and logging
// ============================================================================

// ----------------------------------------------------------------------------
// LEVEL LOADING
// ----------------------------------------------------------------------------
// Load a .lvl file.
bool loadLevelFile();
void printGrid();
// file to be read will be there in the place of that variable we will be using this var to deal to file
// ----------------------------------------------------------------------------
// LOGGING
// ----------------------------------------------------------------------------
// Create/clear log files.
void initializeLogFiles();

// Append train movement to trace.csv.
void logTrainTrace();

// Append switch state to switches.csv.
void logSwitchState();

// Append signal state to signals.csv.
void logSignalState();

// Write final metrics to metrics.txt.
void writeMetrics();

#endif
