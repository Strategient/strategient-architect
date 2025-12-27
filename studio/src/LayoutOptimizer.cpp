#include "LayoutOptimizer.hpp"
#include <QRegularExpression>
#include <QSet>
#include <QDebug>

namespace architect {

QString LayoutConfig::toAttributeString() const {
    QStringList attrs;
    
    // Graph-level attributes
    attrs << QString("rankdir=%1").arg(rankdir);
    attrs << QString("nodesep=%1").arg(nodesep, 0, 'f', 2);
    attrs << QString("ranksep=%1").arg(ranksep, 0, 'f', 2);
    attrs << QString("splines=%1").arg(splines);
    attrs << QString("overlap=%1").arg(overlap);
    
    if (concentrate) {
        attrs << "concentrate=true";
    }
    
    return attrs.join(";\n    ");
}

GraphMetrics LayoutOptimizer::analyzeGraph(const QString& dotSource) {
    GraphMetrics metrics;
    
    // Detect graph type (digraph vs graph)
    static QRegularExpression graphTypePattern(
        "^\\s*(strict\\s+)?(digraph|graph)\\s+",
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption
    );
    QRegularExpressionMatch typeMatch = graphTypePattern.match(dotSource);
    metrics.isDirected = typeMatch.hasMatch() && typeMatch.captured(2).toLower() == "digraph";
    
    // Extract structural elements
    QStringList nodes, edges, clusters;
    extractNodes(dotSource, nodes);
    extractEdges(dotSource, edges);
    extractClusters(dotSource, clusters);
    
    metrics.nodeCount = nodes.size();
    metrics.edgeCount = edges.size();
    metrics.clusterCount = clusters.size();
    
    // Compute density: edges / max_possible_edges
    // For directed: n*(n-1), for undirected: n*(n-1)/2
    if (metrics.nodeCount > 1) {
        double maxEdges = metrics.isDirected 
            ? metrics.nodeCount * (metrics.nodeCount - 1)
            : metrics.nodeCount * (metrics.nodeCount - 1) / 2.0;
        metrics.density = metrics.edgeCount / maxEdges;
    }
    
    // Detect back edges (cycles in directed graph)
    if (metrics.isDirected) {
        metrics.hasBackEdges = detectBackEdges(dotSource, nodes);
    }
    
    // Count cross-cluster edges
    if (metrics.clusterCount > 0) {
        metrics.crossClusterEdges = countCrossClusterEdges(dotSource, clusters);
    }
    
    m_lastMetrics = metrics;
    
    qDebug() << "LayoutOptimizer: Analyzed graph -"
             << "nodes:" << metrics.nodeCount
             << "edges:" << metrics.edgeCount
             << "clusters:" << metrics.clusterCount
             << "density:" << metrics.density
             << "directed:" << metrics.isDirected
             << "hasBackEdges:" << metrics.hasBackEdges
             << "crossClusterEdges:" << metrics.crossClusterEdges;
    
    return metrics;
}

LayoutConfig LayoutOptimizer::selectLayout(const GraphMetrics& metrics) {
    LayoutConfig config;
    
    // === Engine Selection Logic ===
    
    // 1. Cluster-heavy graphs → osage (array-based packing)
    if (metrics.isClusterHeavy() && metrics.crossClusterEdges < metrics.edgeCount * 0.3) {
        config.engine = "osage";
        qDebug() << "LayoutOptimizer: Selected 'osage' - cluster-heavy graph";
    }
    // 2. Large or dense graphs → sfdp (scalable force-directed)
    else if (metrics.isLargeGraph() || (metrics.isMediumGraph() && metrics.isDense())) {
        config.engine = "sfdp";
        qDebug() << "LayoutOptimizer: Selected 'sfdp' - large/dense graph";
    }
    // 3. Strict DAGs with few cross edges → dot (hierarchical)
    else if (metrics.isDAG() && metrics.crossClusterEdges < metrics.edgeCount * 0.2) {
        config.engine = "dot";
        qDebug() << "LayoutOptimizer: Selected 'dot' - strict DAG";
    }
    // 4. Medium graphs, loosely structured → fdp
    else if (metrics.isMediumGraph()) {
        config.engine = "fdp";
        qDebug() << "LayoutOptimizer: Selected 'fdp' - medium loosely-structured graph";
    }
    // 5. Small graphs → dot (usually looks best)
    else if (metrics.isSmallGraph()) {
        config.engine = "dot";
        qDebug() << "LayoutOptimizer: Selected 'dot' - small graph";
    }
    // 6. Default fallback → sfdp
    else {
        config.engine = "sfdp";
        qDebug() << "LayoutOptimizer: Selected 'sfdp' - default fallback";
    }
    
    // === Attribute Configuration ===
    
    // Rankdir: LR for pipeline-style, TB for hierarchical
    if (metrics.clusterCount > 2) {
        config.rankdir = "TB";  // Top-to-bottom for cluster layouts
    } else if (metrics.isDirected && metrics.isDAG()) {
        config.rankdir = "LR";  // Left-to-right for pipelines
    } else {
        config.rankdir = "TB";
    }
    
    // Node/rank separation scaled to graph size
    if (metrics.isLargeGraph()) {
        config.nodesep = 0.3;
        config.ranksep = 0.4;
    } else if (metrics.isMediumGraph()) {
        config.nodesep = 0.5;
        config.ranksep = 0.6;
    } else {
        config.nodesep = 0.8;
        config.ranksep = 1.0;
    }
    
    // Splines configuration
    if (metrics.isDense() || metrics.edgeCount > 100) {
        config.splines = "true";  // Curved splines for dense graphs (faster)
    } else if (config.engine == "osage") {
        config.splines = "polyline";  // osage works better with polyline
    } else {
        config.splines = "ortho";  // Orthogonal for clean diagrams
    }
    
    // Concentrate parallel edges in dense graphs
    config.concentrate = metrics.isDense() || metrics.edgeCount > 50;
    
    // Overlap handling for force-directed layouts
    if (config.engine == "fdp" || config.engine == "sfdp" || config.engine == "neato") {
        config.overlap = "prism";  // Good balance of speed and quality
    } else {
        config.overlap = "false";
    }
    
    m_lastConfig = config;
    
    qDebug() << "LayoutOptimizer: Config -"
             << "engine:" << config.engine
             << "rankdir:" << config.rankdir
             << "nodesep:" << config.nodesep
             << "ranksep:" << config.ranksep
             << "splines:" << config.splines
             << "concentrate:" << config.concentrate;
    
    return config;
}

QString LayoutOptimizer::optimizeDot(const QString& dotSource) {
    GraphMetrics metrics = analyzeGraph(dotSource);
    LayoutConfig config = selectLayout(metrics);
    return injectAttributes(dotSource, config);
}

void LayoutOptimizer::extractNodes(const QString& source, QStringList& nodes) {
    QSet<QString> nodeSet;
    
    // Pattern 1: Explicit node declarations: node_id [attrs] or node_id;
    static QRegularExpression nodePattern(
        "^\\s*(\"[^\"]+\"|[a-zA-Z_][a-zA-Z0-9_]*)\\s*(?:\\[[^\\]]*\\])?\\s*;?\\s*$",
        QRegularExpression::MultilineOption
    );
    
    // Pattern 2: Nodes from edges: from -> to or from -- to
    static QRegularExpression edgeNodePattern(
        "(\"[^\"]+\"|[a-zA-Z_][a-zA-Z0-9_]*)\\s*(?:->|--)\\s*(\"[^\"]+\"|[a-zA-Z_][a-zA-Z0-9_]*)"
    );
    
    // Keywords to exclude
    static QStringList keywords = {"graph", "node", "edge", "digraph", "subgraph", "strict", 
                                   "rankdir", "splines", "nodesep", "ranksep", "bgcolor", 
                                   "fontname", "fontsize", "label", "style", "fillcolor", "overlap"};
    
    // Extract from explicit declarations
    QRegularExpressionMatchIterator nodeMatches = nodePattern.globalMatch(source);
    while (nodeMatches.hasNext()) {
        QRegularExpressionMatch match = nodeMatches.next();
        QString nodeId = match.captured(1).remove('"');
        if (!keywords.contains(nodeId.toLower()) && !nodeId.startsWith("cluster_")) {
            nodeSet.insert(nodeId);
        }
    }
    
    // Extract from edges
    QRegularExpressionMatchIterator edgeMatches = edgeNodePattern.globalMatch(source);
    while (edgeMatches.hasNext()) {
        QRegularExpressionMatch match = edgeMatches.next();
        QString from = match.captured(1).remove('"');
        QString to = match.captured(2).remove('"');
        if (!keywords.contains(from.toLower())) nodeSet.insert(from);
        if (!keywords.contains(to.toLower())) nodeSet.insert(to);
    }
    
    nodes = nodeSet.values();
}

void LayoutOptimizer::extractEdges(const QString& source, QStringList& edges) {
    static QRegularExpression edgePattern(
        "(\"[^\"]+\"|[a-zA-Z_][a-zA-Z0-9_]*)\\s*(->|--)\\s*(\"[^\"]+\"|[a-zA-Z_][a-zA-Z0-9_]*)"
    );
    
    QRegularExpressionMatchIterator matches = edgePattern.globalMatch(source);
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        edges << QString("%1%2%3").arg(match.captured(1), match.captured(2), match.captured(3));
    }
}

