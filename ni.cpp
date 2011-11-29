
#include <iostream>
#include <sstream>
#include <string>
#include <climits>
#include <cmath>
#include <vector>
using std::cout;
using std::cerr;
using std::endl;
using std::istringstream;
using std::string;
using std::vector;

#include <cstring>

#include "ni.h"
#include "utils.h"

    bool heap_less::operator() (uint32_t left, uint32_t right) const
    {
        /* This seems a little counter intuitive...
         * I need the MAX number of edges to have a quick
         * "get" time.
         * This way, I keep the benefits of using a heap,
         * but can grab the next value in the MA order in O(1) time
         */
        return (*values)[left] > (*values)[right];
    }

extern uint32_t n;

uint32_t NI::degree(uint32_t n1)
{
    uint32_t degree = 0;
    for(uint32_t i = 0; i < n; i++)
    {
        degree += orgAdjacency[n1][i];
    }
    for(uint32_t i = 0; i < n; i++)
    {
        degree += orgAdjacency[i][n1];
    }

    return degree;
}

uint32_t NI::degree(uint32_t n1, uint32_t n2, 
    bool contracting, uint32_t newN)
{
    if(n1 < n2)
    {
        uint32_t tmp = n2;
        n2 = n1;
        n1 = tmp;
    }

    if(contracting)
    {
        uint32_t ret = adjacency[n1][n2];
        adjacency[newN][n2] += ret;
        adjacency[n1][n2] = 0;
        return ret;
    }
    else
    {
        return adjacency[n1][n2];
    }
};

void NI::contract(uint32_t n1, uint32_t n2, heap_type &heap)
{
    contractedNodes.push_back(n2);

#ifdef _DEBUG
    cout << "degree between " << n2 << " and " << n1 
	    << " is " << degree(n1,n2) << endl;
#endif /*_DEBUG*/

    for(uint32_t i = 0; i < n; i++)
    {
        bool flag = false;
        for(uint32_t j = 0; j < contractedNodes.size(); j++)
        {
            if(contractedNodes[j] == i)
            {
                flag = true;
                break;
            }
        }
        if(flag)
            continue;

#ifdef _DEBUG
        cout << "degree between " << n2 << " and " << i 
		    << " is " << degree(n2,i) << endl;
#endif /*_DEBUG*/
        values[nodes[i]] += degree(n2, i, true, n1);
        heap.update(i);

    }
    values[nodes[n2]] = 0;
    heap.remove(n2);
}

/* The readConfig function assumes a certain format for input
 * First line: number of nodes
 * Second line-End: [node] [node]
 * Each represents an edge
 */
NI::NI(Graph *g)
{

    adjacency = new uint32_t* [n];
    for(uint32_t i = 0; i < n; i++)
    {
        adjacency[i] = new uint32_t [n];
        memset(adjacency[i], 0, sizeof(uint32_t)*n);
    }

    m = 0;

    Graph::edge_iterator e, e_end;
    tie(e, e_end) = boost::edges(*g);

    for( ; e != e_end; ++e)
    {
        uint32_t u,v;
        u = boost::source(*e, *g);
        v = boost::target(*e, *g);
        if(u > n || v > n)
        {
            cerr << "Node larger than max nodes" << endl;
        }

        if(v <= u)
        {
            adjacency[u][v]++;
        }
        else
        {
            adjacency[v][u]++;
        }
        m++;
    }
}

NI::~NI()
{
    for(uint32_t i = 0; i < n; i++)
    {
        delete [] adjacency[i];
        adjacency[i] = NULL;
    }
    for(uint32_t i = 0; i < n; i++)
    {
        delete [] orgAdjacency[i];
        orgAdjacency[i] = NULL;
    }
}

#ifdef _DEBUG
std::ostream & operator << (std::ostream &ostr, std::vector<uint32_t> v)
{
    for(unsigned i = 0; i < v.size(); i++)
    {
        cout << v[i] << " ";
    }
    cout << endl;
    return ostr;
}

std::ostream & operator << (std::ostream &ostr, std::vector<Edge> v)
{
    for(unsigned i = 0; i < v.size(); i++)
    {
        cout << v[i].first << " -- " << v[i].second << endl;
    }
    return ostr;
}
#endif /*_DEBUG*/

uint32_t NI::ni()
{
#ifdef _DEBUG
        cout << "Running with n=" << n << " and m=" << m << endl;
#endif

        Random rand(n);

        for(uint32_t i = 0; i < n; i++)
        {
            nodes.push_back(i);
        }
     
        orgAdjacency = new uint32_t* [n];
        for(uint32_t i = 0; i < n; i++)
        {
            orgAdjacency[i] = new uint32_t [n];
        }
    
        heap_less less(&values);
        heap_type heap(n, less);
    
        for(uint32_t i = 0; i < n; i++)
        {
            memcpy(orgAdjacency[i], adjacency[i], sizeof(uint32_t)*n);
        }
    
        double avgDegree = 0;
    
        for(uint32_t i = 0; i < n; i++)
        {
            avgDegree += degree(i);
        }
        avgDegree /= n;
        values.resize(n);
    
        uint32_t seed = rand.getRandomInt();
        contractedNodes.push_back(seed);
        values[nodes[seed]] = 0;
    
        for(uint32_t j = 0; j < n; j++)
        {
            if(j == seed)
                continue;
    
            values[nodes[j]] = degree(nodes[seed], nodes[j]);
            heap.push(nodes[j]);
        }
    
        for(uint32_t i = 0; i < n-1; i++)
        {
            contract(nodes[seed], heap.top(), heap);
        }
        /* At this point, we have the 
		 * MA ordering in the contractedNodes vector */
    
        uint32_t lambdaG = UINT_MAX;
    
        vector<uint32_t> criticalNodes;
    
        for(vector<uint32_t>::reverse_iterator iter 
		    = contractedNodes.rbegin();
            iter != contractedNodes.rend(); ++iter)
        {
            if(degree(*iter) < lambdaG)
            {
                lambdaG = degree(*iter);
                criticalNodes.clear();
                criticalNodes.push_back(*iter);
            }
            else if(degree(*iter) == lambdaG)
            {
                criticalNodes.push_back(*iter);
            }
        }

        uint32_t criticalEdges = 0;
    
        for(vector<uint32_t>::iterator iter = criticalNodes.begin();
            iter != criticalNodes.end(); ++iter)
        {
#ifdef _DEBUG
            cout << *iter << " has " << degree(*iter) 
			    << " critical edges" << endl;
#endif /*_DEBUG*/
    
            uint32_t deg = degree(*iter);
            criticalEdges += deg;
        }

    return lambdaG;
}

