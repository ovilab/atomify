#include "neighborlist.h"

Neighborlist::Neighborlist()
{

}

Neighborlist::Neighborlist(const Neighborlist &old)
{
    numNeighbors = old.numNeighbors;
    neighbors = old.neighbors;
}
