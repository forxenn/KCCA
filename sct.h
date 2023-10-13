#ifndef UTILS_SCT_H_
#define UTILS_SCT_H_


struct node{
    bool type;
    bool is_leaf;
    unsigned vertex;
    short num_child;
    node** childs;
};
node **build_sct(graph *g, int k);
void update(graph *g, node **root, unsigned k, double *r, double t, bool update_with_modify);
void dfs_p(node *u, rank *p_vertex, unsigned nump, unsigned numh, int minh, unsigned *rev, double *alpha, int k);
void count(node *u, unsigned sumh, unsigned sump, bool *choice, unsigned k, unsigned long long &res);
void init_r(graph *g, node **root, unsigned k, double *r);
double combiner(unsigned n, unsigned m);
void KPComputation(node **root, unsigned *par, unsigned *depth, unsigned k, graph *g, double & rho_lower_bound);
void CKComputation(node **root, double *ck, unsigned k, graph *g);
void update_kai(graph *g, node **root, unsigned k, double *r, double *ck, double *ck_new, unsigned *par, double *upper_bound, double rho_lower_bound);
void shuffle_order(graph *g, node** root);
void depth_order(graph *g, node** root);
#endif