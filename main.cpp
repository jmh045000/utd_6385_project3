
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>
#include <algorithm>

#include <boost/lexical_cast.hpp>

#include <boost/graph/graph_utility.hpp>

#include "types.h"
#include "utils.h"
#include "ni.h"

extern "C"
{
#   include <stdint.h>
#   include <pthread.h>
}

uint32_t n = 20, k = 3;
static const int NUM_THREADS = 10;

Graph * randomKGraph()
{
    Random rand(n);

    Graph *g = new Graph(n);

    while(NI(g).ni() < k)
    {
        uint32_t u = rand.getRandomInt();
        uint32_t v = u;
        while(v == u) 
        {
            v = rand.getRandomInt();
        }
        add_edge(u, v, EdgeWeight(1), *g);
    }
    return g;
}

Graph * fullGraph()
{
    Graph *g = new Graph(n);

    for(uint32_t i = 0; i < n; i++)
    {
        for(uint32_t j = i+1; j < n; j++)
        {
            add_edge(i, j, EdgeWeight(1), *g);
        }
    }

    return g;
}

uint32_t costOfGraph(Graph *g)
{
    std::vector < int >d(n, (std::numeric_limits < int >::max)());
    int **distances;
    distances = new int*[n];
    for(uint32_t i = 0; i < n; i++)
    {
        distances[i] = new int[n];
    }
    boost::floyd_warshall_all_pairs_shortest_paths(*g, distances, boost::distance_map(&d[0]));

    int diameter = 0;

    for(uint32_t i = 0; i < n; i++)
    {
        for(uint32_t j = 0; j < n; j++)
        {
            if(distances[i][j] > diameter) diameter = distances[i][j];
        }
    }
    uint32_t numEdges = boost::num_edges(*g);

    return numEdges + diameter;
}

void mutate(Graph *g, double scaling)
{
    typedef std::vector<Edge> EdgeVector;
    Random rand(n);
    double addProbability = scaling*0.1;
    double remProbability = scaling*0.5;

    Graph::edge_iterator e, e_end;
    tie(e, e_end) = boost::edges(*g);

    EdgeVector edges;

    for( ; e != e_end; ++e)
    {
        uint32_t source = boost::source(*e, *g);
        uint32_t target = boost::target(*e, *g);
        edges.push_back(Edge(source, target));
    }

    for(EdgeVector::iterator it = edges.begin(); it != edges.end(); ++it)
    {
        double prob = rand.getRandom01();
        if(prob < addProbability)
        {
            add_edge(rand.getRandomInt(), rand.getRandomInt(), EdgeWeight(1), *g);
        }
        else if(prob < remProbability)
        {
            remove_edge(it->first, it->second, *g);
            if(NI(g).ni() < k)
            {
                add_edge(it->first, it->second, EdgeWeight(1), *g);
            }
        }
    }
}

void * sol1(void *)
{
    const uint32_t sameCostLimit = n*n;
    Graph *g = fullGraph();

    uint32_t lastCost = costOfGraph(g);
    uint32_t sameCost = 0;

    while(sameCost < sameCostLimit)
    {
        Graph newG(*g);
        mutate(&newG, 1-((double)sameCost/(double)sameCostLimit));
        
        uint32_t cost = costOfGraph(&newG);
        if(cost < lastCost)
        {
            lastCost = cost;
            sameCost = 0;

            delete g;
            g = new Graph(newG);
        }
        else
        {
            sameCost++;
        }
    }
    
    return g;
}

bool comparator (Graph *left, Graph *right)
{
    return costOfGraph(left) < costOfGraph(right);
}

