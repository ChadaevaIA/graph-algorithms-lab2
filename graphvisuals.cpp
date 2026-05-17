#include "graphvisuals.h"
#include <QtMath>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QGraphicsTextItem>
#include <QDebug>
#include <set>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GraphVisuals::GraphVisuals(QGraphicsScene *scene, QGraphicsView *view, Graph *graph)
    : scene(scene), view(view), graph(graph)
{
    loadPositions();
}

void GraphVisuals::redraw()
{
    scene->clear();
    size_t n = graph->nodeCount();
    if (n == 0) return;

    if (positions.size() != n) {
        adjustPositionsForNewNodes();
        savePositions();
    }

    drawBaseGraph();
    fitToView();
}

void GraphVisuals::visualizeStep(const std::vector<int>& vertices,
                                 const std::vector<std::pair<int,int>>& edges)
{
    scene->clear();
    size_t n = graph->nodeCount();
    if (n == 0) return;

    if (positions.size() != n) {
        adjustPositionsForNewNodes();
        savePositions();
    }

    // Рисуем все рёбра (один раз)
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (graph->getWeight((int)i, (int)j) != -1) {
                drawEdge((int)i, (int)j, Qt::gray, 1);
            }
        }
    }

    // Рисуем вершины
    for (size_t i = 0; i < n; ++i) {
        QPointF center = positions[i];
        bool highlighted = false;
        for (int v : vertices) {
            if (v == (int)i) {
                highlighted = true;
                break;
            }
        }
        if (highlighted) {
            scene->addEllipse(center.x() - 25, center.y() - 25, 50, 50,
                              QPen(Qt::red, 3), QBrush(QColor("#FFCDD2")));
        } else {
            scene->addEllipse(center.x() - 25, center.y() - 25, 50, 50,
                              QPen(QColor("#9C27B0"), 2), QBrush(Qt::white));
        }
        QGraphicsTextItem *label = scene->addText(graph->nodeNames()[(int)i]);
        label->setFont(QFont("Arial", 12, QFont::Bold));
        QRectF rect = label->boundingRect();
        label->setPos(center.x() - rect.width() / 2, center.y() - rect.height() / 2);
    }

    // Подсвеченные рёбра
    for (const auto& e : edges) {
        drawEdge(e.first, e.second, Qt::red, 3);
    }

    fitToView();
}

void GraphVisuals::drawBaseGraph()
{
    size_t n = graph->nodeCount();

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (graph->getWeight((int)i, (int)j) != -1) {
                drawEdge((int)i, (int)j, QColor("#2196F3"), 2);
            }
        }
    }

    for (size_t i = 0; i < n; ++i) {
        QPointF center = positions[i];
        scene->addEllipse(center.x() - 25, center.y() - 25, 50, 50,
                          QPen(QColor("#9C27B0"), 2), QBrush(Qt::white));
        QGraphicsTextItem *label = scene->addText(graph->nodeNames()[(int)i]);
        label->setFont(QFont("Arial", 12, QFont::Bold));
        QRectF rect = label->boundingRect();
        label->setPos(center.x() - rect.width() / 2, center.y() - rect.height() / 2);
    }
}

void GraphVisuals::adjustPositionsForNewNodes()
{
    size_t oldSize = positions.size();
    size_t newSize = graph->nodeCount();

    if (oldSize == newSize) return;

    if (oldSize == 0) {
        assignCircularPositions();
        return;
    }

    // Сохраняем старые позиции
    std::vector<QPointF> oldPositions = positions;
    positions.resize(newSize);

    // Копируем старые позиции для существующих индексов
    for (size_t i = 0; i < oldSize && i < newSize; ++i) {
        positions[i] = oldPositions[i];
    }

    // Для новых вершин находим свободные места
    if (newSize > oldSize) {
        // Вычисляем центр масс старых вершин
        QPointF center(0, 0);
        for (size_t i = 0; i < oldSize; ++i) {
            center += oldPositions[i];
        }
        if (oldSize > 0) {
            center /= oldSize;
        }

        // Определяем радиус для новых вершин
        double maxRadius = 150.0;
        for (size_t i = 0; i < oldSize; ++i) {
            double r = sqrt(pow(oldPositions[i].x() - center.x(), 2) +
                            pow(oldPositions[i].y() - center.y(), 2));
            if (r > maxRadius) maxRadius = r;
        }
        maxRadius += 80;  // Увеличиваем отступ

        // Размещаем новые вершины на окружности
        int newNodesCount = newSize - oldSize;
        double angleStep = 2 * M_PI / newNodesCount;
        double startAngle = 0;

        // Ищем угол, где нет старых вершин
        for (int i = 0; i < newNodesCount; ++i) {
            double angle = startAngle + i * angleStep;
            QPointF newPos = center + QPointF(maxRadius * cos(angle), maxRadius * sin(angle));

            // Проверяем, не слишком ли близко к существующим вершинам
            bool tooClose = true;
            int attempts = 0;
            while (tooClose && attempts < 36) { // максимум 36 попыток (каждые 10 градусов)
                tooClose = false;
                for (size_t j = 0; j < oldSize; ++j) {
                    double dist = sqrt(pow(newPos.x() - oldPositions[j].x(), 2) +
                                       pow(newPos.y() - oldPositions[j].y(), 2));
                    if (dist < 70) { // минимальное расстояние
                        tooClose = true;
                        angle += M_PI / 18; // сдвиг на 10 градусов
                        newPos = center + QPointF(maxRadius * cos(angle), maxRadius * sin(angle));
                        break;
                    }
                }
                attempts++;
            }
            positions[oldSize + i] = newPos;
        }
    }
}

