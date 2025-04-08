# SearchA
A work-in-progress vanilla implementation of the ``A*`` search algorithm in C, designed as a ``Pure Data`` external object.
The ``searchA`` object implements a ``heuristic search algorithm`` based on the ``A*`` algorithm. It enables users to load graph structures from ``.txt`` files, configure custom ``heuristics`` and ``weights``, and perform optimized searches according to different modes and criteria.

This project aims to bring artificial intelligence and heuristic search functionalities into the ``Pure Data`` environment. It provides fully configurable functionalities - with no external machine learning dependencies — designed for both technical research and real-time composition applications.

# Graph Structures Overview
The graph that can be loaded by the ``searchA`` follows the following structure:

``Edge:``
Represents a connection between two ``nodes`` in the graph. Each ``edge`` stores a set of heuristic values (e.g., ``brightness``, ``roughness``, ``harmonicity``, or any other sound/musical attributes and data) and a reference to the destination node it connects to. These values are used to calculate the cost or priority of moving from one node to another during the search.

``Node:``
Represents a point in the graph, typically corresponding to a musical or sound-related event. Each ``node`` contains its own set of heuristic values, a unique index or identifier, and a list of outgoing edges connecting it to other ``nodes``. ``Nodes`` serve as the core units visited and evaluated during the search process.

The object manages the graph structure, including all nodes and their connections, keeps track of the `search state`, stores ``heuristic weights`` (to prioritize certain attributes), and controls the ``input/output`` behavior via outlets. It’s responsible for executing the ``A*`` search algorithm and returning the best path according to the configured ``mode`` and ``weights``.

### 📈 Graph Structure example (Simplified Diagram) 

Each line in the ``.txt`` file represents a node in the graph. The format is:

``<node_heuristics> -> <connected_node_1_heuristics> <connected_node_2_heuristics> ...``

Heuristic values are space-separated, and ``->`` indicates the direction to connected nodes.

### 🔢 Sample .txt Entry
``0.00123164 0.16744 0.560182 -> 0.0149622 0.44435 0.5078 -> 0.065612 0.428811 0.677328``

### 🖼️ Visual Representation
``node`` A is connected to ``node`` B and C:

```
Node A: (0.00123164 0.16744 0.560182)
   |
   |--> Node B: (0.0149622 0.44435 0.5078)
   |
   |--> Node C: (0.065612 0.428811 0.677328)

```
This shows how a single node can lead to multiple destinations, forming a directed ``graph`` structure that the `A*` algorithm will traverse.
The search process will evaluate different paths in this graph, based on the weights and the configured search mode (e.g., ``lowest cost`` or ``highest cost``), to find the optimal route from the current node to a target.



### 🔧 Commands

- **`read <filename>`**  
  Loads a graph from a `.txt` file. Each line defines a ``node``, its heuristic values, and its outgoing connections. This initializes the internal ``graph`` structure.

- **List input**  
  Triggers the `A*` search algorithm using the current ``graph``, ``weighting``, and ``search mode``. The resulting path is output through the object's outlets. This is done by sending a list of features (``heuristics``) to the object.

- **`restart`**  
  Resets the internal ``search state``. Clears all previously visited ``nodes`` and accumulated costs, allowing a new search to be performed without reloading the ``graph``.

- **`mode < < | >`**  
  Sets the ``search mode``. Use `<` to search for the ``lowest-cost`` path or `>` to search for the ``highest-cost`` path. This can adapt the search logic for analytical or creative use cases.

- **`weighting <w1> <w2> <w3>`**  
  Sets the relative importance (``weights``) of the three ``heuristic`` dimensions used by the algorithm. This allows fine-tuning of how each attribute influences the pathfinding.



# Build
> [!NOTE]
`searchA` uses `[pd.cmake](https://github.com/pure-data/pd.cmake). To build the external on Linux, Mac, and Windows (using Mingw64):

1. `git clone https://github.com/oviniciuscesar/searchA-Star/ --recursive`;
2. `cd cnn2d`;
4. `cmake . -B build`;
5. `cmake --build build`;
