#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <vector>
#include "graph.h"
#include "graphvisuals.h"

struct AlgorithmStep {
    QString description;
    std::vector<int> highlightedVertices;
    std::vector<std::pair<int,int>> highlightedEdges;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddNode();
    void onRemoveNode();
    void onAddEdge();
    void onRemoveEdge();
    void onSetWeight();
    void onStartDFS();
    void onStartBFS();
    void onStartDijkstra();
    void onStartFloyd();
    void onStepForward();
    void onStepBackward();
    void onLoadSampleGraph();
    void onClearGraph();

private:
    void setupUI();
    void updateVisuals();
    void generateDFSSteps(int start);
    void generateBFSSteps(int start);
    void generateDijkstraSteps(int start);
    void generateFloydSteps();
    void showCurrentStep();

    Graph *graph;
    GraphVisuals *visuals;
    QGraphicsScene *scene;
    QGraphicsView *view;

    QLineEdit *fromEdit;
    QLineEdit *toEdit;
    QLineEdit *weightEdit;

    QPushButton *addNodeBtn;
    QPushButton *removeNodeBtn;
    QPushButton *addEdgeBtn;
    QPushButton *removeEdgeBtn;
    QPushButton *setWeightBtn;

    QPushButton *dfsBtn;
    QPushButton *bfsBtn;
    QPushButton *dijkstraBtn;
    QPushButton *floydBtn;

    QPushButton *loadSampleBtn;
    QPushButton *clearGraphBtn;

    QPushButton *prevStepBtn;
    QPushButton *nextStepBtn;
    QLabel *stepLabel;

    QTextEdit *outputText;

    std::vector<AlgorithmStep> algorithmSteps;
    int currentStepIndex = -1;
};

#endif