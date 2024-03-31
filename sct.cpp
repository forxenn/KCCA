#include <bits/stdc++.h>
#include "graph.h"
#include "sct.h"

const int inf = 1e9;
struct element{
    int id,nxt;
    element(){}
};

struct hash_table{
    int cnt,size;
    int* head;
    element* hash;
    hash_table(){}
    hash_table(int n){
        size = n;
        cnt = 0;
        head = new int[n]();
        for(int i = 0; i < n; i++) head[i] = inf;
        hash = new element[n](); 
    }
    ~hash_table(){
        delete []head;
        delete []hash;
    }
    void insert(int value){
        int k = value % size;
        hash[cnt].id = value;
        hash[cnt].nxt = head[k];
        head[k] = cnt;
        cnt++;
    }
    bool find(int value){
        int k = value % size;
        for(int i = head[k]; i != inf; i = hash[i].nxt){
            if(hash[i].id == value) return true;
        }
        return false;
    }
};

struct queue_for_sct{
    node *u;
    unsigned* s;
    unsigned up_size;
    unsigned sumh;
    unsigned depth;
    queue_for_sct(){}
    queue_for_sct(node *uu,unsigned *ss,unsigned uup_size,unsigned sumh_, unsigned depth_){
        u = uu;
        s = ss;
        up_size = uup_size;
        sumh = sumh_;
        depth = depth_;
    }
};

