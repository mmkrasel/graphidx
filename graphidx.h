#include <stdlib.h>
#include <string.h>


char* dataset_path;
char* index_table_path;
int DIRECTION_TYPE = 0;
int RANGE = 10;
int MAX_NODE_ID = -1;
int NUM_BLOCKS = -1;
long LAST_OFFSET = -1;

long* offsets;
int* block_degs;
int** n_blocks;
struct SUB_GRAPH{
	int id;
	int len;
	int * data;
};
struct SUB_GRAPH pinnedSubGraph = {-1, 0, NULL};

void init_graphidx(int t, int r, int N, char* dp, char* ip){
	dataset_path = dp;
	index_table_path = ip;
	DIRECTION_TYPE = t;
	RANGE = r;
	MAX_NODE_ID = N;
	// printf("%d %d\n", MAX_NODE_ID, RANGE);	
	NUM_BLOCKS = MAX_NODE_ID / RANGE;
	if(NUM_BLOCKS*RANGE < MAX_NODE_ID){
		NUM_BLOCKS++;
	}
}

void construct_index_table(){
	FILE *fprAdj;
	if((fprAdj = fopen(dataset_path, "rb"))==NULL){
		printf("file open fail 1\n");
		return;
	}
	long* offsets = (long*) malloc(8*NUM_BLOCKS);
	long offset = 0;
	int maxId = -1;
	int uu, u, v, d;
	int current_block = -1, out_block, k, exists;
	if(DIRECTION_TYPE == 0){
		while(!feof(fprAdj)){
			uu = fread(&u, 4, 1, fprAdj);	// node id
			if(current_block < u/RANGE){
				current_block = u/RANGE;
				offsets[current_block] = offset;
			}
			uu = fread(&d, 4, 1, fprAdj);	// number of neighbors
			offset = offset + 2 + d;
			if( u+1 == MAX_NODE_ID){
				break;
			}
			fseek(fprAdj, offset*4, SEEK_SET);
		}
	}
	else {
		block_degs = (int*) malloc(4*NUM_BLOCKS);
		n_blocks = (int**) malloc(sizeof(int*)*NUM_BLOCKS);
		for(int i=0; i<NUM_BLOCKS; i++){
			block_degs[i] = 0;		// number of adjacent incoming blocks
		}
		while(!feof(fprAdj)){
			uu = fread(&u, 4, 1, fprAdj);	// node id
			//printf("%d ", u);
			if(current_block < u/RANGE){
				current_block = u/RANGE;
				offsets[current_block] = offset;
			}
			uu = fread(&d, 4, 1, fprAdj);	// number of neighbors
			//printf("%d ", d);

			for(int j=0; j < d; j++){		// access neighbors
				uu = fread(&v, 4, 1, fprAdj);
				//printf("%d ", v);
				out_block = v / RANGE;	
				if(current_block == out_block){
					continue;
				}
				// check if current_block is listed as an incoming block of out_block
				if(block_degs[out_block]==0){
					block_degs[out_block] = 1;
					n_blocks[out_block] = (int*)malloc(4);
					n_blocks[out_block][0] = current_block;					
				} else {
					exists = 0;
					for(k=0; k < block_degs[out_block]; k++){
						if(n_blocks[out_block][k] == current_block){
							exists = 1;
							break;
						}
						if(n_blocks[out_block][k] > current_block){
							break;
						}
					}
					if(exists == 0){
						block_degs[out_block]++;
						// printf("%d => %d\n", block_degs[out_block], k);
						n_blocks[out_block] = (int*)realloc(n_blocks[out_block], 4*block_degs[out_block]);
						if(k+1 < block_degs[out_block]){
							memcpy(&n_blocks[out_block][k+1], &n_blocks[out_block][k], 4*(block_degs[out_block]-k));
						}
						n_blocks[out_block][k] = current_block;
					}
				}
			}				
			offset = offset + 2 + d;
			//printf("\n");
			if( u+1 == MAX_NODE_ID){
				break;
			}
		}
		//printf("\n");
	}
	fclose(fprAdj);
	// for(int i=0; i<num_blocks; i++){
	// 	printf("%d %d: ", index_table[i][0], index_table[i][1]);
	// 	for(int j=0; j<index_table[i][1]; j++){
	// 		printf("%d ", index_table[i][j+2]);	
	// 	}
	// 	printf("\n--------------------------------\n");
	// }
	// printf("%d %d\n", maxId, RANGE);	
	
	// store index information
	FILE *fprIT;
	if((fprIT = fopen(index_table_path, "wb"))==NULL){
		printf("file open fail 1\n");
		return;
	}
	fwrite(&DIRECTION_TYPE, 4, 1, fprIT);
	fwrite(&RANGE, 4, 1, fprIT);
	fwrite(&MAX_NODE_ID, 4, 1, fprIT);
	fwrite(&NUM_BLOCKS, 4, 1, fprIT);
	fwrite(&offset, 8, 1, fprIT);
	fwrite(offsets, 8, NUM_BLOCKS, fprIT);
	free(offsets);
	offsets = NULL;
	if(DIRECTION_TYPE == 1){
		fwrite(block_degs, 4, NUM_BLOCKS, fprIT);
		for(int i=0; i<NUM_BLOCKS; i++){
			fwrite(n_blocks[i], 4, block_degs[i], fprIT);
			// for(int j=0; j<block_degs[i]; j++){
			// 	// printf("%d ", n_blocks[i][j]);
			// }
			// printf("\n\n");
			free(n_blocks[i]);
			n_blocks[i] = NULL;
			// break;
		}
		free(block_degs);
		block_degs = NULL;
		free(n_blocks);
		n_blocks = NULL;
	}	
	fflush(fprIT);
	fclose(fprIT);
}

