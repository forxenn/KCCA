/*
    to complie:
        g++ kcca.cpp graph.cpp sct.cpp pava.cpp -O3 -o kcca
    to run:
        ./kcca <graph-path> <k> <max_iter> <output_path> <KCCA-basic/KCCA> <update roder> <approximation rate>
*/
#include <bits/stdc++.h>
#include "graph.h"
#include "sct.h"
#include "pava.h"

int main(int argc, char** argv){

    unsigned k = atoi(argv[2]);
    unsigned max_iter = atoi(argv[3]);
    freopen(argv[4], "w", stdout);

    bool update_with_modify = atoi(argv[5]);
    int update_sequence = atoi(argv[6]);
    float err = atof(argv[7]);
    std::cout << "Computing for k = " << k << "  Max_iter is: " << max_iter << std:: endl;
    graph *g = build_graph(argv[1],k);
    auto start_time = std::chrono::high_resolution_clock::now();
    node **root = build_sct(g, k);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Indexing time  " << duration.count() << " milliseconds." << std::endl;
    
    // 0: no-order
    // 1: degen
    // 2: depth
    // 3: random
    switch (update_sequence)
    {
    case 0:
        break;
    case 1:
        std::reverse(root, root + g->n);
        break;
    case 2:
        depth_order(g, root);
        break;
    case 3:
        shuffle_order(g, root);
        break;
    default:
        break;
    }
    double *r = new double[g->n]();
     
    double rho_lower_bound = 0;
   
    if (max_iter == 0) {
        auto start_time_1 = std::chrono::high_resolution_clock::now();
        unsigned T = 1, t = 0;
        for(; ;T <<= 1) {
            for(;t < T; t++) {
                if(t % 100 == 0) {
                    printf("Iteration %d\n",t);
                }
                update(g, root, k, r, t, update_with_modify);
            }
            double res = pava(g, root, r, k);
            auto end_time_1 = std::chrono::high_resolution_clock::now();
            auto duration_1 = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_1 - start_time);
            std::cout << "Total running time after iteration: " << t << "  " <<  duration_1.count() << " milliseconds." << std::endl;
            if(res < err) break;
        }
    } else {
    
        unsigned t;
        for(t = 0; t < max_iter; t++){
            update(g, root, k, r, t, update_with_modify);
        }
        pava(g, root, r, k);
    }
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Total running time  " << duration.count() << " milliseconds." << std::endl;
}