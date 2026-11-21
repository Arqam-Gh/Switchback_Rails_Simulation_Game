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
Delivered: 10/10 | Crashed: 0
```

## Prerequisites

| Tool | Version | Check with |
|---|---|---|
| `g++` | any version with C++11 support, such as 5 or newer | `g++ --version` |
| `make` | any recent version | `make --version` |
| `git` | any recent version, only to clone the repo | `git --version` |

## What it does

- Loads a level from a text file. The file holds the map, the switches and the train list.
  The level file name can be given on the command line.
- Moves every train one tile per tick over `-`, `=`, `|`, `+`, `/` and `\` track.
- Turns trains 90 degrees on curves.
- At a crossing `+`, a train picks the direction (straight, right or left, never
  back) whose next tile is track and is closest to its destination.
- Counts trains entering each switch. When a counter reaches its limit, the
  switch flips after that tick's movement (a deferred flip).
- Supports two switch modes. `PER_DIR` keeps a counter for each direction and
  `GLOBAL` keeps one counter for all trains.
- In the turn state, a switch turns the train 90 degrees onto the side that has
  track. If both sides have track, it picks the side closer to the destination.
  If neither side has track, the train goes straight.
- Finds the nearest destination `D` for each train when it appears.
- Detects two trains aiming at the same tile and two trains swapping tiles.
  The train closer to its destination goes first. If two trains aiming at the
  same tile are the same distance away, the train with the lower number goes
  and the other waits, and an emergency halt starts. If two trains swap tiles
  at the same distance, both crash and an emergency halt does not start.
- During an emergency halt, no train moves for 3 ticks. A halt does not start
  again on the tick right after one ends.
- Stores the signal colour of every switch each tick (0 green, 1 yellow, 2 red).
  A switch is red when a train is 1 tile or less away, and yellow when a train is
  3 tiles or less away.
- Writes log files to an `out/` folder inside the folder the program runs from.
- Counts delivered and crashed trains and prints the grid every tick.

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
that is `levels/out/`). The folder is listed in `.gitignore`.

| File | Content |
|---|---|
| `trace.csv` | One line per active train per tick: `tick,train,x,y,direction,ACTIVE` |
| `switches.csv` | One line per switch per tick: `tick,switch,mode,state` (state 0 straight, 1 turn) |
| `signals.csv` | One line per switch per tick: `tick,switch,colour` |
| `metrics.txt` | Total trains, total ticks, delivered, crashed and weather |

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
5. Detect and resolve collisions.
6. Move the trains. A train that leaves the track crashes. No train moves during an emergency halt.
7. Apply the queued switch flips.
8. Check arrivals at destinations.
9. Update signal colours and the halt timer.
10. Write the trace, switch and signal logs.
11. Print the grid.

The program stops when no train is left on the map, then writes `out/metrics.txt`.

## Level files

Four levels are included in `levels/`:

| File | Size | Trains | Result of the current version |
|---|---|---|---|
| `easy_level.lvl` | 15 x 40 | 2 | 2 delivered |
| `medium_level.lvl` | 20 x 50 | 5 | 5 delivered |
| `hard_level.lvl` | 25 x 60 | 8 | 8 delivered |
| `complex_network.lvl` | 30 x 70 | 10 | 10 delivered |

The format is explained in [docs/LEVEL_FORMAT.md](docs/LEVEL_FORMAT.md).

## Current status and known limits

This project is a work in progress. These are the limits as of this version:

- **Two trains that swap tiles at the same distance still crash.** No bundled
  level does this. Two trains that want the same tile at the same distance no
  longer crash. The one with the higher train number waits.
- **Trains can wait in a line.** A waiting train blocks the trains behind it,
  and there is no check for a deadlock where trains wait on each other forever.
- **Weather has no effect on trains.** It is read, printed at the start and
  written to `out/metrics.txt`. No rule in the code uses it.
- **Switch letters and map letters share the alphabet.** The letters `D` and `S`
  are also used for destinations and spawn points, so a switch named `D` or `S`
  in a level file is counted on those tiles too.
- **Switches are matched by letter order.** Switch lines in a level file must
  start at `A` and not skip letters.

## Ideas for next steps

- Choose destinations so that trains do not all go to the same `D`.
- Give weather a real effect on the simulation.
- Replace the global arrays with a struct for trains and a struct for switches.

## License

Released under the MIT License. See [LICENSE](LICENSE).
