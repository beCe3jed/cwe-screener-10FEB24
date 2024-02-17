# Marine Tetris Programming Challenge 

## Overview
If you've ever been on a field op, you know how hard it is to figure out the right mix of what to pack! Help our Marine friend, Chesty, figure out what he should bring to the field, and what he should leave behind!

Note: Being able to accomplish this problem optimally in C is excellent practice for the technical interview. 

## Instructions
### Reading the Packing List
In ./data.csv, please find an example CSV containing the data for each item Chesty is considering taking to the field. Feel free to use this CSV to test your implementation.
The first row contains the name of each item. 
The second row contains the weight of each item. 
The third row contains the value of each item. 

That is, the format of the CSV is:
``` 
| Item1 Name,   | Item2 Name,  | ...
| Item1 Weight, | Item2 Weight,| ...
| Item1 Value,  | Item 2 Value,| ...
```

### Finding the Optimal Packing List
Your mission, should you choose to accept, is to identify Chesty's optimal value packing list - that is, the list of items Chesty should pack to maximize his value given a constraint of how much weight he can carry - 35 lbs. 

### Finding the Flag
The flag for this problem will be the string created by alphabetically sorting the first letter of every item Chesty should bring in the optimal packing. That is, if the optimal packing list is "MRE1, Ammo1, Bayonet"; then the flag would be "flag{ABM}"

### Performance
If you find that you can run 1.5 miles faster than your code finishes on this input, there is likely a better way to solve the problem that involves making the computer do less work.

## Scoring
As always when preparing your writeup, you will be evaluated based on the following software engineering pillars:

- Correctness (printing the flag)
- Safe from bugs (handles bad input without crashing)
- Easy to understand (commented and inuitive program structure)
- Ready for change (modular implementation)
- Adherence to the [Google C Style Guide](https://google.github.io/styleguide/cppguide.html).
