# SearchA
Work-in-progress vanilla implementation of  ``search A star`` algorithm in `C` as a ``Pure Data`` object.
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
``0.00123164 0.16744 0.560182 -> 0.0149622 0.44435 0.5078 -> 0.065612 0.428811 0.677328``

# Node A is connected to Node B and C

```
Node A: (0.00123164 0.16744 0.560182)
   |
   |--> Node B: (0.0149622 0.44435 0.5078)
   |
   |--> Node C: (0.065612 0.428811 0.677328)

```

The search process will evaluate different paths in this graph, based on the weights and the configured search mode (e.g., ``lowest cost`` or ``highest cost``), to find the optimal route from the current node to a target.



### 🔧 Commands

``read <filename>``
Loads a graph from a ``.txt`` file. Each line in the file represents a node, its heuristic values, and its outgoing connections. This command initializes the internal graph structure used by the search.

``List input`` 
Triggers the ``A*`` search algorithm using the current ``graph``, ``weighting``, and ``search mode``. The resulting path is output through the outlets. This is done by sending a list of features (heuristic).

``restart``
Resets the internal state of the search. This clears all previously visited nodes and accumulated costs, allowing the user to perform a new search without reloading the ``graph``.

``mode``
Sets the ``search mode``. Use ``<`` to search for the ``lowest cost`` path, or ``>`` to search for the ``highest cost path``. This allows the algorithm to be adapted for different creative or analytical goals.

``weighting <w1> <w2> <w3>``
Sets the relative importance (``weights``) of the three ``heuristic dimensions`` used by the ``A*`` algorithm. This lets you guide the search behavior based on the criteria you define.


# Build
> [!NOTE]
`searchA` uses `pd.build`. To build the external on Linux, Mac, and Windows (using Mingw64):

1. `git clone https://github.com/oviniciuscesar/searchA-Star/ --recursive`;
2. `cd cnn2d`;
4. `cmake . -B build`;
5. `cmake --build build`;
