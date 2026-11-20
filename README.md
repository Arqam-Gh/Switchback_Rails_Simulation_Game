# Switchback Rails

A railway simulation written in C++. Trains drive over a grid of track,
switches flip after a set number of trains pass, and the program stops trains
that would run into each other.

The whole program uses only functions, arrays and global state. It has no
classes. The goal of the project was to practice splitting a program into
several files.

## Quickstart

```bash
git clone https://github.com/Arqam-Gh/switchback-rails.git
cd switchback-rails
make
make run
```

You should see the grid printed for every tick and a summary line at the end.
This is the real output of the current version, with the grid lines cut out:

```
Simulation Started
Press SPACE to start

=== TICK 1 ===
...
Active Trains: None
Delivered: 0/10 | Crashed: 10
```

The program does not deliver any train yet, because of the row offset bug
described in [Current status and known limits](#current-status-and-known-limits).

## Prerequisites

| Tool | Version | Check with |
|---|---|---|
| `g++` | any version with C++11 support, such as 5 or newer | `g++ --version` |
| `make` | any recent version | `make --version` |
| `git` | any recent version, only to clone the repo | `git --version` |

## What it does

- Loads a level from a text file. The file holds the map, the switches and the train list.
- Moves every train one tile per tick over `-`, `=`, `|`, `+`, `/` and `\` track.
- Turns trains 90 degrees on curves.
- Counts trains entering each switch. When a counter reaches its limit, the
  switch flips after that tick's movement (a deferred flip).
- Supports two switch modes. `PER_DIR` keeps a counter for each direction and
  `GLOBAL` keeps one counter for all trains.
- Finds the nearest destination `D` for each train when it appears.
- Detects two trains aiming at the same tile and two trains swapping tiles.
  The train closer to its destination goes first. If both are the same
  distance away, both crash.
- Counts delivered and crashed trains and prints the grid every tick.

## Build and run

You need `g++` (C++11 or newer) and `make`.

```bash
make        # builds build/switchback
make run    # runs the simulation
make clean  # removes build/
```

`make run` starts the program inside the `levels/` folder, because the
program opens its level file from the current folder.

## Examples

**Run the default level and keep the output in a file:**

```bash
make run > run.txt
tail -n 2 run.txt
```

You should see `Active Trains: None` and a `Delivered: ... | Crashed: ...` line.

**Run the easy level:** the program always opens `complex_network.lvl`, so
copy the level you want into a new folder under that name:

```bash
mkdir demo
cp levels/easy_level.lvl demo/complex_network.lvl
cd demo && ../build/switchback
```

You should see five ticks and a last line of `Delivered: 0/2 | Crashed: 2`.

To write your own level, follow [docs/LEVEL_FORMAT.md](docs/LEVEL_FORMAT.md).

## Project layout

```
switchback-rails/
├── src/                      C++ source files
│   ├── main.cpp              Entry point and main loop
│   ├── simulation.cpp        One tick of the simulation, grid printing
│   ├── simulation_state.cpp  Global variables and reset
│   ├── trains.cpp            Spawning, routing, movement, collisions, arrivals
│   ├── switches.cpp          Switch counters, flips, signal distance check
│   ├── grid.cpp              Tile checks (track, switch, bounds)
│   └── io.cpp                Level loading and log writers
├── include/                  Header files, one for each source file
├── levels/                   Level files (easy, medium, hard, complex)
├── docs/
│   └── LEVEL_FORMAT.md       How a level file is written
├── Makefile
├── LICENSE
└── README.md
```

## How one tick works

`simulateOneTick()` in `src/simulation.cpp` runs these steps in order:

1. Spawn trains scheduled for this tick.
2. Work out the next tile and direction for every train.
3. Update switch counters for trains that entered a switch.
4. Mark switches whose counter reached its limit.
5. Detect and resolve collisions.
6. Move the trains. A train that leaves the track crashes.
7. Apply the queued switch flips.
8. Check arrivals at destinations.
9. Update signal distances and the halt timer.
10. Print the grid.

The program stops when no train is left on the map.

## Level files

Four levels are included in `levels/`:

| File | Size | Trains |
|---|---|---|
| `easy_level.lvl` | 15 x 40 | 2 |
| `medium_level.lvl` | 20 x 50 | 5 |
| `hard_level.lvl` | 25 x 60 | 8 |
| `complex_network.lvl` | 30 x 70 | 10 |

The format is explained in [docs/LEVEL_FORMAT.md](docs/LEVEL_FORMAT.md).

## Current status and known limits

This project is a work in progress. These are the limits as of this version:

- **The level name is fixed in the code.** `loadLevelFile()` in `src/io.cpp`
  always opens `complex_network.lvl`. To run another level, change that name
  or copy the level over it.
- **Train rows do not line up with the maps.** In the bundled levels, every
  train starts one row below its `S` tile, so each train leaves the track and
  crashes as soon as it appears. In the current version none of the four
  levels delivers a train. This is the first thing to fix.
- **Crossing routing is a stub.** `getSmartDirectionAtCrossing()` only
  returns the current direction, so trains go straight through `+`.
- **Emergency halt is never triggered.** The timer code exists, but nothing
  calls `applyEmergencyHalt()`.
- **Weather, signals and log files are read or computed but not used.**
  Weather is stored, signal colours are not saved anywhere, and the log
  writers in `src/io.cpp` are never called.
- **Switch turning is basic.** A switch in the turn state only handles some
  entry directions, so not every junction behaves like a real one.

## Ideas for next steps

- Take the level file name from the command line.
- Fix the row offset between level files and the grid.
- Choose the direction at a crossing by moving toward the train's destination.
- Call the emergency halt when a collision is about to happen.
- Write the trace, switch and signal logs to an `out/` folder.
- Replace the global arrays with a struct for trains and a struct for switches.

## License

Released under the MIT License. See [LICENSE](LICENSE).
