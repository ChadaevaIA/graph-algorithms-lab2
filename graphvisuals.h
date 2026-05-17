#ifndef GRAPHVISUALS_H
#define GRAPHVISUALS_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>
#include <QPointF>
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <set>
#include "graph.h"

class GraphVisuals
{
public:
    GraphVisuals(QGraphicsScene *scene, QGraphicsView *view, Graph *graph);

    void redraw();
    void visualizeStep(const std::vector<int>& vertices,
                       const std::vector<std::pair<int,int>>& edges);
    void assignCircularPositions();
    void adjustPositionsForNewNodes();
    void fitToView();
    void savePositions();
    void loadPositions();

private:
    void drawBaseGraph();
    void drawEdge(int from, int to, QColor color = Qt::black, int boldness = 2);

    QGraphicsScene *scene;
    QGraphicsView *view;
    Graph *graph;
    std::vector<QPointF> positions;
};

#endif