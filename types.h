
#ifndef _TYPES_H
#define _TYPES_H

extern "C"
{
#   include <stdint.h>
}
#include <map>
#include <vector>


#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>

/* Graph Types */
typedef boost::property<boost::edge_weight_t, int> EdgeWeight;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property,
    EdgeWeight > Graph;


typedef std::pair<uint32_t, uint32_t> Edge;

#endif /*_TYPES_H*/
