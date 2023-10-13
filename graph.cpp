#include <bits/stdc++.h>
#include "graph.h"

graph* new_graph() {
    graph *g = new graph();
    g->n = g->m = 0;
    g->edges = nullptr;
    g->adj = nullptr;
    g->sadj = nullptr;

    return g;
}

graph* read_graph(char *fp) {
    graph *g = new_graph();
    FILE *file = fopen(fp,"r");
    unsigned i;

    fscanf(file, "%u %u", &(g->n), &(g->m));
    g->edges = new edge[g->m]; 
    for(i = 0; i < g->m; i++) {
        fscanf(file, "%u %u", &(g->edges[i].s),&(g->edges[i].t));
        //assert(g->edges[i].s < g->n && g->edges[i].s >= 0);
        //assert(g->edges[i].t < g->n && g->edges[i].t >= 0);
    }

    fclose(file);
    return g;
}
bool e_cmp(edge a, edge b){
    return a.s == b.s ? a.t > b.t : a.s > b.s; 
}
void core_order(graph *g, unsigned k) {
    unsigned i, u, tmp;
    int head = 0,tail = -1;

    unsigned *deg = new unsigned[g->n]();
    unsigned *adj = new unsigned[g->m << 1];
    unsigned *sadj = new unsigned[g->n + 1]();
    unsigned *queue = new unsigned[g->n];
    unsigned *rev = new unsigned[g->n];
    unsigned *count = new unsigned[g->n]();
    char *remove = new char[g->n]();

    //unsigned max_deg = 0;
    for(i = 0; i < g->m; i++) {
        deg[g->edges[i].s]++;
        deg[g->edges[i].t]++;
    }
    for(i = 0; i < g->n; i++) {
        sadj[i + 1] = sadj[i] + deg[i];
    //    max_deg = std::max(max_deg,deg[i]);
        deg[i] = 0;
    }
    //printf("max deg %u\n",max_deg);
    for(i = 0; i < g->m; i++) {
        adj[sadj[g->edges[i].s] + deg[g->edges[i].s]++] = g->edges[i].t;
        adj[sadj[g->edges[i].t] + deg[g->edges[i].t]++] = g->edges[i].s;
    }

    //(k - 1) core decomposition
    k -= 2;
    for(int i = 0; i < g->n; i++){
        if(deg[i] <= k) {
            queue[++tail] = i;
        }
    }     
    while(head <= tail){
        u = queue[head++];
        remove[u] = 1;
        for(i = sadj[u]; i < sadj[u + 1]; i++) {
            if((--deg[adj[i]]) == k) {
                queue[++tail] = adj[i];
            }
        }
    }
    // building core ordered graph
    tmp = 0;
    for(i = 0; i < g->n; i++) {
        if(remove[i]) continue;
        deg[tmp] = deg[i];
        count[deg[tmp]]++;
        rev[i] = tmp++;
    }
    g->n = tmp;
    unsigned *rev_new = new unsigned[g->n];    
    for(int i = 1; i < g->n; i++){
        count[i] += count[i - 1];
    }
    for(int i = 0; i < g->n; i++){
        rev_new[i] = --count[deg[i]];
        deg[i] = 0;
    }
    tmp = 0;
    for(i = 0; i < g->m; i++){
        if(remove[g->edges[i].s] || remove[g->edges[i].t]) continue;
        tmp++;
    }
    edge *edges_new = new edge[tmp];
    tmp = 0;
    for(i = 0; i < g->m; i++){
        if(remove[g->edges[i].s] || remove[g->edges[i].t]) continue;
        g->edges[i].s = rev_new[rev[g->edges[i].s]];
        g->edges[i].t = rev_new[rev[g->edges[i].t]];
        if(g->edges[i].s > g->edges[i].t){
            g->edges[i].t ^= g->edges[i].s;
            g->edges[i].s ^= g->edges[i].t;
            g->edges[i].t ^= g->edges[i].s; 
        }
        edges_new[tmp++] = g->edges[i];
        deg[g->edges[i].s]++;
    }
    g->m = tmp;
    delete[] g->edges;
    g->edges = edges_new;
    std::sort(g->edges, g->edges + g->m, e_cmp);
    g->sadj = new unsigned[g->n + 1]();
    g->adj = new unsigned[g->m];
    unsigned max_deg = 0;
    for(i = 0; i < g->n; i++) {
        g->sadj[i + 1] = g->sadj[i] + deg[i];
        max_deg = std::max(max_deg,deg[i]);
        deg[i] = 0;
    }
    printf("max deg = %u\n",max_deg);
    for(i = 0; i < g->m; i++) {
        g->adj[g->sadj[g->edges[i].s] + deg[g->edges[i].s]++] = g->edges[i].t;
    }
    
    delete[] deg;
    delete[] adj;
    delete[] sadj;
    delete[] queue;
    delete[] rev;
    delete[] count;
    delete[] remove;
}

graph * build_graph(char *fp, int k) {
    printf("Reading graph from file %s\n", fp);
    graph *g = read_graph(fp);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    printf("Graph size |V| = %u, |E| = %u\n", g->n, g->m);
    printf("Building graph structure\n");

    core_order(g, k);
    printf("Building finished\n");
    printf("Graph size after decompostion |V| = %u, |E| = %u\n", g->n, g->m);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Running time for building graphs: " << duration.count() << " milliseconds." << std::endl;
    
    return g;
}
bool cmp(rank x, rank y){
    return x.r == y.r ? x.id < y.id : x.r < y.r;
}
void reorder(graph *g, double *r) {
    int i;
    
    rank *rorder = new rank[g->n];
    unsigned *rev = new unsigned[g->n];
    unsigned *deg = new unsigned[g->n]();

    for(i = 0; i < g->n ; i++) {
        rorder[i].id = i;
        rorder[i].r = r[i];
    }

    unsigned max_deg = 0;
    std::sort(rorder, rorder + g->n, cmp);
    for(i = 0; i < g->n ; i++) {
        rev[rorder[i].id] = i;
    }
    for(i = 0; i < g->m; i++) {
        if(rev[g->edges[i].s] > rev[g->edges[i].t]) {
            g->edges[i].s ^= g->edges[i].t;
            g->edges[i].t ^= g->edges[i].s;
            g->edges[i].s ^= g->edges[i].t;
        }
        deg[g->edges[i].s]++;
    }   

    for(i = 0; i < g->n; i++) {
        g->sadj[i + 1] = g->sadj[i] + deg[i];
        max_deg = std::max(max_deg,deg[i]);
        deg[i] = 0;
    }
    for(i = 0; i < g->m; i++) {
        g->adj[g->sadj[g->edges[i].s] + deg[g->edges[i].s]++] = g->edges[i].t;
    }

    printf("max deg = %u\n",max_deg);

    delete[] rorder;
    delete[] rev;
    delete[] deg;
    

}