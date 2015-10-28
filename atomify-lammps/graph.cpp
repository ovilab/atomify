#include "graph.h"
#include "figure.h"

void Graph::setDirty(bool dirty) {
    if(m_dirty ==dirty)
        return;
    m_dirty = dirty;

    if(m_dirty) {
        // Mark parent as dirty as well (or just call update)
        Figure *figure = qobject_cast<Figure*>(parent());
        if(figure) {
            figure->update();
        }
    }
}

void Graph::markDirty()
{
    setDirty(true);
}
