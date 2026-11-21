#include "simulation_state.h"
#include "simulation.h"
#include "io.h"
#include <iostream>
using namespace std;

// ============================================================================
// MAIN.CPP - Entry point of the application (NO CLASSES)
// ============================================================================

// ----------------------------------------------------------------------------
// MAIN ENTRY POINT
// ----------------------------------------------------------------------------
// This function is the main entry point of the application. It handles command
// line arguments to specify the level file to load, loads the level file using
// loadLevelFile, initializes the simulation system, initializes the SFML
// application window, prints control instructions to the console, runs the
// main application loop, cleans up resources, and prints final simulation
// statistics. Returns 0 on success, 1 on error (e.g., failed to load level
// file or initialize application).
// ----------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    initializeSimulationState();
    if(!loadLevelFile(argc>1 ? argv[1] : "complex_network.lvl")) return 1;
    initializeSimulation();
    const int MAX_TICKS=10000;
    while(!isSimulationComplete() && tick<MAX_TICKS){
    simulateOneTick();
    }
    if(tick>=MAX_TICKS) cout<<"Stopped: reached the limit of "<<MAX_TICKS<<" ticks\n";
    writeMetrics();
}