void LayoutOptimizer::extractClusters(const QString& source, QStringList& clusters) {
    static QRegularExpression clusterPattern(
        "subgraph\\s+(cluster_[a-zA-Z0-9_]+)",
        QRegularExpression::CaseInsensitiveOption
    );
    
    QRegularExpressionMatchIterator matches = clusterPattern.globalMatch(source);
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        clusters << match.captured(1);
    }
}

bool LayoutOptimizer::detectBackEdges(const QString& source, const QStringList& nodes) {
    // Simple heuristic: look for edges that go "backwards" based on node order
    // This is approximate - true cycle detection would require building a graph
    
    static QRegularExpression edgePattern(
        "(\"[^\"]+\"|[a-zA-Z_][a-zA-Z0-9_]*)\\s*->\\s*(\"[^\"]+\"|[a-zA-Z_][a-zA-Z0-9_]*)"
    );
    
    // Create a simple ordering based on first appearance
    QMap<QString, int> nodeOrder;
    for (int i = 0; i < nodes.size(); ++i) {
        nodeOrder[nodes[i]] = i;
    }
    
    // Check for back edges (target appears before source in order)
    QRegularExpressionMatchIterator matches = edgePattern.globalMatch(source);
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString from = match.captured(1).remove('"');
        QString to = match.captured(2).remove('"');
        
        if (nodeOrder.contains(from) && nodeOrder.contains(to)) {
            // If destination has lower order, it's potentially a back edge
            if (nodeOrder[to] < nodeOrder[from]) {
                return true;
            }
        }
    }
    
    return false;
}

