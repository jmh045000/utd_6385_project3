
#ifndef _NI_H
#define _NI_H

#include <vector>

#include <boost/pending/relaxed_heap.hpp>

#include "types.h"

typedef std::vector<uint32_t > values_type;

class heap_less
{
public:
    values_type *values;

    heap_less(values_type *v) : values(v) {}

    bool operator() (uint32_t left, uint32_t right) const;

};

class NI
{
    typedef boost::relaxed_heap<uint32_t, heap_less> heap_type;
private:
    values_type values;
    uint32_t m;
    std::vector<uint32_t> contractedNodes;
    std::vector<uint32_t> nodes;
    uint32_t **adjacency, **orgAdjacency;
    
    uint32_t degree(uint32_t n1);
    uint32_t degree(uint32_t n1, uint32_t n2, bool contracting=false, uint32_t newN = 0);
    void contract(uint32_t n1, uint32_t n2, heap_type &heap);
    
public:
    //NI() {}
    NI(Graph *g);
    ~NI();
    uint32_t ni();
    
    typedef bool result_type;
    //bool operator() (uint32_t left, uint32_t right) const;
};


#endif
