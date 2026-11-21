# Switchback Rails

A railway simulation written in C++. Trains drive over a grid of track,
switches flip after a set number of trains pass, and the program stops trains
that would run into each other.

The whole program uses only functions, arrays and global state. It has no
classes. The goal of the project was to practice splitting a program into
several files.

## Quickstart

```bash
git clone https://github.com/Arqam-Gh/Switchback_Rails_Simulation_Game.git
cd Switchback_Rails_Simulation_Game
make
make run
```

You should see the grid printed for every tick and a summary line at the end.
This is the real output of the current version, with the grid lines cut out:

```
Simulation Started
Level: Complex Interconnected Railway Network - 10 Trains | Weather: NORMAL

=== TICK 1 ===
...
Active Trains: None
Delivered: 8/10 | Crashed: 2
```

Two trains crash in this level because the manual says trains that want the same
tile at the same distance from their destinations crash. See
[Current status and known limits](#current-status-and-known-limits).

## Prerequisites

| Tool | Version | Check with |
|---|---|---|
| `g++` | any version with C++11 support, such as 5 or newer | `g++ --version` |
| `make` | any recent version | `make --version` |
| `git` | any recent version, only to clone the repo | `git --version` |

## What it does

This follows the project manual (CS-1002 Switchback Rails).

- Loads a level from a text file. The level file name can be given on the command line.
  Windows line endings are accepted.
- Moves every train one tile per tick over `-`, `|`, `+`, `/` and `\` track.
- Turns trains 90 degrees on curves.
- At a crossing `+`, a train picks the direction (straight, right or left, never
  back) whose next tile is track and is closest to its destination.
- Finds the nearest destination `D` for each train when it appears.
- A train that is scheduled to appear waits if its start tile is taken.
- Safety buffer `=`: a train that enters it stays on it for one extra tick.
- Switches `A` to `Z`. All tiles with the same letter share one state and
  counters. `PER_DIR` keeps a counter for each direction and `GLOBAL` keeps one
  counter. When a counter reaches its K value, the switch flips after that tick's
  movement and the counters reset.
- The state words in the level file decide the route: `STRAIGHT` goes straight,
  `TURN` turns onto the side that has track, `LEFT` and `RIGHT` turn to that side.
- Collisions use the distance to the destination (Manhattan distance):
  - Two or more trains want the same tile (also at a `+`): the train with the
    higher distance moves and the others wait. If the highest distance is shared,
    those trains crash.
  - Two trains swap tiles: the train with the higher distance moves and the other
    waits. If the distances are equal, both crash.
  - A train also waits if the tile it wants is taken by a train that is not moving.
  - A train that tries to enter a tile that is not track crashes.
- Emergency halt: when a collision happens next to a switch group, the trains in
  the 3 x 3 area around that group wait for 3 ticks. Counters and switch states
  keep running. The same group does not halt again on the tick after a halt ends.
- Signal lights for each switch group: `GREEN` if the tile ahead on the planned
  route is free, `YELLOW` if a train is within two tiles ahead, `RED` if the tile
  ahead is blocked or taken, or the train must wait because of a collision.
- Weather: `NORMAL`, `RAIN` (one extra wait tick after every 5 moves) or `FOG`
  (displayed signals are one tick late).
- Prints the grid, the trains, the switch states and the signals every tick.
- Writes `trace.csv`, `switches.csv`, `signals.csv` and `metrics.txt` to `out/`.

## Build and run

You need `g++` (C++11 or newer) and `make`.

```bash
make        # builds build/switchback
make run    # runs the simulation
make clean  # removes build/
```

`make run` starts the program inside the `levels/` folder, with the default
level `complex_network.lvl`.

To run another level, give its path as the first argument:

```bash
./build/switchback levels/easy_level.lvl
```

If the file cannot be opened, the program prints `Cannot open level file` and exits.

## Output files

The program creates an `out/` folder in the folder it runs from (for `make run`,
that is `levels/out/`). The folder is listed in `.gitignore`. Each file starts
with a header line.

| File | Columns or content |
|---|---|
| `trace.csv` | `Tick,TrainID,X,Y,Direction,State`. State is `MOVING`, `WAITING`, `DELIVERED` or `CRASHED`. |
| `switches.csv` | `Tick,Switch,Mode,State`. State is the word of the current state (for example `STRAIGHT`). |
| `signals.csv` | `Tick,Switch,Signal`. Signal is `GREEN`, `YELLOW` or `RED`. |
| `metrics.txt` | Delivered, crashed, throughput (delivered per 100 ticks), average wait (idle ticks per train), signal violations (trains that entered a switch on a red signal), switch flips, safety buffers, energy used and energy efficiency (trains x ticks / buffers). |

Running the same level twice gives identical files.

## Examples

**Run the default level and keep the output in a file:**

```bash
make run > run.txt
tail -n 2 run.txt
```

You should see `Active Trains: None` and a `Delivered: ... | Crashed: ...` line.

**Run the easy level:**

```bash
make
./build/switchback levels/easy_level.lvl | tail -n 2
```

You should see a last line of `Delivered: 2/2 | Crashed: 0`.

To write your own level, follow [docs/LEVEL_FORMAT.md](docs/LEVEL_FORMAT.md).

## Project layout

```
Switchback_Rails_Simulation_Game/
├── src/                      C++ source files
│   ├── main.cpp              Entry point and main loop
│   ├── simulation.cpp        One tick of the simulation, grid printing
│   ├── simulation_state.cpp  Global variables and reset
│   ├── trains.cpp            Spawning, routing, movement, collisions, arrivals
│   ├── switches.cpp          Switch counters, flips, signal colours
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
5. Resolve waits, collisions and halts.
6. Work out the signal colours.
7. Move the trains. A train that leaves the track crashes. A train that enters `=` or is slowed by rain gets a wait.
8. Apply the queued switch flips.
9. Check arrivals at destinations.
10. Update the halt timers and the displayed signals.
11. Write the trace, switch and signal logs.
12. Print the grid.

The program stops when no train is left on the map or after 10000 ticks, then
writes `out/metrics.txt`.

## Level files

Four levels are included in `levels/`:

| File | Size | Trains | Result of the current version |
|---|---|---|---|
| `easy_level.lvl` | 15 x 40 | 2 | 2 delivered |
| `medium_level.lvl` | 20 x 50 | 5 | 5 delivered |
| `hard_level.lvl` | 25 x 60 | 8 | 2 delivered, 6 crashed |
| `complex_network.lvl` | 30 x 70 | 10 | 8 delivered, 2 crashed |

The format is explained in [docs/LEVEL_FORMAT.md](docs/LEVEL_FORMAT.md).

## Current status and known limits

This project is a work in progress. These are the limits as of this version:

- **Crashes in `hard_level` and `complex_network`.** Each crash is two trains that
  want the same tile while the same distance from their destinations. The manual
  says they crash. The levels have no `=` safety buffers, which are the way to
  delay a train so that the distances differ.
- **Things the manual does not define.** These are choices made in the code:
  - What starts an emergency halt (here: a collision next to a switch group).
  - How a destination is given to a train (here: the nearest `D` when it appears).
  - The rain rule "after n moves" (here: n is 5, the constant `RAIN_N`) and the fog delay (here: 1 tick).
  - The exact signal rules for a switch (see the list above).
  - `SEED` is read but not used. The manual says spawn offsets are seeded by it
    but gives no range, so trains appear at the tick written in the level file.
- **Waiting trains can block each other.** There is no check for trains that wait
  on each other forever. The program stops after 10000 ticks in that case.
- **Switch letters and map letters share the alphabet.** `D` and `S` are also used
  for destinations and spawn points, so a switch named `D` or `S` in a level file
  is counted on those tiles too.
- **No SFML window and no sprites.** Only the console part of the manual is done.

## Ideas for next steps

- Place `=` safety buffers in the hard and complex levels so that no train crashes.
- Add the SFML window with sprites (the bonus part of the manual).
- Replace the global arrays with a struct for trains and a struct for switches.

## License

Released under the MIT License. See [LICENSE](LICENSE).
