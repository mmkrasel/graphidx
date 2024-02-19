# GraphIdx
An Efficient Indexing Technique for Accelerating Graph Data Mining. It is an open-source C library that facilitates a memory efficient and on-the-fly indexing of large graphs. Many graph mining algorithms process large graphs with several passes to obtain the final results that suffers from huge I/O cost. GraphIdx indexes a block of graph data for a set of nodes based on the empirical evaluation of edges. Due to the indexed graph data, graph mining algorithms can access and process only the related nodes and their edges instead of a complete scan of the graph. Since GraphIdx enables graph mining algorithms to access only related data, the number of I/Os is significantly reduced. Moreover, GraphIdx accredits many graph mining algorithms can process data in parallel due to the indexed graph data.

# Usages

...
#include "graphidx.h"
...

...
  
## Configure GraphIdx 
/* Graph dataset information
 - Represented using adjacency list
 - Data in binary format */
   
  char* dataset_path = "....";
  char* index_table_path = "....";
  int t = 1;    // Graph type: 0=undirected, 1=directed
  int r = ...;  // #nodes in a subgraph
  int N = ...;  // Max node id in the graph
  init_graphidx(t, r, N, dataset_path, index_table_path);
  
...

...
  
## Construct Index Table
  construct_index_table();
  
  ...
  
  ...
  
## Load Index Table from disk to memory
  load_index_table();
  
  ...

  ...
  
## Get incoming block ids for a given node
  int v = ...;
  int num_blocks = 0;
  int* i_block_ids = get_incoming_blocks(v, &num_blocks);
  
  ...

  ...
  
## Get incoming node ids to a given node
  int to_node = ...;
  int in_degree = 0;
  int* i_node_ids = get_incoming_nodes(to_node, &in_degree); 
  
  ...

  ...
  
## Get outgoing node ids from a given node
  int from_node = ...;
  int out_degree = 0;
  int* o_node_ids = get_outgoing_nodes(from_node, &out_degree);
  
  ...

  ...
  
## Probe that node 'u' is a source of an incoming edge to node v
  /* Return result: 0=Negative, 1=Positive */
  int u = ...;
  int v = ...;
  int result = is_outneighbor(u, v);
  
  ...

  ...

##
