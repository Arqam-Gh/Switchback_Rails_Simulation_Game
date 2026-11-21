# Level file format

A level is a plain text file with the extension `.lvl`. The loader reads it
section by section. Each section starts with a keyword that ends in a colon.

```
NAME:
Easy Level - Simple Railway

ROWS:
15

COLS:
40

SEED:
12345

WEATHER:
NORMAL

MAP:
(grid rows, one text line per row)

SWITCHES:
A PER_DIR 0 2 2 2 2 STRAIGHT TURN

TRAINS:
0 2 2 1 0
```

## Sections

| Section | Meaning |
|---|---|
| `NAME:` | Level title, one line. |
| `ROWS:` and `COLS:` | Size of the map. |
| `SEED:` | A number. It is read and stored, no rule uses it yet. |
| `WEATHER:` | `NORMAL`, `RAIN` or `FOG`. See the weather table below. |
| `MAP:` | The track drawing, one text line per row. It ends at the first empty line or when `SWITCHES:` starts. |
| `SWITCHES:` | One line per switch. Ends at the first empty line or when `TRAINS:` starts. |
| `TRAINS:` | One line per train, up to 30. Ends at the end of the file or at the first line that is not 5 numbers. |

`ROWS:` and `COLS:` must be between 1 and 100. A level that breaks a rule
(bad number, unknown switch mode, train direction not 0 to 3) is not loaded and
the program prints which line is wrong.

## Weather

| Word | Effect |
|---|---|
| `NORMAL` | Trains move 1 tile per tick. |
| `RAIN` | A train waits one extra tick after every 5 moves. The manual does not give the number 5, it is the constant `RAIN_N`. |
| `FOG` | The signal colours shown in the terminal and in `signals.csv` are one tick late. The train logic is not delayed. |

## Map symbols

| Symbol | Meaning |
|---|---|
| `-` | Horizontal track |
| `=` | Safety buffer. A train that enters it stays on it for one extra tick. |
| `\|` | Vertical track |
| `+` | Crossing |
| `/` and `\` | Curves. A train turns 90 degrees here. |
| `A` to `Z` | A switch. The letter is the switch name. |
| `S` | Spawn point |
| `D` | Destination |

## Switch line

```
A PER_DIR 0 2 2 2 2 STRAIGHT TURN
```

1. Letter of the switch on the map.
2. Mode. `PER_DIR` keeps one counter for each direction a train enters from.
   `GLOBAL` keeps one counter for all trains.
3. Start state. `0` means the first word at the end of the line, `1` means the second word.
4. Four numbers, one for each direction (up, right, down, left). When the
   counter for a direction reaches its number, the switch flips. In `GLOBAL`
   mode only the first of the four numbers is used.
5. The two words at the end are the names of state 0 and state 1. They decide the route:
   `STRAIGHT` goes straight, `TURN` turns 90 degrees onto the side that has track
   (the side closer to the destination if both have track), `LEFT` and `RIGHT` turn
   to that side. If the side has no track, the train goes straight.

Switch letters can be in any order and can skip letters. A tile with the same
letter as a switch shares its state and counters.

## Train line

```
0 2 2 1 0
```

1. Tick when the train appears.
2. Start column (x).
3. Start row (y).
4. Start direction: `0` up, `1` right, `2` down, `3` left.
5. Colour number.

If the start tile is taken at that tick, the train waits and tries again on the next tick.
