#include "graph.h"
#include <climits>
#include <functional>
#include <algorithm>
#include <queue>
#include <stack>

const int INF = 1e9;

// ==================== AdjMatrixStorage ====================

void AdjMatrixStorage::addNode(const QString &name) {
    size_t newSize = adjMatrix.size() + 1;
    for (auto &row : adjMatrix)
        row.push_back(-1);
    adjMatrix.push_back(std::vector<int>(newSize, -1));
    adjMatrix.back().back() = 0;
    names.push_back(name.isEmpty() ? QString::number(newSize) : name);
}

int AdjMatrixStorage::getNodeIndex(const QString& name) const {
    for (size_t i = 0; i < names.size(); ++i)
        if (names[i] == name) return (int)i;
    return -1;
}

void AdjMatrixStorage::removeNode(int index) {
    size_t n = adjMatrix.size();
    if (index < 0 || index >= n) return;
    adjMatrix.erase(adjMatrix.begin() + index);
    for (auto &row : adjMatrix)
        row.erase(row.begin() + index);
    names.erase(names.begin() + index);
}

void AdjMatrixStorage::addEdge(int from, int to, int weight) {
    if (from >= 0 && from < (int)adjMatrix.size() &&
        to >= 0 && to < (int)adjMatrix.size() &&
        from != to) {
        adjMatrix[from][to] = weight;
        adjMatrix[to][from] = weight;
    }
}

void AdjMatrixStorage::removeEdge(int from, int to) {
    if (from >= 0 && from < (int)adjMatrix.size() &&
        to >= 0 && to < (int)adjMatrix.size()) {
        adjMatrix[from][to] = -1;
        adjMatrix[to][from] = -1;
    }
}
int AdjMatrixStorage::getWeight(int from, int to) const {
    if (from >= 0 && from < (int)adjMatrix.size() &&
        to >= 0 && to < (int)adjMatrix.size())
        return adjMatrix[from][to];
    return -1;
}

size_t AdjMatrixStorage::nodeCount() const {
    return adjMatrix.size();
}

const std::vector<QString>& AdjMatrixStorage::nodeNames() const {
    return names;
}

GraphStorage::GraphData AdjMatrixStorage::exportData() const {
    GraphData data;
    data.names = names;
    for (size_t i = 0; i < adjMatrix.size(); ++i)
        for (size_t j = i + 1; j < adjMatrix.size(); ++j)
            if (adjMatrix[i][j] != -1)
                data.edges.push_back({(int)i, (int)j, adjMatrix[i][j]});
    return data;
}

// ==================== AdjListStorage ====================

void AdjListStorage::addNode(const QString &name) {
    adjList.push_back({});
    names.push_back(name.isEmpty() ? QString::number(adjList.size()) : name);
}

int AdjListStorage::getNodeIndex(const QString& name) const {
    for (size_t i = 0; i < names.size(); ++i)
        if (names[i] == name) return (int)i;
    return -1;
}

void AdjListStorage::removeNode(int index) {
    if (index < 0 || index >= (int)adjList.size()) return;
    adjList.erase(adjList.begin() + index);
    names.erase(names.begin() + index);
    for (auto &neighbors : adjList) {
        neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
                                       [index](const std::pair<int,int>& p) { return p.first == index; }),
                        neighbors.end());
        for (auto &p : neighbors) {
            if (p.first > index) p.first--;
        }
    }
}

void AdjListStorage::addEdge(int from, int to, int weight) {
    if (from < 0 || from >= (int)adjList.size() ||
        to < 0 || to >= (int)adjList.size() || from == to) return;

    auto &neigh = adjList[from];
    auto it = std::find_if(neigh.begin(), neigh.end(),
                           [to](const std::pair<int,int>& p) { return p.first == to; });
    if (it != neigh.end())
        it->second = weight;
    else
        neigh.emplace_back(to, weight);

    auto &neighRev = adjList[to];
    auto itRev = std::find_if(neighRev.begin(), neighRev.end(),
                              [from](const std::pair<int,int>& p) { return p.first == from; });
    if (itRev != neighRev.end())
        itRev->second = weight;
    else
        neighRev.emplace_back(from, weight);
}

void AdjListStorage::removeEdge(int from, int to) {
    if (from < 0 || from >= (int)adjList.size() ||
        to < 0 || to >= (int)adjList.size()) return;

    auto &neigh = adjList[from];
    neigh.erase(std::remove_if(neigh.begin(), neigh.end(),
                               [to](const std::pair<int,int>& p) { return p.first == to; }),
                neigh.end());

    auto &neighRev = adjList[to];
    neighRev.erase(std::remove_if(neighRev.begin(), neighRev.end(),
                                  [from](const std::pair<int,int>& p) { return p.first == from; }),
                   neighRev.end());
}