int LayoutOptimizer::countCrossClusterEdges(const QString& source, const QStringList& clusters) {
    if (clusters.isEmpty()) return 0;
    
    // Build map of node -> cluster membership (approximate)
    QMap<QString, QString> nodeCluster;
    
    for (const QString& cluster : clusters) {
        // Find cluster block and extract nodes within it
        QString clusterPattern = QString("subgraph\\s+%1\\s*\\{([^}]*)\\}").arg(QRegularExpression::escape(cluster));
        QRegularExpression re(clusterPattern, QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatch match = re.match(source);
        
        if (match.hasMatch()) {
            QString clusterContent = match.captured(1);
            
            // Extract node IDs from cluster content
            static QRegularExpression nodeInCluster(
                "^\\s*(\"[^\"]+\"|[a-zA-Z_][a-zA-Z0-9_]*)\\s*(?:\\[|;|$)",
                QRegularExpression::MultilineOption
            );
            
            QRegularExpressionMatchIterator nodeMatches = nodeInCluster.globalMatch(clusterContent);
            while (nodeMatches.hasNext()) {
                QRegularExpressionMatch nodeMatch = nodeMatches.next();
                QString nodeId = nodeMatch.captured(1).remove('"');
                
                // Skip keywords
                static QStringList keywords = {"label", "style", "fillcolor", "fontcolor", "node", "edge"};
                if (!keywords.contains(nodeId.toLower())) {
                    nodeCluster[nodeId] = cluster;
                }
            }
        }
    }
    
    // Count edges between different clusters
    int crossCount = 0;
    static QRegularExpression edgePattern(
        "(\"[^\"]+\"|[a-zA-Z_][a-zA-Z0-9_]*)\\s*(?:->|--)\\s*(\"[^\"]+\"|[a-zA-Z_][a-zA-Z0-9_]*)"
    );
    
    QRegularExpressionMatchIterator matches = edgePattern.globalMatch(source);
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString from = match.captured(1).remove('"');
        QString to = match.captured(2).remove('"');
        
        QString fromCluster = nodeCluster.value(from, "");
        QString toCluster = nodeCluster.value(to, "");
        
        // Cross-cluster if both in clusters but different ones
        if (!fromCluster.isEmpty() && !toCluster.isEmpty() && fromCluster != toCluster) {
            crossCount++;
        }
        // Or if one is in a cluster and the other isn't
        else if (fromCluster.isEmpty() != toCluster.isEmpty()) {
            crossCount++;
        }
    }
    
    return crossCount;
}

QString LayoutOptimizer::injectAttributes(const QString& source, const LayoutConfig& config) {
    QString result = source;
    
    // Find the opening brace of the main graph
    static QRegularExpression graphOpenPattern(
        "((?:strict\\s+)?(?:digraph|graph)\\s+[a-zA-Z_][a-zA-Z0-9_]*\\s*)\\{",
        QRegularExpression::CaseInsensitiveOption
    );
    
    QRegularExpressionMatch match = graphOpenPattern.match(source);
    if (match.hasMatch()) {
        int insertPos = match.capturedEnd();
        
        // Build attribute block
        QString attrs = QString("\n    // Auto-generated layout attributes (engine: %1)\n").arg(config.engine);
        attrs += "    " + config.toAttributeString() + ";\n";
        
        // Check if attributes already exist and should be overridden
        // For now, we prepend them (DOT uses last-defined value)
        result.insert(insertPos, attrs);
    }
    
    return result;
}

} // namespace architect

