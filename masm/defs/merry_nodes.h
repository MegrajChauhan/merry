#ifndef _MERRY_NODES_
#define _MERRY_NODES_

#include <merry_types.h>
#include <merry_node_types.h>
#include <merry_extra_defins.h>

typedef struct Node Node;

struct Node {
	node_t type;
	msize_t lnum;
	union {
		struct {
		  	datatype_t data_type;
		  	union {
		  		mqword_t int_val;
		  		double dec_val;
		  	} value;
		  	mstr_t name_st, name_ed; // just storing pointers
		} var_defin;

		struct {
			datatype_t data_type;
			mptr_t buffer;
			msize_t buffer_len; // we don't care about individual element length
			mstr_t name_st, name_ed;
		} var_array_defin;
	};
};

#endif
