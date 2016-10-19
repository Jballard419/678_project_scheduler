
#ifndef NODE_H
#define NODE_H

typedef struct node_t {
  void *value;
  node_t *lower_node;

  /* data */
}node_t;


void node_int(node_t *n, void *v) {
  /* code */
  n->value= *v;
  n->lower_node=NULL;
}

#endif
