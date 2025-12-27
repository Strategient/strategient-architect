#ifndef LAYOUTOPTIMIZER_HPP
#define LAYOUTOPTIMIZER_HPP

#include <QString>
#include <QStringList>

namespace architect {

/**
 * Structural metrics extracted from DOT graph analysis.
 */
struct GraphMetrics {
    int nodeCount{0};
    int edgeCount{0};
    int clusterCount{0};
    int crossClusterEdges{0};     // Edges connecting nodes in different clusters
    bool isDirected{true};        // digraph vs graph
    bool hasBackEdges{false};     // Edges pointing "backwards" in DAG sense
    double density{0.0};          // edge_count / (node_count * (node_count-1))
    
    // Computed properties
    bool isDAG() const { return isDirected && !hasBackEdges; }
    bool isLargeGraph() const { return nodeCount > 50; }
    bool isMediumGraph() const { return nodeCount > 15 && nodeCount <= 50; }
    bool isSmallGraph() const { return nodeCount <= 15; }
    bool isDense() const { return density > 0.3; }
    bool isClusterHeavy() const { return clusterCount >= 3 && (double)clusterCount / nodeCount > 0.1; }
};

/**
 * Layout configuration determined by the optimizer.
 */
struct LayoutConfig {
    QString engine{"dot"};        // Layout engine: dot, neato, fdp, sfdp, circo, twopi, osage
    QString rankdir{"TB"};        // TB, LR, BT, RL
    double nodesep{0.5};          // Minimum space between nodes
    double ranksep{0.5};          // Minimum space between ranks (for dot)
    QString splines{"ortho"};     // ortho, spline, polyline, line, true, false
    bool concentrate{false};      // Merge parallel edges
    QString overlap{"false"};     // Node overlap handling for fdp/neato
    
    // Convert to DOT attribute string for injection
    QString toAttributeString() const;
};

/**
 * LayoutOptimizer - Analyzes DOT graphs and determines optimal layout configuration.
 * 
 * This class provides automatic layout engine selection based on graph structure:
 * - sfdp: Large, dense, or mixed graphs (scalable force-directed)
 * - fdp: Medium-sized, loosely structured graphs
 * - dot: Strict DAGs with minimal cross edges (hierarchical)
 * - osage: Cluster-heavy graphs (array-based packing)
 * 
 * It also injects appropriate layout attributes based on graph size.
 */
class LayoutOptimizer {
public:
    LayoutOptimizer() = default;
    
    /**
     * Analyze DOT source and compute structural metrics.
     */
    GraphMetrics analyzeGraph(const QString& dotSource);
    
    /**
     * Determine optimal layout configuration based on metrics.
     */
    LayoutConfig selectLayout(const GraphMetrics& metrics);
    
    /**
     * Full pipeline: analyze graph and return optimized DOT with injected attributes.
     * @param dotSource Original DOT source
     * @return Modified DOT source with layout attributes injected
     */
    QString optimizeDot(const QString& dotSource);
    
    /**
     * Get the last computed metrics (for debugging/display).
     */
    GraphMetrics lastMetrics() const { return m_lastMetrics; }
    
    /**
     * Get the last selected configuration (for debugging/display).
     */
    LayoutConfig lastConfig() const { return m_lastConfig; }

private:
    // Parsing helpers
    void extractNodes(const QString& source, QStringList& nodes);
    void extractEdges(const QString& source, QStringList& edges);
    void extractClusters(const QString& source, QStringList& clusters);
    bool detectBackEdges(const QString& source, const QStringList& nodes);
    int countCrossClusterEdges(const QString& source, const QStringList& clusters);
    
    // Attribute injection
    QString injectAttributes(const QString& source, const LayoutConfig& config);
    
    GraphMetrics m_lastMetrics;
    LayoutConfig m_lastConfig;
};

} // namespace architect

#endif // LAYOUTOPTIMIZER_HPP