node *creat_node(bool type, unsigned vertex){
    node *res = new node();
    res->type = type;
    res->vertex = vertex;
    res->is_leaf = false;
    res->num_child = 0;
    res->childs = nullptr;
    return res;
}
double cnm[1000][1000];
void init_c(){
    unsigned i, j;
    for(i = 0; i < 1000; i++){
        cnm[i][0] = 1;
        for(j = 1; j <= i; j++){
            cnm[i][j] = cnm[i - 1][j - 1] + cnm[i - 1][j];
        }
    }
}
node **build_sct(graph *g, int k){
    init_c();
    node** root = new node*[g->n];
    unsigned* rev = new unsigned[g->n];
    
    hash_table **mp = new hash_table*[g->n];
    unsigned *deg = new unsigned[g->n]();
    unsigned max_kcore = 0;
    unsigned i;

    // building edge hash tables
    for(i = 0; i < g->m; i++){
        deg[g->edges[i].s]++;
        deg[g->edges[i].t]++;
    }
    for(i = 0; i < g->n; i++){
        mp[i] = new hash_table(deg[i]);
        max_kcore = std::max(max_kcore,deg[i]);
    }
    for(i = 0; i < g->m; i++){
       mp[g->edges[i].s]->insert(g->edges[i].t);
       mp[g->edges[i].t]->insert(g->edges[i].s);
    }

    // creating adj matrix and queue
    bool** new_edge = new bool*[max_kcore]();
    unsigned* name = new unsigned[max_kcore]();
    for(int i = 0; i < max_kcore; i++){
        new_edge[i] = new bool[max_kcore]();
    }
    unsigned queue_size =  std::max((unsigned) 1000000, g->m); 
    int head = 0, tail = queue_size - 1;
    queue_for_sct **q = new queue_for_sct*[queue_size]; // need to check if it is indeed m.


    // iterating for each node
    unsigned uu, j, count, uj, vj;
    for(uu = 0;uu < g->n; uu++){
        if(uu % 100000 == 0) std::cout<< "Building sct for vertex " << uu << std::endl;
        count = 0;
        for(j = g->sadj[uu]; j < g->sadj[uu + 1]; j++){
            name[count] = g->adj[j];
            rev[g->adj[j]] = count++;
        }
        for(uj = g->sadj[uu]; uj < g->sadj[uu + 1]; uj++){
            for(vj = g->sadj[uu]; vj < g->sadj[uu + 1]; vj++){
                new_edge[rev[g->adj[uj]]][rev[g->adj[vj]]] = mp[g->adj[uj]]->find(g->adj[vj]);
            }
        }
        if(count + 1 < k){
            root[uu] = nullptr;
            continue;
        }
        
        // build root vertex
        root[uu] = creat_node(1, uu);
        unsigned *ss = new unsigned[count];
        for(i = 0; i <count; i++) ss[i] = i;
        (++tail) %= queue_size;
        delete q[tail];
        q[tail] = new queue_for_sct(root[uu],ss,count,1,1);
        
        // pivot
        while(head != (tail + 1) % queue_size){
            
            node *u = q[head]->u;
            unsigned* s = q[head]->s;
            unsigned up_size = q[head]->up_size;
            unsigned sumh = q[head]->sumh;
            unsigned depth = q[head]->depth;
            head = (head + 1) % queue_size;
            if(up_size == 0){
                delete[] s;
                u->is_leaf = 1;
                continue;
            }

            // finding pivot
            int p = -1, value = -1;
            for(i = 0; i < up_size; i++){
                int tmpp = s[i];
                int tmpv = 0;
                for(int j = 0; j < up_size; j++){
                    if(new_edge[s[i]][s[j]]) tmpv++;
                }
                if(tmpv > value){
                    p = tmpp;
                    value = tmpv;
                }
            }
            
            
            node *childp = creat_node(0, name[p]); 
            u->num_child++;

            unsigned *sp = new unsigned[value];
            unsigned *vlist = new unsigned[up_size - value - 1];
            unsigned sp_size = 0;
            unsigned vlist_size = 0;
            for(i = 0; i < up_size; i++){ 
                if(new_edge[p][s[i]]) sp[sp_size++] = s[i];
                else{
                    if(s[i] != p){
                        vlist[vlist_size++] = s[i];
                        u->num_child++;
                    }
                }
            }
            if(depth + sp_size + 1>= k){
                (++tail) %= queue_size;
                delete q[tail];
                q[tail] = new queue_for_sct(childp, sp, sp_size, sumh, depth + 1);
            }
            // bound for k
            if(sumh == k){
                u->num_child = 1;
                u->childs = new node*[1];
                u->childs[0] = childp;
            }

            else{

                u->childs = new node*[u->num_child];
                u->childs[0] = childp;
                unsigned tmp = 0;
                hash_table *del_v = new hash_table(vlist_size);
                
                for(i = 0; i < vlist_size; i++){
                    node *childh = creat_node(1, name[vlist[i]]);
                    u->childs[++tmp] = childh;
                    unsigned sh_size = 0;
                    for(j = 0; j < up_size; j++){
                        if(del_v->find(s[j])) continue;
                        if(new_edge[vlist[i]][s[j]]) sh_size++;
                    }
                    unsigned *sh = new unsigned[sh_size];
                    sh_size = 0;
                    for(int j = 0; j < up_size; j++){
                        if(del_v->find(s[j])) continue;
                        if(new_edge[vlist[i]][s[j]]) sh[sh_size++] = s[j];
                    }
                    if(depth + sh_size + 1>= k){
                        (++tail) %= queue_size;
                        delete q[tail];
                        q[tail] = new queue_for_sct(childh, sh, sh_size, sumh + 1, depth + 1);
                    }
                    del_v->insert(vlist[i]);
                }
                delete del_v;
            }
            delete []vlist;
            delete []s;
        }
    }
    for(int i = 0; i < max_kcore; i++) delete []new_edge[i];
    for(int i = 0; i < g->n; i++) delete mp[i];
    delete []mp;
    for(int i = 0; i < queue_size; i++) delete q[i];
    delete []q;
    delete []new_edge;
    delete []rev;
    delete []name;
    delete []deg;
    return root;
}
bool new_cmp(rank x, rank y){
    return x.r == y.r ? x.id < y.id : x.r < y.r;
}
void dfs(node *u, unsigned *p_vertex, unsigned *h_vertex, unsigned nump, unsigned numh, double *r, unsigned k, double gamma, double *rpast, bool update_with_modify){
    if(u->type){
        h_vertex[numh++] = u->vertex;
    }
    else{
        p_vertex[nump++] = u->vertex;
    }
    int i;
    int index_h = 0;
    if(u->is_leaf){
        if(numh + nump < k) return;
        rank *tmh_vertex = new rank[numh];
        rank *tmp_vertex = new rank[nump];
        if(update_with_modify){
            for(i = 0; i < numh; i++){
                tmh_vertex[i].id = h_vertex[i];
                tmh_vertex[i].r = r[h_vertex[i]];
            }
            for(i = 0; i < nump; i++){
                tmp_vertex[i].id = p_vertex[i];
                tmp_vertex[i].r = r[p_vertex[i]];
            }
            std::sort(tmh_vertex, tmh_vertex + numh, new_cmp);
            std::sort(tmp_vertex, tmp_vertex + nump, new_cmp);
            unsigned left = nump;
            unsigned index_h = 0;
            if(k - numh != 0){
                for(i = 0; i < nump; i++){
                    left--;
                    if(tmp_vertex[i].r < r[tmh_vertex[0].id] ){
                        r[tmp_vertex[i].id] += gamma * cnm[left][k - numh - 1];
                    }
                    else{
                        r[tmh_vertex[0].id] += gamma * cnm[left][k - numh - 1];
                        index_h = 0;
                    }
                }
            }
            r[tmh_vertex[0].id] += gamma * cnm[left][k - numh];
        }
        else{
            for(i = 0; i < numh; i++){
                tmh_vertex[i].id = h_vertex[i];
                tmh_vertex[i].r = rpast[h_vertex[i]];
            }
            for(i = 0; i < nump; i++){
                tmp_vertex[i].id = p_vertex[i];
                tmp_vertex[i].r = rpast[p_vertex[i]];
            }
            std::sort(tmh_vertex, tmh_vertex + numh, new_cmp);
            std::sort(tmp_vertex, tmp_vertex + nump, new_cmp);
            unsigned left = nump;
            unsigned index_h = 0;
            if(k - numh != 0){
                for(i = 0; i < nump; i++){
                    left--;
                    if(tmp_vertex[i].r < rpast[tmh_vertex[0].id] ){
                        r[tmp_vertex[i].id] += gamma * cnm[left][k - numh - 1];
                    }
                    else{
                        r[tmh_vertex[0].id] += gamma * cnm[left][k - numh - 1];
                        index_h = 0;
                    }
                }
            }
            r[tmh_vertex[0].id] += gamma * cnm[left][k - numh];
        }
        delete[] tmp_vertex;
        delete[] tmh_vertex;
        return;
    }
    
    for(i = 0; i < u->num_child; i++){
        dfs(u->childs[i], p_vertex, h_vertex, nump, numh, r, k, gamma, rpast, update_with_modify);
    }

}