void load_index_table(){
	FILE *fprIT;
	if((fprIT = fopen(index_table_path, "rb"))==NULL){
		printf("file open fail 1\n");
		return;
	}
	fread(&DIRECTION_TYPE, 4, 1, fprIT);
	fread(&RANGE, 4, 1, fprIT);
	fread(&MAX_NODE_ID, 4, 1, fprIT);
	fread(&NUM_BLOCKS, 4, 1, fprIT);
	fread(&LAST_OFFSET, 8, 1, fprIT);
	//printf("%d %d %d %d %ld\n\n", DIRECTION_TYPE, RANGE, MAX_NODE_ID, NUM_BLOCKS, LAST_OFFSET);	
	offsets = (long*) malloc(8*NUM_BLOCKS);
	fread(offsets, 8, NUM_BLOCKS, fprIT);

	if(DIRECTION_TYPE == 1){
		block_degs = (int*) malloc(4*NUM_BLOCKS);
		fread(block_degs, 4, NUM_BLOCKS, fprIT);
		n_blocks = (int**) malloc(sizeof(int*)*NUM_BLOCKS);
		for(int i=0; i<NUM_BLOCKS; i++){
			n_blocks[i] = (int*) malloc(4*block_degs[i]);
			fread(n_blocks[i], 4, block_degs[i], fprIT);
		}
	}
	// for(int i=0; i<NUM_BLOCKS; i++){
	// 	printf("%d ", i);
	// 	printf("%ld ", offsets[i]);
	// 	printf("%d ", block_degs[i]);
	// 	for(int j=0; j<block_degs[i];j++){
	// 		printf("%d ", n_blocks[i][j]);
	// 	}
	// 	printf("\n");
	// }
	// printf("\n\n");
	fclose(fprIT);
}

int* get_incoming_blocks(int v, int* num_blocks){
	int block_id = v/RANGE;
	(*num_blocks) = block_degs[block_id];
	// printf("%d %d\n", block_id, (*num_blocks));	
	int* i_block_ids = (int*) malloc(4*(*num_blocks));
	memcpy(i_block_ids, &n_blocks[block_id][0], 4*(*num_blocks));
	return i_block_ids;
}

void load_block(int block_id){
	if(pinnedSubGraph.id != block_id){
		long len;
		if(block_id+1 == NUM_BLOCKS){
			len = LAST_OFFSET - offsets[block_id];
		} else {
			len = offsets[block_id+1] - offsets[block_id];
		}
		if(pinnedSubGraph.id == -1){
			pinnedSubGraph.data = (int*)malloc(len*4);
		} else {
			pinnedSubGraph.data = (int*)realloc(pinnedSubGraph.data, len*4);
		}

		//printf("%d %ld %ld %ld %s\n", block_id, len, LAST_OFFSET, offsets[block_id], dataset_path);

		pinnedSubGraph.len = len;
		pinnedSubGraph.id = block_id;
		FILE *fprAdj;
		if((fprAdj = fopen(dataset_path, "rb"))==NULL){
			printf("file open fail @load_block\n");
			return;
		}
		fseek(fprAdj, offsets[block_id]*4, SEEK_SET);
		fread(pinnedSubGraph.data, 4, pinnedSubGraph.len, fprAdj);
		fclose(fprAdj);
	}
}

