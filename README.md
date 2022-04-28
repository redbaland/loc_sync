# LocSync

# Compiling in GCC 8+
g++ -std=c++17 sync.cpp -o sync

# Usage
If paths.cfg already exists	./sync \n
If file is different		./sync <config file name>

# Config structure

<main directory path>
<1 directory to sync path>
<2 directory to sync path>
..........................

# Result
The contents of all directories to sync will be synchronized with the main directory content