Graph *mate(Graph *g1, Graph *g2)
{
    Graph *lower, *upper;
    Graph *g = new Graph(n);
    Random rand(n);

    if(costOfGraph(g1) <= costOfGraph(g2))
    {
        lower = g1;
        upper = g2;
    }
    else
    {
        lower = g2;
        upper = g1;
    }

    // If the two costs are very close to each other, make it close to 0.5
    // Otherwise, favor the lower cost graph more, proportionally with how much lower it costs
    double probability = ((double)costOfGraph(lower)/(double)costOfGraph(upper))/2;
    
    Graph::edge_iterator lower_e, lower_e_end;
    Graph::edge_iterator upper_e, upper_e_end;

    for(boost::tie(lower_e, lower_e_end) = boost::edges(*lower), boost::tie(upper_e, upper_e_end) = boost::edges(*upper);
        lower_e != lower_e_end && upper_e != upper_e_end && costOfGraph(g) < k;
        lower_e++, upper_e++)
    {
        if(rand.getRandom01() >= probability)
        {
            add_edge(boost::source(*lower_e, *lower), boost::target(*lower_e, *lower), EdgeWeight(1), *g);
        }
        else
        {
            add_edge(boost::source(*upper_e, *upper), boost::target(*upper_e, *upper), EdgeWeight(1), *g);
        }
    }

    while(costOfGraph(g) < k)
    {
        if(lower_e != lower_e_end)
        {
            add_edge(boost::source(*lower_e, *lower), boost::target(*lower_e, *lower), EdgeWeight(1), *g);
            lower_e++;
        }
        else if(upper_e != upper_e_end)
        {
            add_edge(boost::source(*upper_e, *upper), boost::target(*upper_e, *upper), EdgeWeight(1), *g);
        }
        else
        {
            // Try, Try again
            delete g;
            g = mate(g1, g2);
        }
    }


    return g;
}

void * sol2(void *)
{
    const int NUM_GRAPHS = n*10;
    const int GENERATION_LIMIT = 10;

    std::vector<Graph *> g;

    for(int i = 0; i < NUM_GRAPHS; i++)
    {
        g.push_back(randomKGraph());
    }

    for(int generation = 0; generation < GENERATION_LIMIT; generation++)
    {
        std::vector<Graph *> gVec;
        for(int i = 0; i < NUM_GRAPHS; i++)
        {
            Random rand(g.size());
            uint32_t index1 = rand.getRandomInt(), index2 = rand.getRandomInt();
            gVec.push_back(mate(g[index1], g[index2]));
        }

        gVec.insert(gVec.begin(), g.begin(), g.end());

        std::sort(gVec.begin(), gVec.end(), comparator);
        g.clear();
        g.insert(g.begin(), gVec.begin(), gVec.begin()+NUM_GRAPHS);
    }

    return g[0];
}

int main(int argc, char *argv[])
{
    // Multi-call binary
    enum{SOL1, SOL2, UNDEF};
    int execution_type = UNDEF;

    if(strstr(argv[0], "sol1") != NULL) execution_type = SOL1;
    else if(strstr(argv[0], "sol2") != NULL) execution_type = SOL2;
    else 
    {
        std::cerr << "Must call through sol1 or sol2" << std::endl;
        return -1;
    }

    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-n") == 0)
        {
            try
            {
                n = boost::lexical_cast<int> (argv[i+1]);
                i++;
            }
            catch(...)
            {
                std::cerr << "Invalid argument to -n" << std::endl;
                return -1;
            }
        }
        if(strcmp(argv[i], "-k") == 0)
        {
            try
            {
                k = boost::lexical_cast<int> (argv[i+1]);
                i++;
            }
            catch(...)
            {
                std::cerr << "Invalid argument to -k" << std::endl;
                return -1;
            }
        }
    }
        
    // Use threads as a trick
    // Start 10 threads, pick the best solution among them
    Graph *g[10];
    pthread_t id[10];

    if(execution_type == SOL1)
    {
        for(int i = 0; i < NUM_THREADS; i++)
        {
            pthread_create(&id[i], NULL, sol1, NULL);
        }
    }
    else
    {
        for(int i = 0; i < NUM_THREADS; i++)
        {
            pthread_create(&id[i], NULL, sol2, NULL);
        }
    }

    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(id[i], (void**)&g[i]);
    }

    Graph *minG = g[0];
    for(int i = 1; i < NUM_THREADS; i++)
    {
        if(costOfGraph(minG) < costOfGraph(g[i]))
        {
            delete g[i];
        }
        else
        {
            delete minG;
            minG = g[i];
        }
    }
    std::cout << costOfGraph(minG) << std::endl;

    char *name = new char[n];
    for(uint32_t i = 0; i < n; i++)
    {
        name[i] = 'A'+i;
    }

/*
  std::cout << "vertex set: ";
  boost::print_vertices(*g, name);
  std::cout << std::endl;

  std::cout << "edge set: ";
  boost::print_edges(*g, name);
  std::cout << std::endl;

  std::cout << "out-edges: " << std::endl;
  boost::print_graph(*g, name);
  std::cout << std::endl;
*/
  return 0;
}


