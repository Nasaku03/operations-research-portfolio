# Linear Integer Optimization over MOQIP

## Overview

This project contains research codes associated with multiobjective optimization methods such as:
- [Boland et al.](https://10.1016/j.ejor.2016.02.037) method
- [Ouaïl et al.](https://doi.org/10.1007/s10479-017-2698-6) method
- [Badjara et al.](https://doi.org/10.1080/01605682.2024.2416510) method
- ...

All the implemented methods are in C++ compiling with VS 2022 or newer. Some solvers may be used like CPLEX and GAMS under the academic licenses. 

## Research Focus

The project concerns optimization methods for multiobjective integer optimization. We target:

- the multiobjective integer linear problems (MOILP), the multiobjective integer quadratic problems (MOIQP), the optimization of a linear function over the efficient set of a MOILP or a MOIQP;
- the papers introduce the [efficient cut of Chergui](https://doi.org/10.1007/978-3-540-87477-5_8) and the [generalized efficient cut](https://doi.org/10.1080/01605682.2024.2416510) using the epsilon parameter of Badjara and Chergui.
- CPLEX is used for some steps that use an integer linear mono-objective problem like [the efficient check of Benson](https://doi.org/10.1007/BF00933152).
- the stopping criteria is when all efficient solutions are found, but if a time-out limit is reached (generally after 2 hours) then the computation is automatically stopped. The results are collected in a text file.

## Requirements

- Microsoft Visual Studio 2022 or newer
- Desktop development with C++ workload
- IBM ILOG CPLEX and/or GAMS only if required by the implementation

## Build and Run

1. Open the relevant `.sln` or `.vcxproj` file in Visual Studio.
2. Verify the solver version, library paths, and license requirements, if applicable.
3. Select `Debug`.
4. Build the project.
5. Run it with input data in the format expected by the implementation. The input format may differ between projects.

## Contents

This directory contains the implementation and project files associated with this optimization method. See the filenames and source comments for the implemented variants.

...
