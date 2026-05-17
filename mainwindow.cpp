#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <queue>
#include <functional>

const int INF = 1e9;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    graph = new Graph();
    setupUI();
}

MainWindow::~MainWindow()
{
    delete graph;
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    QVBoxLayout *leftPanel = new QVBoxLayout();

    QFormLayout *form = new QFormLayout();
    fromEdit = new QLineEdit();
    toEdit = new QLineEdit();
    weightEdit = new QLineEdit();
    form->addRow("Откуда:", fromEdit);
    form->addRow("Куда:", toEdit);
    form->addRow("Вес:", weightEdit);
    leftPanel->addLayout(form);

    QHBoxLayout *nodeBtns = new QHBoxLayout();
    addNodeBtn = new QPushButton("Добавить вершину");
    removeNodeBtn = new QPushButton("Удалить вершину");
    nodeBtns->addWidget(addNodeBtn);
    nodeBtns->addWidget(removeNodeBtn);
    leftPanel->addLayout(nodeBtns);

    QHBoxLayout *edgeBtns = new QHBoxLayout();
    addEdgeBtn = new QPushButton("Добавить ребро");
    removeEdgeBtn = new QPushButton("Удалить ребро");
    setWeightBtn = new QPushButton("Изменить вес");
    edgeBtns->addWidget(addEdgeBtn);
    edgeBtns->addWidget(removeEdgeBtn);
    edgeBtns->addWidget(setWeightBtn);
    leftPanel->addLayout(edgeBtns);

    leftPanel->addWidget(new QLabel("Алгоритмы:"));
    QHBoxLayout *algoBtns1 = new QHBoxLayout();
    dfsBtn = new QPushButton("DFS");
    bfsBtn = new QPushButton("BFS");
    algoBtns1->addWidget(dfsBtn);
    algoBtns1->addWidget(bfsBtn);
    leftPanel->addLayout(algoBtns1);

    QHBoxLayout *algoBtns2 = new QHBoxLayout();
    dijkstraBtn = new QPushButton("Дейкстра");
    floydBtn = new QPushButton("Флойд");
    algoBtns2->addWidget(dijkstraBtn);
    algoBtns2->addWidget(floydBtn);
    leftPanel->addLayout(algoBtns2);

    QHBoxLayout *stepNav = new QHBoxLayout();
    prevStepBtn = new QPushButton("← Шаг назад");
    nextStepBtn = new QPushButton("Шаг вперёд →");
    stepLabel = new QLabel("Шаг 0 / 0");
    stepNav->addWidget(prevStepBtn);
    stepNav->addWidget(stepLabel);
    stepNav->addWidget(nextStepBtn);
    leftPanel->addLayout(stepNav);

    loadSampleBtn = new QPushButton("Загрузить граф из варианта");
    leftPanel->addWidget(loadSampleBtn);

    clearGraphBtn = new QPushButton("Очистить граф");
    leftPanel->addWidget(clearGraphBtn);

    outputText = new QTextEdit();
    outputText->setReadOnly(true);
    outputText->setMaximumHeight(150);
    leftPanel->addWidget(outputText);

    leftPanel->addStretch();
    mainLayout->addLayout(leftPanel);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumSize(600, 500);
    mainLayout->addWidget(view, 1);

    visuals = new GraphVisuals(scene, view, graph);

    connect(addNodeBtn, &QPushButton::clicked, this, &MainWindow::onAddNode);
    connect(removeNodeBtn, &QPushButton::clicked, this, &MainWindow::onRemoveNode);
    connect(addEdgeBtn, &QPushButton::clicked, this, &MainWindow::onAddEdge);
    connect(removeEdgeBtn, &QPushButton::clicked, this, &MainWindow::onRemoveEdge);
    connect(setWeightBtn, &QPushButton::clicked, this, &MainWindow::onSetWeight);
    connect(dfsBtn, &QPushButton::clicked, this, &MainWindow::onStartDFS);
    connect(bfsBtn, &QPushButton::clicked, this, &MainWindow::onStartBFS);
    connect(dijkstraBtn, &QPushButton::clicked, this, &MainWindow::onStartDijkstra);
    connect(floydBtn, &QPushButton::clicked, this, &MainWindow::onStartFloyd);
    connect(prevStepBtn, &QPushButton::clicked, this, &MainWindow::onStepBackward);
    connect(nextStepBtn, &QPushButton::clicked, this, &MainWindow::onStepForward);
    connect(loadSampleBtn, &QPushButton::clicked, this, &MainWindow::onLoadSampleGraph);
    connect(clearGraphBtn, &QPushButton::clicked, this, &MainWindow::onClearGraph);
}

