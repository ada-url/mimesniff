## Amalgamation demo

While in the mimesniff main directory, using Python 3, type:

```
python singleheader/amalgamate.py
```

This will create two new files (ada_mimesniff.h and ada_mimesniff.cpp).

You can then compile the demo file as follows:

```
c++ -std=c++17 -c demo.cpp
```

Alternatively, Windows users can use

```
cl /std:c++17 singleheader/demo.cpp
```

Inside of the Developer Command Prompt.

It will produce a binary file (e.g., demo.o) which contains ada_mimesniff.cpp.
