#include <bits/stdc++.h>
#include "graph.h"
#include "sct.h"
bool cmp_y(rank a, rank b){
    return a.r == b.r ? a.id < b.id : a.r > b.r;
}


double pava(graph *g, node **root, double *r,unsigned k){
    rank *yorder = new rank[g->n];
    unsigned *id = new unsigned[g->n];
    unsigned *rev = new unsigned[g->n];
    unsigned i;
    for(i = 0; i < g->n; i++){
        yorder[i].id = i;   
        yorder[i].r = r[i];
    }
    
    std::sort(yorder, yorder + g->n, cmp_y);
    double r_max = 0;
    double ssum = 0;
    bool flag = 0;
    for(i = 0; i < g->n; i++){
        ssum += yorder[i].r;
        if(flag == 0 && 1.0 * combiner(i + 1, k) / (i + 1) > ssum / (i + 1)){
            flag = 1;
        }
        if(flag == 0){
            r_max = std::max(r_max, 1.0 * combiner(i + 1, k) / (i + 1));
        }
        if(flag == 1){
            r_max = std::max(r_max, ssum / (i + 1));
        }
    }
    
    for(i = 0; i < g->n; i++){
        rev[yorder[i].id] = i;
        id[i] = yorder[i].id;
    }
    
    double *alpha = new double[g->n]();
    for(i = 0; i < g->n; i++) {
        if(root[i] != nullptr){
            dfs_p(root[i], yorder, 0, 0, -1, rev, alpha, k);
        }
    }

    double *y = new double[g->n]();
    for(i = 0; i < g->n; i++){
        y[i] = alpha[id[i]];
    }

    unsigned last = 0;
    double last_sum = y[0], sum = y[0];
    for(i = 1; i < g->n; i++){
        sum += y[i];
        if((double) sum / (i + 1)  >= (double) last_sum / (last + 1) ){
            last = i;
            last_sum = sum;
        }
    }

    std::cout<<"r_max =  "<<r_max<<::std::endl;
    double rho = (double) last_sum / (last + 1);
    printf("Maximum density is %.10lf\n", (double) last_sum / (last + 1));
    printf("Approxiamtion rate is %.10lf\n", r_max / (rho));
    delete[] rev;
    delete[] id;
    return r_max  / (rho);
}
