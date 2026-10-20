# Sort Visualizer

A terminal-based sorting visualizer written in C++17. It renders animated, solid-color bar charts with live comparison and move counters and a minimal interface.

## Files

- `sort.cpp` - complete source code
- `sort.exe` - Windows x64 build (static, no extra runtime DLLs required)

## Build

```bash
g++ -std=c++17 -O2 -static -o sort.exe sort.cpp
```

Use any ANSI-capable terminal. Windows Terminal is recommended.

## Run

```bash
./sort.exe
```

On Windows:

```powershell
.\sort.exe
```

## Included Algorithms

1. Bubble Sort
2. Selection Sort
3. Insertion Sort
4. Shell Sort
5. Merge Sort
6. Quick Sort
7. Heap Sort
8. Counting Sort
9. Radix Sort (LSD)

## Visualization

- Solid colored bars represent values; height is scaled to the current maximum and color follows value order.
- White bar: an element being compared.
- Yellow bar: an element being swapped.
- Cyan bar: an element being written.
- Magenta bar: pivot, key, or counting/radix focus.
- Only the algorithm name, basic counters, current operation, index row, and bars are displayed.

## Controls

| Key | Action |
| --- | --- |
| `1` - `9` | Run the corresponding sorting algorithm |
| `A` | Run all nine algorithms on the current data |
| `R` | Generate new random data |
| `M` | Enter custom values manually |
| `N` | Change the number of values (5-32) |
| `S` | Change animation speed (1 = slow, 9 = instant) |
| `Q` | Quit |

## Verification

After every run, the program checks that:

1. The array is sorted in ascending order.
2. The sorted array is a permutation of the original array, so no value is lost or duplicated.

The result is shown as a green `SORTED` or a red `ERROR`.

## Notes

- Random values are in the range 1-99.
- Manual values must be in the range 1-999.
- The program uses ANSI escape sequences for colors, cursor movement, and the alternate screen.
- On Windows, virtual terminal processing is enabled automatically when the console supports it.
- A terminal at least 100 columns wide is recommended.
- Default data size is 24 and the default speed is 6.
