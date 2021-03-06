TODO
====

Python
------
- Migrate to Python 3
- Install script to run Makefile if necessary
- Tracking metrics (MOT)
- Properly pass softmax output to C++

C/C++ and CUDA
--------------
- Add merge hypothesis
- Prediction class defaults to six states, make this model agnostic
- Update belief matrix using CUDA parallelisation
- Give each track a unique hash to make sure we don't overwrite IDs
- Add windows compatible __declspec(dllexport) for .DLL compilation

- Test other motion model instantiation (sp. Prediction defaults to 6 states)
- Use softmax score to weight hypothesis generation
- Output more tracking stats back to Python

- Add a fast update option that only evaluates local trajectories fully

Misc
----
- Other motion models
- Importer/Exporters for other data analysis packages
- Change the track naming convention



Updates
=======

0.2.7
-----
- Moved btrack types to seperate lib to help migration to python 3

0.2.6
-----
- Added get_motion_vector function to motion model to make predictions more
  model agnostic
- Added the ability to select which hypotheses are generated during optimization
- Added more tracking statistics to logging
- Improved track linking heuristics
- Minor bug fixes to log likelihood calculations

0.2.5
-----
- Changed default logger to work with Sequitr GPU server
- Cleaned up rendering of tracks for Jupyter notebooks
- Added time dimension to 'volume' cropping
- Added fate property to tracks

0.2.4
-----
- Returns dummy objects to HDF5 writer
- Returns parent ID from tracks to enable lineage tree creation

0.2.3
-----
- Hypothesis generation from track objects, integration of new Eigen code
- Hypothesis based track optimisation using GLPK
- Track merging moved to C++ code as part of track manager

0.2.2
-----
- HDF5 is now the default file format, for integration with conv-nets
- Tracker returns references to HDF5 groups
- Started integration of track optimiser code

0.2.1
-----
- Set limits on the volume, such that tracks which are predicted to exit the tracking volume are set to lost automatically.
- Enabled frame range in tracking to limit the range of data used
- Fast plotting of tracks
- Output a tracking statistics structure back to Python
- Track iteration to enable incremental tracking and feedback to user

0.2.0
-----
- Major update. Converted Bayesian update code to use Eigen
- Added z-dimension to tracking