void dfs_p(node *u, rank *p_vertex, unsigned nump, unsigned numh, int minh, unsigned *rev, double *alpha, int k){
    if(u->type){
        numh++;
        if(minh == -1){
            minh = u->vertex;
        }
        else if(rev[u->vertex] > rev[minh]){
            minh = u->vertex;
        }
    }
    else{
        p_vertex[nump].id = u->vertex;
        p_vertex[nump].r = rev[u->vertex];
        nump++;
    }
    int i;
    if(u->is_leaf){
        rank *tmp_vertex = new rank[nump];
        for(i = 0; i < nump; i++){
            tmp_vertex[i] = p_vertex[i];
        }
        std::sort(tmp_vertex, tmp_vertex + nump, new_cmp);
        unsigned left = 0;
        alpha[minh] += cnm[left][k - numh];
        if(k - numh != 0){
            for(i = 0; i < nump; i++){
                if(tmp_vertex[i].r < rev[minh] ){
                    alpha[minh] += cnm[left][k - numh - 1];
                }
                else{
                    alpha[tmp_vertex[i].id] += cnm[left][k - numh - 1];
                }
                left++;
            }
        }
        delete[] tmp_vertex;
        return;
    }
    for(i = 0; i < u->num_child; i++){
        dfs_p(u->childs[i], p_vertex, nump, numh, minh, rev, alpha, k);
    }

}

void count(node *u, unsigned sumh, unsigned sump, bool *choice, unsigned k, unsigned long long &res){
    if(u->type){
        if(choice[u->vertex]){
            sumh++;
        }
        else{
            return;
        }
    }
    else if(choice[u->vertex]){
        sump++;
    }
    int i;
    if(u->is_leaf){
        res += cnm[sump][k - sumh];
        return;
    }
    for(i = 0; i < u->num_child; i++){
        count(u->childs[i], sumh, sump, choice, k ,res);
    }
}
struct sort_s{
    node * r;
    double rv;
};
bool cmp_s(sort_s a, sort_s b){
    return a.rv < b.rv;
}
void update(graph *g, node **root, unsigned k, double *r, double t, bool update_with_modify){
    unsigned i;
    double *rpast = new double[g->n];
    if(update_with_modify == 0){
        for(i = 0; i < g->n; i++){
            rpast[i] = r[i];
        }    
    }
    double gamma = 2.0 / (t + 2);
    for(i = 0; i < g->n; i++){
        r[i] = (1 - gamma) * r[i];
    }
    unsigned *pvertex = new unsigned[g->n];
    unsigned *hvertex = new unsigned[g->n];
    for(i = 0; i < g->n; i++) {
        if(root[i] != nullptr){
            dfs(root[i], pvertex, hvertex, 0, 0, r, k, gamma, rpast, update_with_modify);
        }
    }
    delete[] pvertex;
    delete[] hvertex;
    delete[] rpast;
}

double combiner(unsigned n, unsigned m){
    return cnm[n][m];
}
void dfs_shuffle(node *u){
    unsigned i;
    if(u->is_leaf) return;
    for(i = 0; i < u->num_child; i++){
        dfs_shuffle(u->childs[i]);
    }
    std::random_shuffle(u->childs, u->childs + u->num_child);
}
void shuffle_order(graph *g, node** root){
    unsigned i;
    for(i = 0; i < g->n; i++){
        if(root[i] != nullptr){
            dfs_shuffle(root[i]);
        }
    }
    std::random_shuffle(root, root + g->n);
}
int dfs_depth(node *u){
    unsigned i;
    if(u->is_leaf) return 0;
    std::pair<int,node*> *order = new std::pair<int,node*>[u->num_child];
    for(i = 0; i < u->num_child; i++){
        int dep = dfs_depth(u->childs[i]);
        order[i] = std::make_pair(-dep,u->childs[i]);
    }
    std::sort(order, order + u->num_child);
    for(i = 0; i < u->num_child; i++){
        u->childs[i] = order[i].second;
    }
    int dep = -order[0].first + 1;
    delete[] order;
    return dep;
}
void depth_order(graph *g, node** root){
    unsigned i;
    std::pair<int,node*> *order = new std::pair<int,node*>[g->n];
    for(i = 0; i < g->n; i++){
        int dep = 0;
        if(root[i] != nullptr){
            dep = dfs_depth(root[i]);
        }
        order[i] = std::make_pair(dep, root[i]);
    }
    std::sort(order, order + g->n);
    for(i = 0; i < g->n; i++){
        root[i] = order[i].second;
    }
    delete[] order;
}
