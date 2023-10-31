(this is not part of the license)

Project description: 

# EECS 281: Project 1 - Puzzle Solver

Due Tuesday September 19, 2023 at 11:59 PM

## Overview

EECS281Games™ is developing a puzzle game that they want to bring to the market. They’ve already built the engine and designed thousands of maze levels, but they realized that they’re not sure how to solve many of them! What’s worse, they think some of them may be unsolvable!

You are tasked with developing a C++ program that will take as input a level map to try and solve, and some command line options indicating how your program will behave. Your solution will read the map and confirm that the input is valid, then attempt to solve the puzzle. Finally, it will output information about the puzzle’s solvability and a description of the puzzle’s solution (if one exists).

### Learning Objectives

These are the skills and concepts encountered in this project:

- 2D/3D Maze: read, store, access, and write
- Breadth first search (BFS w/ queue)
- Depth first search (DFS w/ stack)
- Map and coordinate list mode output
- Create custom data structures for efficient storage and access
- Use `getopt_long()` to handle command line inputs
- Use `std::cin` and `std::cout` to handle input and output
- Use `std::cerr` to handle error messages
- Use `std::vector<>` to store and access data
- Use `std::deque<>` to store and access data

## Command Line Input

Your solution will run from the command line, in the spirit of the Unix tradition. This includes using command line options and redirection of standard input (`cin`).

### Modifying Behavior

Programs often offer *options* that can change how they work at runtime. These are specified at the command line in the form of either “short options”, which are a single character following a single hyphen (eg. `-o`) or “long options”, which use full words following a double hyphen (eg. `--a-long-option`). Multiple short options can be combined (eg. `-al` is equivalent to `-a -l`). Both short and long options can also accept *arguments*, which allow a custom value to be associated with an option. Each option can independently prohibit, require, or optionally accept arguments.

Q: Why both short and long options?

A1: There are only 52 different single letter options, but an infinite number of long options can be created.

A2: Two programs can implement the same functionality using different options (eg. `-R` or `-r` for `--recursive`), but full words are more easily remembered.

A3: Short options are quick and easy to type at the command line, and when programs are used in scripts, long options make scripts more readable.

The solution to this problem must accept both short and long options. Some options will be implemented with no arguments, and others will be implemented with required arguments.

Clarification: An option with a required argument is still optional and might not be specified at the command line, but if it is specified, an argument must be provided.