void MainWindow::updateVisuals()
{
    visuals->adjustPositionsForNewNodes();  // Используем новый метод
    visuals->redraw();
}

void MainWindow::onAddNode()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Добавить вершину", "Имя вершины:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        if (graph->getNodeIndex(name) != -1) {
            QMessageBox::warning(this, "Ошибка", "Вершина с таким именем уже существует.");
            return;
        }
        graph->addNode(name);
        updateVisuals();
        outputText->append(QString("Вершина '%1' добавлена.").arg(name));
    }
}

void MainWindow::onRemoveNode()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Удалить вершину", "Имя вершины:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    int index = graph->getNodeIndex(name);
    if (index == -1) {
        QMessageBox::warning(this, "Ошибка", "Вершина не найдена.");
        return;
    }
    graph->removeNode(index);
    updateVisuals();
    outputText->append(QString("Вершина '%1' удалена.").arg(name));
}

void MainWindow::onAddEdge()
{
    QString fromName = fromEdit->text().trimmed();
    QString toName = toEdit->text().trimmed();
    int w = weightEdit->text().toInt();
    int from = graph->getNodeIndex(fromName);
    int to = graph->getNodeIndex(toName);
    if (from == -1 || to == -1) {
        QMessageBox::warning(this, "Ошибка", "Вершины не найдены.");
        return;
    }
    if (from == to) {
        QMessageBox::warning(this, "Ошибка", "Нельзя создать ребро в саму себя.");
        return;
    }
    graph->addEdge(from, to, w);
    updateVisuals();
    outputText->append(QString("Ребро %1 → %2 (вес %3) добавлено.").arg(fromName, toName).arg(w));
}

void MainWindow::onRemoveEdge()
{
    QString fromName = fromEdit->text().trimmed();
    QString toName = toEdit->text().trimmed();
    int from = graph->getNodeIndex(fromName);
    int to = graph->getNodeIndex(toName);
    if (from == -1 || to == -1) {
        QMessageBox::warning(this, "Ошибка", "Вершины не найдены.");
        return;
    }
    graph->removeEdge(from, to);
    updateVisuals();
    outputText->append(QString("Ребро %1 → %2 удалено.").arg(fromName, toName));
}

void MainWindow::onSetWeight()
{
    QString fromName = fromEdit->text().trimmed();
    QString toName = toEdit->text().trimmed();
    int w = weightEdit->text().toInt();
    int from = graph->getNodeIndex(fromName);
    int to = graph->getNodeIndex(toName);
    if (from == -1 || to == -1) {
        QMessageBox::warning(this, "Ошибка", "Вершины не найдены.");
        return;
    }
    graph->setWeight(from, to, w);
    updateVisuals();
    outputText->append(QString("Вес ребра %1 → %2 изменён на %3.").arg(fromName, toName).arg(w));
}

void MainWindow::onLoadSampleGraph()
{
    // Очищаем граф
    while (graph->nodeCount() > 0)
        graph->removeNode(0);

    // Добавляем вершины (1-6)
    for (int i = 1; i <= 6; ++i)
        graph->addNode(QString::number(i));

    // Добавляем рёбра согласно варианту
    graph->addEdge(0, 1, 34);  // 1-2
    graph->addEdge(0, 2, 8);   // 1-3
    graph->addEdge(0, 4, 2);   // 1-5
    graph->addEdge(0, 5, 21);  // 1-6

    graph->addEdge(1, 2, 57);  // 2-3
    graph->addEdge(1, 4, 5);   // 2-5
    graph->addEdge(1, 3, 45);  // 2-4

    graph->addEdge(3, 4, 3);   // 4-5

    graph->addEdge(4, 5, 13);  // 5-6

    // Принудительно пересчитываем позиции
    visuals->assignCircularPositions();
    visuals->savePositions(); // Сохраняем позиции для этого графа
    updateVisuals();

    outputText->append("Загружен граф из варианта.");

    // Автоматически запускаем алгоритм Дейкстры от вершины 2 (индекс 1)
    fromEdit->setText("2");  // Исправлено: начинаем с вершины 2
    onStartDijkstra();
}
void MainWindow::onClearGraph()
{
    while (graph->nodeCount() > 0)
        graph->removeNode(0);
    updateVisuals();
    algorithmSteps.clear();
    currentStepIndex = -1;
    stepLabel->setText("Шаг 0 / 0");
    outputText->clear();
    outputText->append("Граф очищен.");
}

void MainWindow::onStartDFS()
{
    QString startName = fromEdit->text().trimmed();
    int start = graph->getNodeIndex(startName);
    if (start == -1) {
        QMessageBox::warning(this, "Ошибка", "Вершина не найдена.");
        return;
    }
    generateDFSSteps(start);
    currentStepIndex = 0;
    showCurrentStep();
}