int* get_incoming_nodes_in_a_block(int to_node, int* degree, int block_id, int* neighbors){
	load_block(block_id);
	int v, d, n;
	for(int i=0; i<pinnedSubGraph.len; ){
		v = pinnedSubGraph.data[i];
		d = pinnedSubGraph.data[i+1];
		//printf("%d %d: ", v, d);
		i+=2;
		n = i + d;
		for(int j=i; j<n; j++){
			//printf("%d ", pinnedSubGraph.data[j]);
			if(pinnedSubGraph.data[j] == to_node){
				(*degree)++;
				neighbors = (int*)realloc(neighbors, 4*(*degree));
				neighbors[(*degree)-1] = v;
				//printf("%d ", v);
			}
		}
		// printf("\n");
		i+=d;
	}
	return neighbors;
}

int* get_outgoing_nodes(int from_node, int* degree){
	int* neighbors = (int*)malloc(4);
	int block_id = from_node / RANGE;
	load_block(block_id);
	for(int i=0; i<pinnedSubGraph.len; ){
		if(pinnedSubGraph.data[i] == from_node){
			neighbors = (int*)realloc(neighbors, 4*pinnedSubGraph.data[i+1]);
			memcpy(neighbors, &pinnedSubGraph.data[i+2], 4*pinnedSubGraph.data[i+1]);
			*degree = pinnedSubGraph.data[i+1];
			break;
		}
		i = i + 2 + pinnedSubGraph.data[i+1];
	}
	return neighbors;
}

int* get_incoming_nodes(int to_node, int* degree){
	if(DIRECTION_TYPE==0){
		return get_outgoing_nodes(to_node, degree);
	}
	int* neighbors = (int*)malloc(4);
	int to_block = to_node / RANGE;
	if(block_degs[to_block]==0 || to_block < n_blocks[to_block][0]){
		neighbors = get_incoming_nodes_in_a_block(to_node, degree, to_block, neighbors);
	}
	for(int k=0; k < block_degs[to_block]; k++){
		if(k>0 && k < block_degs[to_block] 
			&& to_block > n_blocks[to_block][k-1] && to_block < n_blocks[to_block][k]){
			neighbors = get_incoming_nodes_in_a_block(to_node, degree, to_block, neighbors);
		}
		neighbors = get_incoming_nodes_in_a_block(to_node, degree, n_blocks[to_block][k], neighbors);
	}
	if(to_block > n_blocks[to_block][block_degs[to_block]-1]){
		neighbors = get_incoming_nodes_in_a_block(to_node, degree, to_block, neighbors);
	}
	return neighbors;
}

int is_outneighbor(int from_node, int to_node){
	int result = 0;
	int to_block = to_node / RANGE;
	int from_block = from_node / RANGE;


	if(to_block == from_block){
		//printf("Neighbor block:%d for block:%d exists\n", from_block, to_block);
		load_block(from_block);
		int v, d, n;
		for(int i=0; i<pinnedSubGraph.len; ){
			v = pinnedSubGraph.data[i];
			d = pinnedSubGraph.data[i+1];
			if(v!=from_node){
				i+=2+d;
				continue;
			}
			// printf("%d %d: ", v, d);
			i+=2;
			n = i + d;
			for(int j=i; j<n; j++){
				// printf("%d ", pinnedSubGraph.data[j]);
				if(pinnedSubGraph.data[j] == to_node){
					return 1;
				}
			}
			// printf("\n");
			i+=d;
		}
	}

	for(int k=0; k < block_degs[to_block]; k++){
		if(n_blocks[to_block][k] > from_block){
			break;
		}
		if(n_blocks[to_block][k] == from_block){
			//printf("Neighbor block:%d for block:%d exists\n", from_block, to_block);
			load_block(from_block);
			int v, d, n;
			for(int i=0; i<pinnedSubGraph.len; ){
				v = pinnedSubGraph.data[i];
				d = pinnedSubGraph.data[i+1];
				if(v!=from_node){
					i+=2+d;
					continue;
				}
				// printf("%d %d: ", v, d);
				i+=2;
				n = i + d;
				for(int j=i; j<n; j++){
					// printf("%d ", pinnedSubGraph.data[j]);
					if(pinnedSubGraph.data[j] == to_node){
						return 1;
					}
				}
				// printf("\n");
				i+=d;
			}
			break;
		}
	}
	return 0;
}

