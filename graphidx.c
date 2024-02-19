//COMPILE: gcc graphidx.c -o graphidx 
//RUN: ./graphidx

#include <stdio.h>
#include "graphidx.h"


void create_directed_graph(){
	int graph[24][4] = {
		{2, 7, 16},
		{2, 0, 15},
		{2, 3, 6},
		{1, 0},
		{2, 0, 1},
		{1, 7},
		{2, 7, 17},
		{2, 3, 9},
		{2, 9, 12},
		{2, 6, 18},
		{2, 5, 7},
		{1, 10},
		{1, 14},
		{2, 11, 12},
		{2, 13, 21},
		{1, 16},
		{0},
		{1, 19},
		{2, 16, 17},
		{1, 1},
		{3, 4, 21, 23},
		{2, 12, 22},
		{1, 20},
		{1, 2}
	};
	FILE* fpr = fopen("example_directed_graph.bin", "wb");

	for(int i=0; i<24; i++){
		fwrite(&i, 4, 1, fpr);
		fwrite(&graph[i][0], 4, 1, fpr);
		// printf("%d ", i);
		// printf("%d ", graph[i][0]);
		for(int j=1; j<=graph[i][0]; j++){
			fwrite(&graph[i][j], 4, 1, fpr);
			// printf("%d ", graph[i][j]);
		}
		// printf("\n");
	}
	// printf("\n");
	fflush(fpr);
	fclose(fpr);
}
void create_undirected_graph(){
	int graph[24][7] = {
		{5, 1, 3, 4, 7, 16},
		{4, 0, 4, 15, 19},
		{3, 3, 6, 23},
		{3, 0, 2, 7},
		{3, 0, 1, 20},
		{2, 7, 10},
		{4, 2, 7, 9, 17},
		{6, 0, 3, 5, 6, 9, 10},
		{2, 9, 12},
		{4, 6, 7, 8, 18},
		{3, 5, 7, 11},
		{2, 10, 13},
		{4, 8, 13, 14, 21},
		{3, 11, 12, 14},
		{3, 12, 13, 21},
		{2, 1, 16},
		{3, 0, 15, 18},
		{3, 6, 18, 19},
		{3, 9, 16, 17},
		{2, 1, 17},
		{4, 4, 21, 22, 23},
		{4, 12, 14, 20, 22},
		{2, 20, 21},
		{2, 2, 20}
	};
	FILE* fpr = fopen("example_undirected_graph.bin", "wb");

	for(int i=0; i<24; i++){
		fwrite(&i, 4, 1, fpr);
		fwrite(&graph[i][0], 4, 1, fpr);
		// printf("%d ", i);
		// printf("%d ", graph[i][0]);
		for(int j=1; j<=graph[i][0]; j++){
			fwrite(&graph[i][j], 4, 1, fpr);
			// printf("%d ", graph[i][j]);
		}
		// printf("\n");
	}
	// printf("\n");
	fflush(fpr);
	fclose(fpr);
}



int main() {

	/* You may test the implementation by creating customized graph */
	// create_directed_graph();
	// create_undirected_graph();

	/* Configure GraphIdx
		Graph dataset information
		- Represented using adjacency list
		- Data in binary format  */
	// char* dataset_path = "example_undirected_graph.bin";
	// char* index_table_path = "example_undirected_graphidx.bin";
	char* dataset_path = "example_directed_graph.bin";
	char* index_table_path = "example_directed_graphidx.bin";
	int t = 1;    // Graph type: 0=undirected, 1=directed
	int r = 5;  // #nodes in a subgraph
	int N = 24;  // Number of nodes in the graph

	init_graphidx(t, r, N, dataset_path, index_table_path);
	
	/* Construct Index Table*/
	construct_index_table();
	printf("Index-construction is completed\n\n");
	
	/* Load Index Table from disk to memory */
	load_index_table();
	printf("Index-reading is completed\n\n");	
	
	// /* Get incoming block ids for a given node */
	int v = 2;
	int num_blocks;
	int* i_block_ids = get_incoming_blocks(v, &num_blocks);
	printf("Block for node:%d has %d incoming blocks. They are: ", v, num_blocks);
	for(int i=0; i<num_blocks; i++){
		printf("%d ", i_block_ids[i]);
	}
	printf("\n\n");


	/* Get incoming node ids for a given node */
	int to_node = 12;
	int in_degree = 0;
	int* i_node_ids = get_incoming_nodes(to_node, &in_degree);
	printf("Node:%d has %d incoming nodes. They are: ", to_node, in_degree);
	for(int i=0; i<in_degree; i++){
		printf("%d ", i_node_ids[i]);
	}
	printf("\n\n");


	/* Get outgoing node ids for a given node */
	int from_node = 0;
	int out_degree = 0;
	int* o_node_ids = get_outgoing_nodes(from_node, &out_degree);
	printf("Node:%d has %d outgoing nodes: They are: ", from_node, out_degree);
	for(int i=0; i<out_degree; i++){
		printf("%d ", o_node_ids[i]);
	}
	printf("\n\n");


	/* Show all outgoing nodes 
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


	/* Show all incoming nodes 
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
	printf("\n\n");
}