void MainWindow::onStartBFS()
{
    QString startName = fromEdit->text().trimmed();
    int start = graph->getNodeIndex(startName);
    if (start == -1) {
        QMessageBox::warning(this, "Ошибка", "Вершина не найдена.");
        return;
    }
    generateBFSSteps(start);
    currentStepIndex = 0;
    showCurrentStep();
}

void MainWindow::onStartDijkstra()
{
    QString startName = fromEdit->text().trimmed();
    int start = graph->getNodeIndex(startName);
    if (start == -1) {
        QMessageBox::warning(this, "Ошибка", "Вершина не найдена.");
        return;
    }
    generateDijkstraSteps(start);
    currentStepIndex = 0;
    showCurrentStep();
}

void MainWindow::onStartFloyd()
{
    generateFloydSteps();
    currentStepIndex = 0;
    showCurrentStep();
}

void MainWindow::onStepForward()
{
    if (algorithmSteps.empty() || currentStepIndex >= (int)algorithmSteps.size() - 1) return;
    currentStepIndex++;
    showCurrentStep();
}

void MainWindow::onStepBackward()
{
    if (algorithmSteps.empty() || currentStepIndex <= 0) return;
    currentStepIndex--;
    showCurrentStep();
}

void MainWindow::generateDFSSteps(int start)
{
    algorithmSteps.clear();
    size_t n = graph->nodeCount();
    std::vector<bool> visited(n, false);
    std::vector<int> order;

    std::function<void(int)> dfsVisit;
    dfsVisit = [&](int v) {
        visited[v] = true;
        order.push_back(v);
        algorithmSteps.push_back(AlgorithmStep{
            QString("Посещаем вершину %1").arg(graph->nodeNames()[v]),
            {v}, {}
        });

        for (size_t i = 0; i < n; ++i) {
            if (graph->getWeight(v, (int)i) != -1) {
                int neighbor = (int)i;
                algorithmSteps.push_back(AlgorithmStep{
                    QString("Рассматриваем ребро %1 → %2").arg(graph->nodeNames()[v]).arg(graph->nodeNames()[neighbor]),
                    {v, neighbor}, {{v, neighbor}}
                });
                if (!visited[neighbor]) {
                    dfsVisit(neighbor);
                }
            }
        }
        algorithmSteps.push_back(AlgorithmStep{
            QString("Возврат из %1").arg(graph->nodeNames()[v]),
            {v}, {}
        });
    };

    algorithmSteps.push_back(AlgorithmStep{
        QString("Начинаем DFS от %1").arg(graph->nodeNames()[start]),
        {start}, {}
    });
    dfsVisit(start);

    QString result = "Порядок обхода DFS: ";
    for (size_t i = 0; i < order.size(); ++i) {
        result += graph->nodeNames()[order[i]];
        if (i != order.size() - 1) result += " → ";
    }
    algorithmSteps.push_back(AlgorithmStep{result, {}, {}});
}

void MainWindow::generateBFSSteps(int start)
{
    algorithmSteps.clear();
    size_t n = graph->nodeCount();
    std::vector<bool> visited(n, false);
    std::queue<int> q;
    std::vector<int> order;

    visited[start] = true;
    q.push(start);
    algorithmSteps.push_back(AlgorithmStep{
        QString("Начинаем BFS от %1, добавляем в очередь").arg(graph->nodeNames()[start]),
        {start}, {}
    });

    while (!q.empty()) {
        int v = q.front(); q.pop();
        order.push_back(v);
        algorithmSteps.push_back(AlgorithmStep{
            QString("Извлекаем %1 из очереди").arg(graph->nodeNames()[v]),
            {v}, {}
        });

        for (size_t i = 0; i < n; ++i) {
            if (graph->getWeight(v, (int)i) != -1) {
                int neighbor = (int)i;
                algorithmSteps.push_back(AlgorithmStep{
                    QString("Рассматриваем ребро %1 → %2").arg(graph->nodeNames()[v]).arg(graph->nodeNames()[neighbor]),
                    {v, neighbor}, {{v, neighbor}}
                });
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                    algorithmSteps.push_back(AlgorithmStep{
                        QString("Добавляем %1 в очередь").arg(graph->nodeNames()[neighbor]),
                        {neighbor}, {}
                    });
                }
            }
        }
    }

    QString result = "Порядок обхода BFS: ";
    for (size_t i = 0; i < order.size(); ++i) {
        result += graph->nodeNames()[order[i]];
        if (i != order.size() - 1) result += " → ";
    }
    algorithmSteps.push_back(AlgorithmStep{result, {}, {}});
}

