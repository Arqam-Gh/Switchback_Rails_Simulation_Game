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
| `SEED:` | Number for random choices. It is read and stored. |
| `WEATHER:` | Weather word such as `NORMAL`. It is read and stored. |
| `MAP:` | The track drawing, one text line per row. |
| `SWITCHES:` | One line per switch. Ends at the first blank line. |
| `TRAINS:` | One line per train. Ends at a line of zeros. |

## Map symbols

| Symbol | Meaning |
|---|---|
| `-` or `=` | Horizontal track |
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
3. Start state. `0` means the train goes straight. `1` means it turns.
4. Four numbers, one for each direction (up, right, down, left). When the
   counter for a direction reaches its number, the switch flips. In `GLOBAL`
   mode only the first of the four numbers is used.
5. The two words `STRAIGHT` and `TURN` are read and stored.

## Train line

```
0 2 2 1 0
```

1. Tick when the train appears.
2. Start column (x).
3. Start row (y).
4. Start direction: `0` up, `1` right, `2` down, `3` left.
5. Colour number.

The train list ends at the first line where the first three numbers are all `0`.
That means a train cannot start at tick 0 from column 0, row 0.
