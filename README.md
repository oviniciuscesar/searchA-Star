# searchA-Star
Work-in-progress vanilla implementation of  ``search A star`` algorithm in `C` as ``Pure Data`` object.
The ``searchA`` object implements a heuristic search algorithm based on `A* algorithm` and it allows reading a graph structure loaded from text files, configuring heuristics and weights, and executing optimized searches according to different modes and criteria.

The project aims to integrate machine learning and artificial intelligence algorithms into the `Pure Data` environment and to provide configurable functionalities (with no external machine learning dependencies) designed for technical studies and real-time composition applications.



# Data Structures Overview
``Edge:``
Represents a connection between two nodes in the graph. Each edge stores a set of heuristic values (e.g., ``brightness``, ``roughness``, ``harmonicity``, etc.) and a reference to the destination node it connects to. These values are used to calculate the cost or priority of moving from one node to another during the search.

``Node:``
Represents a point in the graph, typically corresponding to a musical or sound-related event. Each node contains its own set of heuristic values, a unique index or identifier, and a list of outgoing edges connecting it to other nodes. Nodes serve as the core units visited and evaluated during the search process.

``Search Object:``
This is the main object used in Pure Data. It manages the graph structure, including all nodes and their connections, keeps track of the search state, stores heuristic weights (to prioritize certain attributes), and controls the input/output behavior via Pd outlets. It’s responsible for executing the A* search algorithm and returning the best path according to the configured mode and weights.

### 📈 Graph Structure example (Simplified Diagram) 

Each line in the ``.txt`` file represents a node in the graph. The format is:

``<node_heuristics> -> <connected_node_1_heuristics> <connected_node_2_heuristics> ...``

Heuristic values are space-separated, and ``->`` indicates the direction to connected nodes.

### 🔢 Sample .txt Entry
``0.00123164 0.16744 0.560182 -> 0.0149622 0.44435 0.5078 0.065612 0.428811 0.677328``


```Node 0: (0.0012, 0.1674, 0.5601)
   |
   |--> Node 1: (0.0150, 0.4443, 0.5078)
   |      |
   |      |--> Node 2: (0.0006, 0.4465, 0.1239)
   |             |
   |             |--> Node 3: (0.1123, 0.2839, 0.2780)
   |                    |
   |                    |--> Node 4: (0.0002, 0.4660, 0.0317)
   |                           |
   |                           |--> Node 5: (0.0010, 0.6099, 0.3229)
   |                                  |
   |                                  |--> Node 6: (0.3090, 0.4109, 0.3233)
```



### 🧠 Key Functions

liberar_grafo: Frees the memory allocated for the graph structure.

searchA_pond: Receives a list of three values and stores them as heuristic weights.

set_mode: Sets the search mode (< for lowest cost, > for highest cost).

searchA_read: Loads a graph structure from a .txt file, initializing nodes and edges.

searchA_bang: Executes the A* search based on the configured heuristics, weights, and search mode, and outputs the results via Pd outlets.

searchA_restart: Resets the search state, clearing costs and visited nodes.

searchA_new: Constructor function for the object; initializes outlets, default values, and canvas context.

searchA_destroy: Destructor function; frees memory and removes outlets.

searchA_setup: Registers the object and its methods in the Pure Data environment.


# Build
> [!NOTE]
`searchA` uses `pd.build`. To build the external on Linux, Mac, and Windows (using Mingw64):

1. `git clone https://github.com/oviniciuscesar/searchA-Star/ --recursive`;
2. `cd cnn2d`;
4. `cmake . -B build`;
5. `cmake --build build`;