int AdjListStorage::getWeight(int from, int to) const {
    if (from < 0 || from >= (int)adjList.size() ||
        to < 0 || to >= (int)adjList.size()) return -1;
    for (const auto &p : adjList[from])
        if (p.first == to) return p.second;
    return -1;
}

size_t AdjListStorage::nodeCount() const {
    return adjList.size();
}

const std::vector<QString>& AdjListStorage::nodeNames() const {
    return names;
}

GraphStorage::GraphData AdjListStorage::exportData() const {
    GraphData data;
    data.names = names;
    for (size_t i = 0; i < adjList.size(); ++i)
        for (const auto &p : adjList[i])
            if (p.first > (int)i)
                data.edges.push_back({(int)i, p.first, p.second});
    return data;
}

// ==================== Graph ====================

Graph::Graph() {
    storage_ = std::make_unique<AdjMatrixStorage>();
}

Graph::~Graph() = default;

void Graph::setStorageType(StorageType type) {
    if (storageType() == type) return;
    GraphStorage::GraphData data = storage_->exportData();
    std::unique_ptr<GraphStorage> newStorage;
    if (type == StorageType::AdjacencyMatrix)
        newStorage = std::make_unique<AdjMatrixStorage>();
    else
        newStorage = std::make_unique<AdjListStorage>();
    for (const auto& name : data.names)
        newStorage->addNode(name);
    for (const auto& e : data.edges)
        newStorage->addEdge(e.from, e.to, e.weight);
    storage_ = std::move(newStorage);
}

StorageType Graph::storageType() const {
    if (dynamic_cast<AdjMatrixStorage*>(storage_.get()))
        return StorageType::AdjacencyMatrix;
    return StorageType::AdjacencyList;
}

void Graph::addNode(const QString &name) { storage_->addNode(name); }
void Graph::removeNode(int index) { storage_->removeNode(index); }
void Graph::addEdge(int from, int to, int weight) { storage_->addEdge(from, to, weight); }
void Graph::removeEdge(int from, int to) { storage_->removeEdge(from, to); }
void Graph::setWeight(int from, int to, int weight) { storage_->addEdge(from, to, weight); }
int Graph::getWeight(int from, int to) const { return storage_->getWeight(from, to); }
int Graph::getNodeIndex(const QString& name) const { return storage_->getNodeIndex(name); }
size_t Graph::nodeCount() const { return storage_->nodeCount(); }
const std::vector<QString>& Graph::nodeNames() const { return storage_->nodeNames(); }

std::vector<int> Graph::dfs(int start) const {
    std::vector<int> order;
    std::vector<bool> visited(nodeCount(), false);
    std::function<void(int)> dfsRec = [&](int v) {
        visited[v] = true;
        order.push_back(v);
        for (size_t i = 0; i < nodeCount(); ++i) {
            if (getWeight(v, (int)i) != -1 && !visited[i])
                dfsRec((int)i);
        }
    };
    dfsRec(start);
    return order;
}

std::vector<int> Graph::bfs(int start) const {
    std::vector<int> order;
    size_t n = nodeCount();
    std::vector<bool> visited(n, false);
    std::queue<int> q;
    visited[start] = true;
    q.push(start);
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        order.push_back(cur);
        for (size_t i = 0; i < n; ++i) {
            if (getWeight(cur, (int)i) != -1 && !visited[i]) {
                visited[i] = true;
                q.push((int)i);
            }
        }
    }
    return order;
}

std::pair<std::vector<int>, std::vector<int>> Graph::dijkstra(int start) const {
    size_t n = nodeCount();
    std::vector<int> dist(n, INF);
    std::vector<int> parent(n, -1);
    std::vector<bool> used(n, false);
    dist[start] = 0;
    for (size_t i = 0; i < n; ++i) {
        int v = -1;
        for (size_t j = 0; j < n; ++j)
            if (!used[j] && (v == -1 || dist[j] < dist[v]))
                v = (int)j;
        if (dist[v] == INF) break;
        used[v] = true;
        for (size_t to = 0; to < n; ++to) {
            int w = getWeight(v, (int)to);
            if (w != -1) {
                int newDist = dist[v] + w;
                if (newDist < dist[to]) {
                    dist[to] = newDist;
                    parent[to] = v;
                }
            }
        }
    }
    return {dist, parent};
}

std::vector<std::vector<int>> Graph::floyd() const {
    size_t n = nodeCount();
    std::vector<std::vector<int>> dist(n, std::vector<int>(n, INF));
    for (size_t i = 0; i < n; ++i) {
        dist[i][i] = 0;
        for (size_t j = i + 1; j < n; ++j) {
            int w = getWeight((int)i, (int)j);
            if (w != -1) {
                dist[i][j] = w;
                dist[j][i] = w;
            }
        }
    }
    for (size_t k = 0; k < n; ++k)
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                if (dist[i][k] < INF && dist[k][j] < INF)
                    dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            if (dist[i][j] >= INF) dist[i][j] = -1;
    return dist;
}