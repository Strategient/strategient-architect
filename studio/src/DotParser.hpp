#ifndef DOTPARSER_HPP
#define DOTPARSER_HPP

#include <QString>
#include <QVector>
#include <QMap>

namespace architect {

// Represents a parsed node from DOT
struct ParsedNode {
    QString id;           // The node ID (e.g., "MARKET_FEED")
    QString label;        // Display label (from label attribute or id)
    QString type;         // Shape type: "box", "ellipse", "cylinder", etc.
    QString parentId;     // Parent subgraph/cluster if nested
    QString imagePath;    // Image attribute path for custom icons
};

// Represents a parsed edge/relationship from DOT
struct ParsedEdge {
    QString fromId;
    QString toId;
    QString label;        // Optional edge label
    QString arrowType;    // "->" (directed) or "--" (undirected)
};

// Result of parsing DOT
struct ParsedDiagram {
    QString title;
    QString graphType;    // "digraph" or "graph"
    QVector<ParsedNode> nodes;
    QVector<ParsedEdge> edges;
    QMap<QString, QString> containerChildren;  // Maps child ID to parent ID
};

/**
 * DotParser - Parses Graphviz DOT format for component-style diagrams.
 * 
 * Supported syntax:
 *   - digraph G { ... }
 *   - graph G { ... }
 *   - subgraph cluster_name { ... }
 *   - node_id [label="Label", shape=box]
 *   - node1 -> node2 [label="edge label"]
 *   - node1 -- node2
 *   - Graph attributes: label, rankdir, splines
 *   - Node attributes: label, shape, style, fillcolor, etc.
 *   - Edge attributes: label, style, color, etc.
 */
class DotParser {
public:
    DotParser() = default;
    
    // Parse DOT source and return parsed diagram
    ParsedDiagram parse(const QString& source);
    
private:
    void parseLine(const QString& line, ParsedDiagram& diagram, 
                   QVector<QString>& clusterStack);
    bool parseNode(const QString& line, ParsedNode& node);
    bool parseEdge(const QString& line, ParsedEdge& edge);
    bool parseSubgraph(const QString& line, ParsedNode& cluster);
    bool parseGraphDeclaration(const QString& line, ParsedDiagram& diagram);
    
    // Extract attribute value from attribute list
    QString extractAttribute(const QString& attrList, const QString& attrName);
    
    // Map DOT shape to internal type
    QString shapeToType(const QString& shape);
    
    // Track implicit nodes from edges
    QMap<QString, bool> m_knownNodes;
};

} // namespace architect

#endif // DOTPARSER_HPP

