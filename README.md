# Tank 2017

[![Build Status](https://travis-ci.com/NYUCG2017/assignment-4-majia67.svg?token=fnyrhqk4XSU6oLqKs1pL&branch=master)](https://travis-ci.com/NYUCG2017/assignment-4-majia67)

An OpenGL version Tank 1991

Author: Yicong Tao (yt1182@nyu.edu)

### Game Objective
User should protect the home from being destroyed by the enemy tanks. If all enemy tanks are destroyed, the user wins.

### Control
Move: Up/Down/Left/Right

Fire: Space

### Implementation Details
+ Map blocks, tanks & bullets: rectangles specified by the upper left and lower right 
 corners, and generated on-the-fly in the geometry shader
+ Texture mapping: one aggregate texture image with predefined uv indices
+ Collision detection: using regular grid
+ Relative position with sea and forest: doing depth test
+ Control: using sticky keys instead of key callback

### Future works
+ Adding 3D mode (possible solution: ortho/perspective control, 3d regular grid collision detection, 3d texture mapping)
+ Adding items that can change or improve tank abilities (such as increasing the speed, firing more than one bullet at the same time, etc.)
+ Adding more maps and map switching
+ Adding map editor
