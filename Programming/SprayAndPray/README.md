# Spray and Pray (Data Analysis) Programming Challenge 

## Overview
Chesty is eternally grateful for your help in getting him packed up for his field op. Now, he's on a live fire range, and is not having a very good time. Help our favorite Marine figure out exactly what the dimensions are for his target!

Note: Being able to accomplish this problem in C is excellent practice for the technical interview. 

## Instructions
### The Situation
Chesty is shooting for his annual live fire rifle qualification. He usually shoots a perfect shot every time, but today he is quite literally shooting randomly. To be precise, if his square target is a 2D grid, with x ranging from 0 to 1, and y ranging from 0 to 1; his bullet impacts at a point generated from a uniform distribution over all possible x and y values. 

After each shot, Chesty is able to see how his shot scored:
    - A perfect 5 means he hit the dead center inner circle, 
    - A 4 means he hit the second circle
    - A 3 means he hit the outer circle
    - A 2 means he put his round on the square target

Chesty sends you this image of what the target looks like (basically a bullseye), and his scoring results for the day (yes, he shot about 50000 rounds in one day, he's a motivated Marine!) 

**Target Image:**  

<p align="center">
  <img src="https://user-images.githubusercontent.com/46451845/176683769-4ccaa67b-b575-4ffd-b9ed-88599290b19f.png" />
</p>

**Scoring Results:** data.csv - One row of values ranging from 2 to 5, corresponding to his score on each shot taken. 

After a day of bad shooting, Chesty is curious as to what the dimensions of each circle on the target are. He knows that the square target is 4' by 4', but he has no idea what the size of each circle is. 

### Finding the Target Dimensions 
Using the data in data.csv, determine the radius, in inches, of each of the three concentric circles that form Chesty's target. The center of each circle is the dead center of the square target. 

The flag will be in the format flag{InnerRadius_MiddleRadius_OuterRadius}, where each radius is rounded to the nearest tenth; therefore flag{6.1_12.6_18.0} is properly formatted (but I'm pretty sure it's not the right flag).

## Scoring
As always when preparing your writeup, you will be evaluated based on the following software engineering pillars:

- Correctness (printing the flag)
- Safe from bugs (handles bad input without crashing)
- Easy to understand (commented and inuitive program structure)
- Ready for change (modular implementation)
- Adherence to the [Google C Style Guide](https://google.github.io/styleguide/cppguide.html).