void GraphVisuals::assignCircularPositions()
{
    size_t n = graph->nodeCount();
    positions.resize(n);

    if (n == 0) return;

    // Специальная расстановка для 6 вершин (ваш вариант)
    if (n == 6) {
        positions[5] = QPointF(-70, -180);   // вершина 6
        positions[0] = QPointF(100, -100);   // вершина 1
        positions[4] = QPointF(-180, -50);   // вершина 5
        positions[2] = QPointF(220, 30);     // вершина 3
        positions[3] = QPointF(-150, 120);   // вершина 4
        positions[1] = QPointF(0, 180);      // вершина 2
    }
    else if (n == 1) {
        positions[0] = QPointF(0, 0);
    }
    else if (n == 2) {
        positions[0] = QPointF(-120, 0);
        positions[1] = QPointF(120, 0);
    }
    else if (n == 3) {
        positions[0] = QPointF(0, -120);
        positions[1] = QPointF(-104, 60);
        positions[2] = QPointF(104, 60);
    }
    else if (n == 4) {
        positions[0] = QPointF(0, -120);
        positions[1] = QPointF(120, 0);
        positions[2] = QPointF(0, 120);
        positions[3] = QPointF(-120, 0);
    }
    else if (n == 5) {
        double radius = 140.0;
        QPointF center(0, 0);
        for (size_t i = 0; i < n; ++i) {
            double angle = 2.0 * M_PI * i / n - M_PI / 2;
            positions[i] = center + QPointF(radius * cos(angle), radius * sin(angle));
        }
    }
    else if (n >= 7) {
        double radius = 180.0;
        QPointF center(0, 0);
        for (size_t i = 0; i < n; ++i) {
            double angle = 2.0 * M_PI * i / n;
            positions[i] = center + QPointF(radius * cos(angle), radius * sin(angle));
        }
    }
}

void GraphVisuals::savePositions()
{
    QFile file("positions.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << positions.size() << "\n";
        for (const auto& pos : positions) {
            out << pos.x() << " " << pos.y() << "\n";
        }
        file.close();
    }
}

void GraphVisuals::loadPositions()
{
    QFile file("positions.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        int count;
        in >> count;
        if (count == (int)graph->nodeCount()) {
            positions.resize(count);
            for (int i = 0; i < count; ++i) {
                double x, y;
                in >> x >> y;
                positions[i] = QPointF(x, y);
            }
        }
        file.close();
    }
}

void GraphVisuals::fitToView()
{
    if (scene->items().isEmpty()) {
        view->fitInView(QRectF(-300, -300, 600, 600), Qt::KeepAspectRatio);
        return;
    }

    QRectF boundingRect;
    for (QGraphicsItem* item : scene->items()) {
        boundingRect = boundingRect.united(item->sceneBoundingRect());
    }

    boundingRect = boundingRect.adjusted(-50, -50, 50, 50);
    view->setSceneRect(boundingRect);
    view->fitInView(boundingRect, Qt::KeepAspectRatio);
}

void GraphVisuals::drawEdge(int from, int to, QColor color, int boldness)
{
    if (from == to) return;

    QPointF pFrom = positions[from];
    QPointF pTo = positions[to];
    QLineF line(pFrom, pTo);

    double angle = atan2(line.dy(), line.dx());

    QPointF start = pFrom + QPointF(25 * cos(angle), 25 * sin(angle));
    QPointF end   = pTo   - QPointF(25 * cos(angle), 25 * sin(angle));

    scene->addLine(QLineF(start, end), QPen(color, boldness));

    int w = graph->getWeight(from, to);
    if (w != -1) {
        QPointF mid = (start + end) / 2.0;
        QPointF dir = end - start;
        double len = sqrt(dir.x() * dir.x() + dir.y() * dir.y());
        if (len > 0.01) {
            QPointF unit = dir / len;
            QPointF perp(-unit.y(), unit.x());
            double offset = 18.0;
            QPointF textPos = mid + perp * offset;

            QGraphicsTextItem *txt = scene->addText(QString::number(w));
            txt->setDefaultTextColor(QColor("#E91E63"));
            txt->setFont(QFont("Arial", 11, QFont::Bold));

            QRectF r = txt->boundingRect();
            QGraphicsRectItem *background = scene->addRect(
                textPos.x() - r.width() / 2 - 2,
                textPos.y() - r.height() / 2 - 2,
                r.width() + 4,
                r.height() + 4,
                QPen(Qt::NoPen),
                QBrush(QColor(255, 255, 255, 200))
                );

            txt->setPos(textPos.x() - r.width() / 2, textPos.y() - r.height() / 2);

            txt->setZValue(1);
            background->setZValue(0);
        }
    }
}
