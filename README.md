# GraphIdx
An Efficient Indexing Technique for Accelerating Graph Data Mining. It is an open-source C library that facilitates a memory efficient and on-the-fly indexing of large graphs. Many graph mining algorithms process large graphs with several passes to obtain the final results that suffers from huge I/O cost. GraphIdx indexes a block of graph data for a set of nodes based on the empirical evaluation of edges. Due to the indexed graph data, graph mining algorithms can access and process only the related nodes and their edges instead of a complete scan of the graph. Since GraphIdx enables graph mining algorithms to access only related data, the number of I/Os is significantly reduced. Moreover, GraphIdx accredits many graph mining algorithms can process data in parallel due to the indexed graph data.

# Usages
```
#include "graphidx.h"
```
  
## Configure GraphIdx
```
/* Graph dataset information
 - Represented using adjacency list
 - Data in binary format */
   
  char* dataset_path = "....";
  char* index_table_path = "....";
  int t = 1;    // Graph type: 0=undirected, 1=directed
  int r = ...;  // #nodes in a subgraph
  int N = ...;  // Max node id in the graph
  init_graphidx(t, r, N, dataset_path, index_table_path);
```
  
## Construct Index Table
```
  construct_index_table();
```

## Load Index Table from disk to memory
```
  load_index_table();
```
  
## Get incoming block ids for a given node
```
  /* Only use for a directed graph */
  int v = ...;
  int num_blocks = 0;
  int* i_block_ids = get_incoming_blocks(v, &num_blocks);
```
  
## Get incoming node ids to a given node
```
  /* All neighbor nodes will return incase of an undirected graph */
  int to_node = ...;
  int in_degree = 0;
  int* i_node_ids = get_incoming_nodes(to_node, &in_degree); 
```
  
## Get outgoing node ids from a given node
```
  /* All neighbor nodes will return incase of an undirected graph */
  int from_node = ...;
  int out_degree = 0;
  int* o_node_ids = get_outgoing_nodes(from_node, &out_degree);
```
  
## Probe that node 'u' is a source of an incoming edge to node v
```
  /* Only use for a directed graph */
  /* Return result: 0=Negative, 1=Positive */
  int u = ...;
  int v = ...;
  int result = is_outneighbor(u, v);  
```

## Show all outgoing nodes 
```
  /*
	   - First column shows the from node id
	   - Second column shows the value of out-degree
	   - Other columns list the outgoing node ids */

	printf("Show all outgoing nodes of all nodes.\n");
	for(int node = 0; node < N; node++){
		int num_nodes = 0; 
		int* o_node_ids = get_outgoing_nodes(node, &num_nodes);
		printf("%d %d ", node, num_nodes);
		for(int i=0; i<num_nodes; i++){
			printf("%d ", o_node_ids[i]);
		}
		printf("\n");
	}
	printf("\n\n");
```

## Show all incoming nodes 
```
  /*
	   - First column shows the to node id
	   - Second column shows the value of in-degree
	   - Other columns list the incoming node ids */

	printf("Show all incoming nodes of all nodes.\n");
	for(int node = 0; node<N; node++){
		int num_nodes = 0;
		int* i_node_ids = get_incoming_nodes(node, &num_nodes);
		printf("%d %d ", node, num_nodes);
		for(int i=0; i<num_nodes; i++){
			printf("%d ", i_node_ids[i]);
		}
		printf("\n");
		free(i_node_ids);
		i_node_ids = NULL;
	}
```

##