The complicated task of parsing options and arguments is made easier with a classic library `<getopt.h>` that provides both `getopt()` for handling short options only, and `getopt_long()` for handling short and long options. A helpful reference can be found at the [getopt man page](https://linux.die.net/man/3/getopt).

### Puzzle Options

Your program, `puzzle`, should accept the following command line options:

- `--help/-h`

  This option prohibits arguments and causes the program to print a helpful message of your own design, and then return 0 to exit.

- `--queue/-q`

  This option prohibits arguments and directs the program to use a search container that behaves like a queue, performing a breadth first search (exactly one of `stack` or `queue` must be specified, exactly once)

- `--stack/-s`

  This option prohibits arguments and directs the program to use a search container that behaves like a stack, performing a depth first search (exactly one of `stack` or `queue` must be specified, exactly once)

- `--output {TYPE}/-o {TYPE}`

  This option requires an argument, `{TYPE}`, that must be either `map` or `list`, and directs the program to print solution output in the specified format (see [Output](https://eecs281staff.github.io/p1-puzzle/#output))

If the `--output` flag isn’t provided to your program, then your program should act as though `--output map` was provided.

You are only required to error check a subset of the command line inputs (see [Errors You Must Check For](https://eecs281staff.github.io/p1-puzzle/#errors-you-must-check-for)), but you might find it helpful during testing to account for all of the possible cases and print a descriptive error message (e.g. if the filename is not provided, or the file cannot be opened).

The autograder uses a mix of short and long options in different orders. Do not rely on any option being in any fixed location in `argv[]`, and make sure that your solution can handle both long and short options equivalently.

### Legal Command Line Examples

`$ ./puzzle --queue -o map < spec-simple.txt > output.txt`
`$ ./puzzle -o map -q < spec-simple.txt > output.txt`
`$ ./puzzle -qo map < spec-simple.txt > output.txt`
`$ ./puzzle -q --output map < spec-simple.txt > output.txt`
`$ ./puzzle -q < spec-simple.txt > output.txt`
Run the program using a search container that behaves like a queue to perform a breadth-first search and output the results in map format; Input is redirected to `cin` from `spec-simple.txt`, and output is redirected from `cout` to `output.txt`.

`$ ./puzzle -so list < spec-simple.txt`
`$ ./puzzle --stack --output list < spec-simple.txt`
`$ ./puzzle --output list --stack < spec-simple.txt`
Run the program using a search container that behaves like a stack to perform a depth-first search and output the results in list format; Input is redirected to `cin` from `spec-simple.txt`, and output is printed to the screen (`cout`).

`$ ./puzzle --help`
`$ ./puzzle -h`
Prints a helpful message and exits.

When redirecting output to a file, you can use `diff` to compare the output to a file containing the expected output. For example, if you have a file `solution.txt` containing the expected output, you can run the following command to compare the output of your program to the expected output:

|      | $ ./puzzle --queue -o map < spec-simple.txt > output.txt |
| ---- | -------------------------------------------------------- |
|      | $ diff solution.txt output.txt                           |

### Illegal Command Line Examples

`$ ./puzzle --stack --queue < spec-simple.txt`
Both `stack` and `queue` modes specified.

`$ ./puzzle -o map -o list < spec-simple.txt`
Both `map` and `list` output modes specified.

`$ ./puzzle -o < spec-simple.txt`
No argument provided to the output mode option.

## Input

A puzzle is a maze presented as a two dimensional map that is received through standard input. **Your program will not open or read any files.** When testing your program from the command line on CAEN, MacOS terminal, or WSL terminal, you can use a feature of your shell called *input redirection* using `<`, which converts to contents of a file to fill standard input that can be read in the program by using `cin`.

|      | ./puzzle < some-input-file.txt |
| ---- | ------------------------------ |
|      |                                |

Input redirection can also be used from within your IDE, but is accomplished differently for each environment. Get help on Project 0 and Lab 1, check [https://eecs280.org](https://eecs280.org/), or visit other online resources to get redirection working with your IDE of choice.

Each valid map will use a 2D coordinate system with at least one row and one column, include a starting location and a target, and composes a predefined set of characters.

### Valid Characters

The map characters and what they represent:

- `@`: the starting location (valid maps will have exactly one)
- `?`: the target (valid maps will have exactly one)
- `.`: an empty floor space
- `#`: a wall that blocks player movement
- `A`, `B`, …,`Z`: a colored door that blocks player movement when closed
- `a`, `b`, …, `z`: a colored button that closes all doors of all other colors, and opens all doors of the matching color
- `^`: a trapped button that closes all doors of all colors

### Doors and Buttons

Each map has a fixed number of available colors, 0≤�≤26, for doors and buttons that is included in the file. The first � letters of the English alphabet are used to represent the colors of doors and buttons. There may be zero or more buttons (lower case letters) of each color, and zero or more doors (upper case letters) of each color, and there may be zero or more trapped buttons (`^`).

Buttons (both trapped and colored) can be active (pressable) or inactive (pressed). When a button is inactive your solution should treat it as open floor. Colored buttons begin each game active and trapped buttons begin each game inactive.

Players of the game are unable to view button colors, so your solution must always press any active button you encounter. Pressing any button will close all open doors in the puzzle, then if it is a colored button it will open all doors of the same color; trapped buttons do not open any doors. After pressing a button of a certain color, all buttons of that color become inactive and all buttons of other colors become active again.

If a button is pressed that doesn’t match any door color, all doors close and none are opened. If a door has no matching button, it cannot be opened. These scenarios are possible because the number of available colors given in a map is independent of the number of upper- and lowercase letters found in that map.

### Input File Format

The first line of a valid map file specifies the map parameters and will contain three integers separated by spaces:

|      | <num_colors> <height> <width> |
| ---- | ----------------------------- |
|      |                               |

- `<num_colors>` is an integer showing the upper limit of the number of different colors of doors and buttons that may be found in that map. These colors will be the first `<num_colors>` letters of the English alphabet. For example, if `<num_colors>` is 4, then the valid door symbols are `'A'`, `'B'`, `'C'`, `'D'` and the valid button symbols are `'a'`, `'b'`, `'c'`, `'d'`. If `<num_colors>` is 2, any letter `'c'`..`'z'` or `'C'`..`'Z'` is invalid. 0≤ `<num_colors>` ≤26.
- `<height>` is the number of rows in the map. `<height>` ≥1.
- `<width>` is the number of columns in the map. `<width>` ≥1.

The trap symbol `'^'` is always valid, even if `<num_colors>` is 0.

After the map parameters are given, zero or more comments may appear. Comments begin a line with `//` and continue to the end of the line.

Following any comments, the map itself is given. The map is `<height>` lines long, and each line is `<width>` characters wide. Each character in the map is one of the valid symbols described in [Valid Characters](https://eecs281staff.github.io/p1-puzzle/#valid-characters).

### Simple Example

Here is a small sample input that your program could receive:

p1-puzzle/spec-simple.txt

|      | 2 4 7                      |
| ---- | -------------------------- |
|      | // A simple example puzzle |
|      | // 2 colors (A, B)         |
|      | // 4x7 grid                |
|      | @..A..b                    |
|      | .a.#B##                    |
|      | ####...                    |
|      | ?..B.^^                    |

With the game’s rules in mind, one solution to the puzzle could be:

1. Walk east one column from the starting location
2. Walk south one row
3. Press the button there, setting the current color to a and open all A doors
4. Walk north one row
5. Walk east 5 columns (passing through the open A door)
6. Press the button there, setting the current color to b, closing all A doors and opening all B doors
7. Walk west two columns
8. Walk south three rows (passing through the first open B door)
9. Walk west four columns to the target (passing through the second open B door)

Notice that steps 7-9 avoid the traps, which would close the B doors.

### Errors You Must Check For

A small portion of your grade will be based on error checking. Your program will be provided with deliberately invalid inputs, and if your program does not reject them, you will lose points.

If your program receives any invalid input (either from the map on `std::cin` or while processing the command line) it should print a message to `std::cerr`, then immediately return 1 from `main()` or call `exit(1)`. The error message is highly recommended to help you debug, but not required; the autograder will not evaluate the error message. If the input is valid (even if there is no solution), your program should return 0 from `main()`, and should NOT call `exit()` at all.

You may make only the following assumptions about the input:

- The first line of the map will be 3 unsigned integers separated by spaces
- These integers will fit in a 32-bit unsigned integer (eg. `uint32_t`).
- The map and command line options will contain only non-null ASCII characters
- `<num_colors> * <width> * <height> <= 400,000,000`
- If the `--output/-o` flag is provided, then there WILL be some argument after it (but that argument might not be a valid value)

The errors you must check for are:

- 0 <= num_colors <= 26 (0 colors is valid, it just means that there are no doors)
- 1 <= width
- 1 <= height
- Exactly one of `--stack/-s` and `--queue/-q` are provided
- The argument to `--output/-o` (if provided) is either `"list"` or `"map"`
- No invalid command line options are provided (such as `-x` or `--eecs281`)
- No invalid door or button appears in the map (if `<num_colors> == 3` , then `'M'` and `'z'` are invalid)
- No invalid characters appear in the map (`'+'` can’t appear in the map, but it could appear in a comment)
- `'@'` appears exactly once in the input map
- `'?'` appears exactly once in the input map

In all of these cases, print an informative error message to standard error and `exit(1)``.

**You do not need to check for any other errors.**

## Solving a Puzzle

The goal is to take a map as input and find a sequence of actions (moving north, east, south, and west, or pressing buttons) that bring you to the target, or confirm that a path to the target doesn’t exist. There’s no obvious strategy to solving these puzzles, so you will attempt to solve the puzzle with a brute force approach that tries every action.

The concept of a player’s **state** will be used to help organize the search. A player’s state is a combination of the color of the last button pressed (think of `^` as a color for the purpose of player state), and their position in the grid. Before any buttons have been pressed, use `^` for the initial state. The top-left corner of the map is row 0, column 0. The bottom-right corner of the map is row `height - 1`, column `width - 1`.

Including the color in the player’s state allows you to think of the puzzle in three dimensions even though the map is only two dimensional. The map (x- and y-axes) gets extended to a third dimension because it is necessary to keep information about every location in the map separately for each possible color (z-axis). These “layers” all have the same layout as the map, so that information need not be duplicated, but the layers will be needed to distinguish when a player is standing at (1, 1) after pushing `a` from when they are standing at (1, 1) after pushing `b`. Therefore, the number of layers must be equal to `<num_colors> + 1`: one layer for each color that can be opened, +1 for when all colors are closed. Use one layer for initial state and after a `^` has been pressed, where all doors (`A`..`Z`) are impassible. Another layer results from pressing an `a` button: all `A` doors and `a` buttons behave like open floor, while all other doors are impassible and all other buttons are active. The next layer is what results from pressing a `b` button, etc.

### Using Player State

First, the initial state is no color (`'^'`) plus the coordinates of the starting location, with all doors closed. (From the [Simple Example](https://eecs281staff.github.io/p1-puzzle/#simple-example1), this state is `('^', (0, 0))`.)

Secondly, how the player got to state � doesn’t affect whether or how they can win; only � itself and the map does. If you know how to get from � to the target, then any method of getting from the start to � will lead to a solution.

Third, there are at most `(num_color + 1) * width * height` possible states.

Together, these observations lead to the following strategy:

- We know how to get to the initial location from the map, we start there.
- If we know how to get to a state �, and state � is just one action away from � (for example moving north/east/south/west or pressing a button) then we know how to get to �: first get to �, then perform that one action. For example, if the current state is (`'^'`, (0, 0)) and you move east, you arrive at state (`'^'`, (0, 1)), or if the current state is (`'^'`, (1, 1)) and you press an `a` button, you arrive at state (`'a'`, (1, 1)).
- If we “learn” how to get to each reachable state, then we will eventually learn a way to the target, or find out that no such route exists.

### Discovery and Investigation

**Never use the term “visited”, or think in terms of locations visited.**

While trying to find the target, the important terms are **discover** and **investigate**.

Discovery is when a new state (color and location) is encountered. If the location of your *current_state* on the map contains an active button, you can discover a state with the same location and a new color; if the location of your *current_state* on the map doesn’t contain an active button, you can discover the states that are one row or column away, with the same color. Each state can only be discovered once and must be added to the *search_container* and then marked as discovered. If a state is encountered a second time (either from a different direction or because of different button presses) it cannot be discovered again, as adding it to the *search_container* would create a loop in the search that could never be exited.

Investigation is the act of making all possible discoveries from the *current_state*. When a new state is retrieved from the *search_container*, you can think of yourself setting the doors to that color and moving to that location. After setting the *current_state*, remove it from the *search_container*. Similar to discovery, each state can only be investigated once, but since investigation follows discovery, there is no need to keep track of whether or not a location has been investigated.

### Solving In More Detail

1. Create a *search_container* that will hold the discovered states that have not yet been investigated.
2. Discover the initial state, created from the non-color and the row and column of the starting location. Add this state to the *search_container* and record that state as having been discovered so you will not discover it a second time.
3. Take the “next state” from *search_container*, “next” refers to the oldest state when the *search_container* is a queue and the newest state when it is a stack. Call this *current_state*. Remove it from *search_container*.
4. Investigate the *current_state* by discovering states that can be reached in one action, and adding each of these states to the back of the *search_container* if they have never been discovered before.
5. Repeat from step 3 until you’ve found the target (solution), or the *search_container* is empty (unsolvable).

Based on the observations above, once this process is complete, you will have either determind a path to the target exists, or that is is not reachable from the given starting location.

## The Algorithm

The algorithm will depend on the command line arguments. If `--queue`/`-q` is provided, the *search_container* behaves like a queue, and if `--stack`/`-s` is provided, the *search_container* behaves like a stack. Think of the *search_container* as holding the list of states that you know you should consider (in the order that you found them) but haven’t considered yet.

Your solution should faithfully implement the following algorithm:

1. Initially mark all states as not discovered.

2. Add the initial state to the *search_container* and mark it as discovered.

3. Loop while the *search_container* is NOT empty.

4. Remove the “next” item from the *search_container*. This is the *current_state*, which consists of a `<color>` plus `<row>` and `<col>` coordinates `{ <color>, <row>, <col> }`.

5. If *current_state* `{ <color>, <row>, <col> }` is standing on an active button, `<button>,` there is a chance to add a color change. If `{ <button>, <row>, <col> }` is not yet discovered, add the color change to the *search_container* and mark it as discovered. Investigation from an active button will result in zero or one states being discovered.

6. If

    

   current_state

    

   ```plaintext
   { <color>, <row>, <col> }
   ```

    

   is

    

   NOT

    

   standing on an active button, there is a chance to add adjacent locations. Discover the following four states, provided they are not off the edge of the map, not previously discovered, and not a wall or closed door:

   - north/up `{ <color>, <row - 1>, <col> }`
   - east/right `{ <color>, <row>, <col + 1> }`
   - south/down `{ <color>, <row + 1>, <col> }`
   - west/left `{ <color>, <row>, <col - 1> }`

7. Repeat from step 3.

If any state at the target location is marked as discovered, immediately stop all investigation, exit the loop, and proceed to output generation. Investigation from any location not an active button will result in zero to four states being discovered, depending on the map topology and previously discovered states.

If at step 3 the *search_container* is empty, the search has failed. When there are no states left to investigate, there is no solution. If you discover the target, you’ve solved the puzzle, and can leave the loop. Trace the path backwards from the target to the starting location using backtracing.

In states 5 and 6, a new state can only be added to the *search_container* if it has not already been discovered. The *search_container* has one purpose, to show the next state that should be investigated; it must behave like a stack or queue, so cannot itself be searched. Therefore, keeping track of which states have been discovered will require something other than the *search_container* to be used. Also, when output is being generated later, it will be important to know the current state at the time of each new discovery.

In step 6, you can discover open floor locations (.) and any buttons or open doors, but you can not discover walls (#) or any doors whose color is different from the *current_state*. For example, if your current color is `e`, you can discover `E` doors, but not `B` doors. Of course all of these can only be discovered if they have not been discovered before.

If this algorithm isn’t properly executed and your solution solves the puzzle by some other method, it still won’t pass on the autograder because your output will be different.

## Output

If there is a solution to the puzzle, the output depends on the output mode, which can be configured by the `--output`/`-o` command line option’s argument. There are two modes: list output and map output.

Using the map from [Simple Example](https://eecs281staff.github.io/p1-puzzle/#simple-example) these two output modes will be described and shown below.

p1-puzzle/spec-simple.txt

|      | 2 4 7                      |
| ---- | -------------------------- |
|      | // A simple example puzzle |
|      | // 2 colors (A, B)         |
|      | // 4x7 grid                |
|      | @..A..b                    |
|      | .a.#B##                    |
|      | ####...                    |
|      | ?..B.^^                    |

### List Output

For list output, print the list of states in the solution path from the starting location to the target, inclusive. Use the format `(<color>, (<row>, <col>))` with `<color>` being a lowercase letter or `'^'` to indicate the active color, `<row>` being the 0-indexed row of the location, and `<col>` being the 0-indexed column of the location. The first state printed should be `'^'` with the starting location, and the last state printed should be the current color when the location of the target was first discovered by the algorithm.

This is list mode output using a queue *search_container*:

p1-puzzle/out-spec-simple-queue-list.txt

|      | (^, (0, 0)) |
| ---- | ----------- |
|      | (^, (0, 1)) |
|      | (^, (1, 1)) |
|      | (a, (1, 1)) |
|      | (a, (0, 1)) |
|      | (a, (0, 2)) |
|      | (a, (0, 3)) |
|      | (a, (0, 4)) |
|      | (a, (0, 5)) |
|      | (a, (0, 6)) |
|      | (b, (0, 6)) |
|      | (b, (0, 5)) |
|      | (b, (0, 4)) |
|      | (b, (1, 4)) |
|      | (b, (2, 4)) |
|      | (b, (3, 4)) |
|      | (b, (3, 3)) |
|      | (b, (3, 2)) |
|      | (b, (3, 1)) |
|      | (b, (3, 0)) |

### Map Output

For map output, print the map similar to the way it was given in the input (excluding the map parameters and any comments). Print each color in a separate map one after the other, starting with `^`, then moving from alphabetically from `A` for each legally available color as allowed by *num_colors*. Each color map should be preceded by a comment indicating the color being output and include the following replacements:

- On the solution path between the starting location and the target, empty floors and open doors should be replaced with `'+'`
- On the solution path, replace inactive buttons with `'%'` on the color map where they were discovered and with `'@'` on the color map that matches the button
- On the `// color ^` map, all trapped buttons not on the solution path are replaced with `'.'`
- On the `// color a` map, all matching buttons `a` and doors `A` not on the solution path are replaced with `'.'`
- Color maps following `a` replace buttons and doors with `'.'`, similar to `a`

This is map output using a queue *search_container*. It begins with the starting location `@` in the upper left of the `// color ^` map, moves east to the `+` and then south onto the `a` button, which is pressed and replaced by `%`. The `a` button press moves the solution path to the `// color a` map at the same location, which is replaced by `@`. Then the trail of `+` shows travel north and east until button `b` is pressed and replaced by `%`. The `b` button press moves the solution path to the `// color b` map at the same location, which is replaced by `@`, then the trail of `+` shows travel west then south then west until the target is discovered. The starting location is only displayed on the `^` map, but the target is displayed on all color maps.

p1-puzzle/out-spec-simple-queue-map.txt

|      | // color ^ |
| ---- | ---------- |
|      | @+.A..b    |
|      | .%.#B##    |
|      | ####...    |
|      | ?..B...    |
|      | // color a |
|      | .+++++%    |
|      | .@.#B##    |
|      | ####...    |
|      | ?..B.^^    |
|      | // color b |
|      | ...A++@    |
|      | .a.#+##    |
|      | ####+..    |
|      | ?++++^^    |

This is map output using a stack *search_container*:

p1-puzzle/out-spec-simple-stack-map.txt

|      | // color ^ |
| ---- | ---------- |
|      | @..A..b    |
|      | +%.#B##    |
|      | ####...    |
|      | ?..B...    |
|      | // color a |
|      | ..++++%    |
|      | .@+#B##    |
|      | ####...    |
|      | ?..B.^^    |
|      | // color b |
|      | ...A++@    |
|      | .a.#+##    |
|      | ####+..    |
|      | ?++++^^    |

Stack and queue output are almost the same, only differing in the first two moves. In a larger and/or complicated map, you would see more differences.

### No Solution

What if no solution is found? Using the same input file but replacing the `'B'` in the bottom row with either a wall (`'#'`) or different color door (`'A'`) results in an unsolvable map. With no solution, stack or queue doesn’t matter, nor does the requested output mode. Your solution should print the following header:

No solution file header

|      | No solution. |
| ---- | ------------ |
|      | Discovered:  |

This is then followed by the map as it was provided upon input, but with all undiscovered locations replaced with `'#'`. A location is undiscovered if it was never discovered in any color. This includes empty floors, doors, buttons, and the target. If there is no solution to the puzzle, the target will always be replaced with `'#'`.

p1-puzzle/out-spec-simple-no.txt

|      | No solution. |
| ---- | ------------ |
|      | Discovered:  |
|      | @..A..b      |
|      | .a.#B##      |
|      | ####...      |
|      | ####.^^      |

## Buggy Solutions and Test Files

A portion of your grade will be based on creating test files that can expose bugs. In addition to the correct solution, the autograder has several buggy implementations of the project. Each buggy implementation is based on the instructor solution, but has small bugs (either based on implementation mistakes or misreadings of the specification).

Your goal is to submit test files that cause the buggy implementations to produce different output than the correct implementation. Each time you submit, you can include up to 15 test files to the autograder (though it is possible to get full credit with fewer test files). Each test file must be valid input with size at most 10x10. The instructor solution will reject test files that are invalid (see [Errors…](https://eecs281staff.github.io/p1-puzzle/#errors-you-must-check-for)). If you submit a test file that causes the instructor solution to `exit(1)`, then that file will not be used to detect bugs.

Part of the reason to submit these test files is to create tests that you would like to know if your program gets the correct output. If you submit a test file that your program produces wrong output, for the first such file that the autograder encounters it will include your output and the correct output in your autograder feedback! So help the autograder help you, and submit test files of your own, early. Don’t wait until the last minute to submit these.

Each test file should be a valid input file named `test-n-flags.txt` where 1≤�≤15. The “flags” portion should include a combination of letters that will specify command line options to be used. Valid letters in the flags portion of the filename are:

- s: Run stack mode
- q: Run queue mode
- m: Produce map mode output
- l: Produce list mode output

The flags that you specify as part of your test filename should allow us to produce a valid command line. For instance, don’t include both `s` and `q`, but include one of them; include `m` or `l`, but if you leave it off, the autograder will run in map output mode. If `m` or `l` is present, it should follow the `s` or `q`. For example, a valid test file might be named `test-1-sl.txt` (stack mode, list output). Given this test file name, the AG would run your program with a command line similar to the following (it might use long or short options, such as `--output` instead of `-o`):

```
$ ./puzzle --stack -o list < test-1-sl.txt > test-1-output.txt
```

When you start submitting test files to the autograder, it will tell you (in the section called “Scoring student test files”) how many bugs exist, the number needed to start earning points, and the number needed for full points. It will also tell you how many are needed to start earning an extra submit/day!

## Submitting to the Autograder

Do all of your work (with all needed files, as well as test files) in some directory other than your home directory. This will be your “submit directory”. Before you turn in your code, be sure that:

- Every source code and header file contains the following project identifier in a comment at the top of the file:
  `// Project Identifier: A8A3A33EF075ACEF9B08F5B9845569ECCB423725`
- The Makefile must also have this identifier
- You have deleted all .o files and your executable(s). Typing ‘make clean’ shall accomplish this.
- Your makefile is called Makefile. Typing ‘make’ builds your code without errors and generates an executable file called `puzzle`.
- Your Makefile specifies that you are compiling with the gcc optimization option `-O3`. This is extremely important for getting all of the performance points, as `-O3` can often speed up code by an order of magnitude. You should also ensure that you are not submitting a Makefile to the autograder that compiles with the debug flag, `-g`, as this will slow your code down considerably. If your code “works” when you don’t compile with `-O3` and breaks when you do, it means you have a bug in your code!
- Your test files are named `test-n-flags.txt` and no other project file names begin with `test`. Up to 15 test files may be submitted.
- The total size of your program and test files does not exceed 2MB.
- You don’t have any unnecessary files or other junk in your submit directory and your submit directory has no subdirectories.
- Your code compiles and runs correctly using the g++ compiler. This is available on the CAEN Linux systems (that you can access via login.engin.umich.edu). Even if everything seems to work on another operating system or with different versions of GCC, the course staff will not support anything other than GCC running on CAEN Linux.

Turn in all of the following files:

- All your .h, .hpp, and .cpp files for the project
- Your Makefile
- Your test files

You must prepare a compressed tar archive or “tarball” (.tar.gz file) of all of your files to submit to the autograder. One way to do this is to have all of your files for submission (and nothing else) in one directory. Our Makefile provides the command `make fullsubmit`. Alternately you can go into this directory and run this command:

```
$ tar czvf ./submit.tar.gz *.cpp *.h *.hpp Makefile test*.txt
```

This will prepare a suitable file in your working directory.

If you’re using the Makefile that we provided with the project, these things will be done for you! When you want to prepare a submission, you can use either ‘make fullsubmit’ or ‘make partialsubmit’. The difference is that a “full submit” includes test files, while a “partial submit” does not. Use the command ‘make help’ to get our Makefile to tell you everything it can do!

Submit your project files directly to an autograder at https://eecs281staff.github.io/ag-status/. The autograders are identical and your daily submission limit will be shared (and kept track of) between them. You may submit up to three times per calendar day (more during the Spring). For this purpose, days begin and end at midnight (Ann Arbor local time). We will count only your best submission for your grade. If you would instead like us to use your **LAST** submission, see the autograder FAQ page, or [use this form](https://docs.google.com/forms/d/e/1FAIpQLSe8BxRNnZKgRI4o-V7eG5F6LY_GhhWjMyJW8yKcP4XKVm2JrQ/viewform?usp=sf_link). If you use an online revision control system, make sure that your projects and files are **PRIVATE**; many sites make them public by default! If someone searches and finds your code and uses it, this could trigger Honor Code proceedings for you.

### Scoring

Please be sure to read all messages shown at the top of the autograder results! The messages before the scoring of individual test cases are equally as important as the scores below, and will often explain outstanding issues the autograder finds.

- 80 points: Grades will be primarily based on the correctness of algorithm implementation. For full points, each submission must have correct and working stack and queue algorithms, support both types of output modes, and properly handle all command line options. Additionally: Part of the grade will be derived from the runtime performance of the submission. Fast, correct implementations will receive all possible performance points. Slower implementations may receive only a portion of the performance points.
- 10 points: Test file coverage (effectiveness at exposing buggy solutions).
- 10 points: Memory leak check with valgrind

### Libraries and Restrictions

Unless otherwise stated, you are allowed and encouraged to use all parts of the C++ STL and the other standard header files for this project. You are not allowed to use other libraries (eg: boost, pthread, etc). You are **NOT** allowed to use the regular expressions library or the thread/atomics libraries (which spoil runtime measurements). Also, do not use the STL’s unique or shared pointers.

Have fun coding!