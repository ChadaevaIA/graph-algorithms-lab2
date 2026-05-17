#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <queue>
#include <memory>
#include <QString>

class GraphStorage;

enum class StorageType {
    AdjacencyMatrix,
    AdjacencyList
};

class Graph {
public:
    Graph();
    ~Graph();

    void setStorageType(StorageType type);
    StorageType storageType() const;

    void addNode(const QString &name = "");
    void removeNode(int index);
    void addEdge(int from, int to, int weight);
    void removeEdge(int from, int to);
    void setWeight(int from, int to, int weight);
    int getWeight(int from, int to) const;
    int getNodeIndex(const QString& name) const;

    std::vector<int> dfs(int start) const;
    std::vector<int> bfs(int start) const;
    std::pair<std::vector<int>, std::vector<int>> dijkstra(int start) const;
    std::vector<std::vector<int>> floyd() const;

    size_t nodeCount() const;
    const std::vector<QString>& nodeNames() const;

private:
    std::unique_ptr<GraphStorage> storage_;
};

class GraphStorage {
public:
    virtual ~GraphStorage() = default;
    virtual void addNode(const QString& name) = 0;
    virtual void removeNode(int index) = 0;
    virtual void addEdge(int from, int to, int weight) = 0;
    virtual void removeEdge(int from, int to) = 0;
    virtual int getWeight(int from, int to) const = 0;
    virtual int getNodeIndex(const QString& name) const = 0;
    virtual size_t nodeCount() const = 0;
    virtual const std::vector<QString>& nodeNames() const = 0;

    struct GraphData {
        std::vector<QString> names;
        struct Edge { int from, to, weight; };
        std::vector<Edge> edges;
    };
    virtual GraphData exportData() const = 0;
};

class AdjMatrixStorage : public GraphStorage {
public:
    void addNode(const QString& name) override;
    void removeNode(int index) override;
    void addEdge(int from, int to, int weight) override;
    void removeEdge(int from, int to) override;
    int getWeight(int from, int to) const override;
    int getNodeIndex(const QString& name) const override;
    size_t nodeCount() const override;
    const std::vector<QString>& nodeNames() const override;
    GraphData exportData() const override;

private:
    std::vector<std::vector<int>> adjMatrix;
    std::vector<QString> names;
};

class AdjListStorage : public GraphStorage {
public:
    void addNode(const QString& name) override;
    void removeNode(int index) override;
    void addEdge(int from, int to, int weight) override;
    void removeEdge(int from, int to) override;
    int getWeight(int from, int to) const override;
    int getNodeIndex(const QString& name) const override;
    size_t nodeCount() const override;
    const std::vector<QString>& nodeNames() const override;
    GraphData exportData() const override;

private:
    std::vector<std::vector<std::pair<int, int>>> adjList;
    std::vector<QString> names;
};

#endif