void MainWindow::generateDijkstraSteps(int start)
{
    algorithmSteps.clear();
    size_t n = graph->nodeCount();
    std::vector<int> dist(n, INF);
    std::vector<int> parent(n, -1);
    std::vector<bool> used(n, false);
    dist[start] = 0;

    algorithmSteps.push_back(AlgorithmStep{
        QString("Инициализация: dist[%1] = 0, остальные = ∞").arg(graph->nodeNames()[start]),
        {start}, {}
    });

    for (size_t iter = 0; iter < n; ++iter) {
        int u = -1;
        for (size_t i = 0; i < n; ++i) {
            if (!used[i] && (u == -1 || dist[i] < dist[u])) {
                u = (int)i;
            }
        }
        if (u == -1 || dist[u] == INF) break;

        algorithmSteps.push_back(AlgorithmStep{
            QString("Выбрана вершина %1 (расстояние %2)").arg(graph->nodeNames()[u]).arg(dist[u]),
            {u}, {}
        });

        used[u] = true;
        for (size_t v = 0; v < n; ++v) {
            int w = graph->getWeight(u, (int)v);
            if (w != -1) {
                int newDist = dist[u] + w;
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    parent[v] = u;
                    algorithmSteps.push_back(AlgorithmStep{
                        QString("Обновляем dist[%1] = %2 через %3").arg(graph->nodeNames()[v]).arg(newDist).arg(graph->nodeNames()[u]),
                        {u, (int)v}, {{u, (int)v}}
                    });
                }
            }
        }
    }

    QString result = "Результат Дейкстры от вершины " + graph->nodeNames()[start] + ":\n";
    for (size_t i = 0; i < n; ++i) {
        if (dist[i] == INF) {
            result += graph->nodeNames()[i] + ": недостижима\n";
        } else {
            result += graph->nodeNames()[i] + ": " + QString::number(dist[i]);
            if (i != (size_t)start) {
                result += " (путь: ";
                std::vector<int> path;
                int cur = (int)i;
                while (cur != -1) {
                    path.push_back(cur);
                    cur = parent[cur];
                }
                for (int j = path.size() - 1; j >= 0; --j) {
                    result += graph->nodeNames()[path[j]];
                    if (j > 0) result += " → ";
                }
                result += ")";
            }
            result += "\n";
        }
    }
    algorithmSteps.push_back(AlgorithmStep{result, {}, {}});
}

void MainWindow::generateFloydSteps()
{
    algorithmSteps.clear();
    size_t n = graph->nodeCount();
    std::vector<std::vector<int>> D(n, std::vector<int>(n, INF));

    for (size_t i = 0; i < n; ++i) {
        D[i][i] = 0;
        for (size_t j = 0; j < n; ++j) {
            if (i != j) {
                int w = graph->getWeight((int)i, (int)j);
                if (w != -1) D[i][j] = w;
            }
        }
    }

    QString matrixStr = "Исходная матрица:\n";
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (D[i][j] == INF) matrixStr += "∞ ";
            else matrixStr += QString::number(D[i][j]) + " ";
        }
        matrixStr += "\n";
    }
    algorithmSteps.push_back(AlgorithmStep{matrixStr, {}, {}});

    for (size_t k = 0; k < n; ++k) {
        algorithmSteps.push_back(AlgorithmStep{
            QString("Итерация k = %1 (%2)").arg(k+1).arg(graph->nodeNames()[k]),
            {(int)k}, {}
        });
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (D[i][k] < INF && D[k][j] < INF && D[i][k] + D[k][j] < D[i][j]) {
                    D[i][j] = D[i][k] + D[k][j];
                    algorithmSteps.push_back(AlgorithmStep{
                        QString("D[%1][%2] = %3 через %4").arg(graph->nodeNames()[i]).arg(graph->nodeNames()[j]).arg(D[i][j]).arg(graph->nodeNames()[k]),
                        {(int)i, (int)j, (int)k}, {}
                    });
                }
            }
        }
    }

    matrixStr = "Итоговая матрица кратчайших расстояний:\n";
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (D[i][j] == INF) matrixStr += "∞ ";
            else matrixStr += QString::number(D[i][j]) + " ";
        }
        matrixStr += "\n";
    }
    algorithmSteps.push_back(AlgorithmStep{matrixStr, {}, {}});
}

void MainWindow::showCurrentStep()
{
    if (currentStepIndex < 0 || currentStepIndex >= (int)algorithmSteps.size()) return;
    const AlgorithmStep& step = algorithmSteps[currentStepIndex];
    stepLabel->setText(QString("Шаг %1 / %2").arg(currentStepIndex + 1).arg(algorithmSteps.size()));
    outputText->clear();
    outputText->append(step.description);
    visuals->visualizeStep(step.highlightedVertices, step.highlightedEdges);
}