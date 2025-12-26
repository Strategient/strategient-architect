#include "DotParser.hpp"
#include <QRegularExpression>
#include <QDebug>

namespace architect {

ParsedDiagram DotParser::parse(const QString& source) {
    ParsedDiagram diagram;
    diagram.graphType = "digraph";  // Default
    m_knownNodes.clear();
    
    QVector<QString> clusterStack;
    int braceDepth = 0;
    
    const QStringList lines = source.split('\n');
    
    for (const QString& rawLine : lines) {
        QString line = rawLine.trimmed();
        
        // Skip empty lines and comments
        if (line.isEmpty() || line.startsWith("//") || line.startsWith("#")) {
            continue;
        }
        
        // Skip /* */ block comments (simplified - doesn't handle multi-line)
        if (line.startsWith("/*") && line.contains("*/")) {
            continue;
        }
        
        // Check for graph declaration (digraph/graph)
        if (parseGraphDeclaration(line, diagram)) {
            if (line.contains('{')) {
                braceDepth++;
            }
            continue;
        }
        
        // Check for subgraph/cluster
        ParsedNode cluster;
        if (parseSubgraph(line, cluster)) {
            cluster.parentId = clusterStack.isEmpty() ? "" : clusterStack.last();
            diagram.nodes.append(cluster);
            m_knownNodes[cluster.id] = true;
            
            if (line.contains('{')) {
                clusterStack.append(cluster.id);
                braceDepth++;
            }
            continue;
        }
        
        // Track braces
        int openBraces = line.count('{');
        int closeBraces = line.count('}');
        
        // Check for edge definition (must check before node to avoid false matches)
        ParsedEdge edge;
        if (parseEdge(line, edge)) {
            diagram.edges.append(edge);
            
            // Add implicit nodes if not already known
            if (!m_knownNodes.contains(edge.fromId)) {
                ParsedNode implicitNode;
                implicitNode.id = edge.fromId;
                implicitNode.label = edge.fromId;
                implicitNode.type = "box";
                implicitNode.parentId = clusterStack.isEmpty() ? "" : clusterStack.last();
                diagram.nodes.append(implicitNode);
                m_knownNodes[edge.fromId] = true;
            }
            if (!m_knownNodes.contains(edge.toId)) {
                ParsedNode implicitNode;
                implicitNode.id = edge.toId;
                implicitNode.label = edge.toId;
                implicitNode.type = "box";
                implicitNode.parentId = clusterStack.isEmpty() ? "" : clusterStack.last();
                diagram.nodes.append(implicitNode);
                m_knownNodes[edge.toId] = true;
            }
            continue;
        }
        
        // Check for node definition
        ParsedNode node;
        if (parseNode(line, node)) {
            node.parentId = clusterStack.isEmpty() ? "" : clusterStack.last();
            
            // Update existing implicit node or add new one
            bool found = false;
            for (int i = 0; i < diagram.nodes.size(); ++i) {
                if (diagram.nodes[i].id == node.id) {
                    // Update with explicit definition
                    diagram.nodes[i].label = node.label;
                    diagram.nodes[i].type = node.type;
                    diagram.nodes[i].parentId = node.parentId;
                    found = true;
                    break;
                }
            }
            if (!found) {
                diagram.nodes.append(node);
            }
            m_knownNodes[node.id] = true;
            
            if (!node.parentId.isEmpty()) {
                diagram.containerChildren[node.id] = node.parentId;
            }
            continue;
        }
        
        // Handle closing braces
        if (closeBraces > 0) {
            for (int i = 0; i < closeBraces && !clusterStack.isEmpty(); ++i) {
                clusterStack.removeLast();
            }
            braceDepth -= closeBraces;
        }
        
        braceDepth += openBraces;
    }
    
    return diagram;
}

bool DotParser::parseGraphDeclaration(const QString& line, ParsedDiagram& diagram) {
    // Match: digraph G { or strict digraph G {
    // Match: graph G { or strict graph G {
    static QRegularExpression graphPattern(
        "^\\s*(strict\\s+)?(digraph|graph)\\s+(\\w+)?\\s*\\{?",
        QRegularExpression::CaseInsensitiveOption
    );
    
    QRegularExpressionMatch match = graphPattern.match(line);
    if (match.hasMatch()) {
        diagram.graphType = match.captured(2).toLower();
        if (!match.captured(3).isEmpty()) {
            diagram.title = match.captured(3);
        }
        return true;
    }
    
    return false;
}

bool DotParser::parseSubgraph(const QString& line, ParsedNode& cluster) {
    // Match: subgraph cluster_name {
    // Match: subgraph cluster_name [label="Label"] {
    static QRegularExpression subgraphPattern(
        "^\\s*subgraph\\s+(cluster_)?(\\w+)\\s*(?:\\[([^\\]]*)\\])?\\s*\\{?",
        QRegularExpression::CaseInsensitiveOption
    );
    
    QRegularExpressionMatch match = subgraphPattern.match(line);
    if (match.hasMatch()) {
        QString prefix = match.captured(1);
        QString name = match.captured(2);
        QString attrs = match.captured(3);
        
        cluster.id = prefix.isEmpty() ? name : (prefix + name);
        cluster.type = "rectangle";  // Clusters render as rectangles
        
        // Extract label from attributes
        QString label = extractAttribute(attrs, "label");
        cluster.label = label.isEmpty() ? name : label;
        
        return true;
    }
    
    return false;
}

bool DotParser::parseNode(const QString& line, ParsedNode& node) {
    // Skip lines that are graph attributes or keywords
    static QRegularExpression skipPattern(
        "^\\s*(graph|node|edge|rankdir|splines|overlap|nodesep|ranksep)\\s*\\[",
        QRegularExpression::CaseInsensitiveOption
    );
    if (skipPattern.match(line).hasMatch()) {
        return false;
    }
    
    // Skip graph attribute assignments (no brackets, just key=value)
    static QRegularExpression graphAttrPattern(
        "^\\s*(rankdir|splines|overlap|nodesep|ranksep|bgcolor|fontname|fontsize)\\s*=",
        QRegularExpression::CaseInsensitiveOption
    );
    if (graphAttrPattern.match(line).hasMatch()) {
        return false;
    }
    
    // Match: node_id [attributes]
    // Match: "node id" [attributes]
    // Match: node_id; (simple declaration)
    static QRegularExpression nodePattern(
        "^\\s*(\"[^\"]+\"|\\w+)\\s*(?:\\[([^\\]]*)\\])?\\s*;?\\s*$"
    );
    
    QRegularExpressionMatch match = nodePattern.match(line);
    if (match.hasMatch()) {
        QString nodeId = match.captured(1);
        QString attrs = match.captured(2);
        
        // Remove quotes from node ID
        if (nodeId.startsWith('"') && nodeId.endsWith('"')) {
            nodeId = nodeId.mid(1, nodeId.length() - 2);
        }
        
        // Don't match keywords as nodes
        static QStringList keywords = {"graph", "node", "edge", "digraph", "subgraph", "strict"};
        if (keywords.contains(nodeId.toLower())) {
            return false;
        }
        
        node.id = nodeId;
        
        // Extract label attribute
        QString label = extractAttribute(attrs, "label");
        node.label = label.isEmpty() ? nodeId : label;
        
        // Extract shape attribute
        QString shape = extractAttribute(attrs, "shape");
        node.type = shape.isEmpty() ? "box" : shapeToType(shape);
        
        // Extract image attribute for custom icons
        node.imagePath = extractAttribute(attrs, "image");
        
        return true;
    }
    
    return false;
}

bool DotParser::parseEdge(const QString& line, ParsedEdge& edge) {
    // Match: from -> to [attributes]
    // Match: from -- to [attributes]
    // Match: "from node" -> "to node" [attributes]
    static QRegularExpression edgePattern(
        "^\\s*(\"[^\"]+\"|\\w+)\\s*(->|--)\\s*(\"[^\"]+\"|\\w+)\\s*(?:\\[([^\\]]*)\\])?",
        QRegularExpression::CaseInsensitiveOption
    );
    
    QRegularExpressionMatch match = edgePattern.match(line);
    if (match.hasMatch()) {
        QString fromId = match.captured(1);
        QString arrowType = match.captured(2);
        QString toId = match.captured(3);
        QString attrs = match.captured(4);
        
        // Remove quotes
        if (fromId.startsWith('"') && fromId.endsWith('"')) {
            fromId = fromId.mid(1, fromId.length() - 2);
        }
        if (toId.startsWith('"') && toId.endsWith('"')) {
            toId = toId.mid(1, toId.length() - 2);
        }
        
        edge.fromId = fromId;
        edge.toId = toId;
        edge.arrowType = arrowType;
        edge.label = extractAttribute(attrs, "label");
        
        return true;
    }
    
    return false;
}

QString DotParser::extractAttribute(const QString& attrList, const QString& attrName) {
    if (attrList.isEmpty()) {
        return QString();
    }
    
    // Match: attrName="value" or attrName=value
    QRegularExpression pattern(
        QString("%1\\s*=\\s*(?:\"([^\"]*)\"|([^,;\\]\\s]+))").arg(attrName),
        QRegularExpression::CaseInsensitiveOption
    );
    
    QRegularExpressionMatch match = pattern.match(attrList);
    if (match.hasMatch()) {
        // Return quoted value if present, otherwise unquoted
        QString quoted = match.captured(1);
        QString unquoted = match.captured(2);
        return quoted.isEmpty() ? unquoted : quoted;
    }
    
    return QString();
}

QString DotParser::shapeToType(const QString& shape) {
    // Map DOT shapes to internal types
    static QMap<QString, QString> shapeMap = {
        {"box", "component"},
        {"rect", "component"},
        {"rectangle", "component"},
        {"component", "component"},
        {"ellipse", "ellipse"},
        {"oval", "ellipse"},
        {"circle", "circle"},
        {"cylinder", "database"},
        {"database", "database"},
        {"folder", "folder"},
        {"note", "note"},
        {"tab", "tab"},
        {"box3d", "box3d"},
        {"diamond", "diamond"},
        {"hexagon", "hexagon"},
        {"octagon", "octagon"},
        {"house", "house"},
        {"cloud", "cloud"},
        {"star", "star"},
        {"none", "none"},
        {"plaintext", "plaintext"},
        {"plain", "plaintext"}
    };
    
    QString shapeLower = shape.toLower();
    return shapeMap.value(shapeLower, "component");
}

} // namespace architect

