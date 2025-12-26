#include "PlantUMLParser.hpp"
#include <QRegularExpression>
#include <QDebug>

namespace architect {

ParsedDiagram PlantUMLParser::parse(const QString& source) {
    ParsedDiagram diagram;
    QString currentContainer;
    int braceDepth = 0;
    QVector<QString> containerStack;
    
    const QStringList lines = source.split('\n');
    
    for (const QString& rawLine : lines) {
        QString line = rawLine.trimmed();
        
        // Skip empty lines and comments
        if (line.isEmpty() || line.startsWith("'") || line.startsWith("//")) {
            continue;
        }
        
        // Skip PlantUML directives
        if (line.startsWith("@start") || line.startsWith("@end") || 
            line.startsWith("skinparam") || line.startsWith("hide") ||
            line.startsWith("!") || line.startsWith("left to right") ||
            line.startsWith("top to bottom")) {
            continue;
        }
        
        // Parse title
        if (line.startsWith("title ")) {
            diagram.title = line.mid(6).trimmed();
            continue;
        }
        
        // Track brace depth for containers
        int openBraces = line.count('{');
        int closeBraces = line.count('}');
        
        // Check for container opening (rectangle, cloud, node with braces)
        ParsedNode container;
        if (parseContainer(line, container)) {
            container.parentId = containerStack.isEmpty() ? "" : containerStack.last();
            diagram.nodes.append(container);
            
            if (line.contains('{')) {
                containerStack.append(container.id);
            }
            braceDepth += openBraces;
            braceDepth -= closeBraces;
            continue;
        }
        
        // Check for simple node
        ParsedNode node;
        if (parseNode(line, node)) {
            node.parentId = containerStack.isEmpty() ? "" : containerStack.last();
            diagram.nodes.append(node);
            
            if (!node.parentId.isEmpty()) {
                diagram.containerChildren[node.id] = node.parentId;
            }
            continue;
        }
        
        // Check for edge
        ParsedEdge edge;
        if (parseEdge(line, edge)) {
            diagram.edges.append(edge);
            continue;
        }
        
        // Handle closing braces
        if (line == "}" || line.endsWith("}")) {
            braceDepth -= closeBraces;
            for (int i = 0; i < closeBraces && !containerStack.isEmpty(); ++i) {
                containerStack.removeLast();
            }
        }
        
        braceDepth += openBraces;
    }
    
    return diagram;
}

bool PlantUMLParser::parseNode(const QString& line, ParsedNode& node) {
    // Match: component "Label" as ALIAS
    // Match: database "Label" as ALIAS
    // Match: storage "Label" as ALIAS
    // Match: queue "Label" as ALIAS
    
    static QRegularExpression nodePattern(
        "^\\s*(component|database|storage|queue|actor|interface|artifact)\\s+"
        "\"([^\"]+)\"\\s+as\\s+(\\w+)",
        QRegularExpression::CaseInsensitiveOption
    );
    
    static QRegularExpression nodePatternNoQuotes(
        "^\\s*(component|database|storage|queue|actor|interface|artifact)\\s+"
        "(\\w+)\\s+as\\s+(\\w+)",
        QRegularExpression::CaseInsensitiveOption
    );
    
    QRegularExpressionMatch match = nodePattern.match(line);
    if (match.hasMatch()) {
        node.type = match.captured(1).toLower();
        node.label = match.captured(2);
        node.id = match.captured(3);
        return true;
    }
    
    match = nodePatternNoQuotes.match(line);
    if (match.hasMatch()) {
        node.type = match.captured(1).toLower();
        node.label = match.captured(2);
        node.id = match.captured(3);
        return true;
    }
    
    return false;
}

bool PlantUMLParser::parseContainer(const QString& line, ParsedNode& container) {
    // Match: rectangle "Label" as ALIAS {
    // Match: cloud "Label" as ALIAS {
    // Match: node "Label" as ALIAS {
    // Match: package "Label" as ALIAS {
    
    static QRegularExpression containerPattern(
        "^\\s*(rectangle|cloud|node|package|frame|folder)\\s+"
        "\"([^\"]+)\"\\s+as\\s+(\\w+)\\s*\\{?",
        QRegularExpression::CaseInsensitiveOption
    );
    
    static QRegularExpression containerPatternNoAlias(
        "^\\s*(rectangle|cloud|node|package|frame|folder)\\s+"
        "\"([^\"]+)\"\\s*\\{?",
        QRegularExpression::CaseInsensitiveOption
    );
    
    QRegularExpressionMatch match = containerPattern.match(line);
    if (match.hasMatch()) {
        container.type = match.captured(1).toLower();
        container.label = match.captured(2);
        container.id = match.captured(3);
        return true;
    }
    
    match = containerPatternNoAlias.match(line);
    if (match.hasMatch()) {
        container.type = match.captured(1).toLower();
        container.label = match.captured(2);
        // Generate ID from label (remove spaces, uppercase)
        container.id = match.captured(2).toUpper().replace(' ', '_').replace(QRegularExpression("[^A-Z0-9_]"), "");
        return true;
    }
    
    return false;
}

bool PlantUMLParser::parseEdge(const QString& line, ParsedEdge& edge) {
    // Match: FROM --> TO : label
    // Match: FROM -> TO
    // Match: FROM -- TO
    // Match: FROM ..> TO
    
    static QRegularExpression edgePattern(
        "^\\s*(\\w+)\\s*([-\\.]+>|[-\\.]+|<[-\\.]+)\\s*(\\w+)(?:\\s*:\\s*(.+))?",
        QRegularExpression::CaseInsensitiveOption
    );
    
    QRegularExpressionMatch match = edgePattern.match(line);
    if (match.hasMatch()) {
        edge.fromId = match.captured(1);
        edge.arrowType = match.captured(2);
        edge.toId = match.captured(3);
        edge.label = match.captured(4).trimmed();
        return true;
    }
    
    return false;
}

QString PlantUMLParser::extractQuotedString(const QString& text, int startPos, int& endPos) {
    int quoteStart = text.indexOf('"', startPos);
    if (quoteStart == -1) {
        endPos = startPos;
        return QString();
    }
    
    int quoteEnd = text.indexOf('"', quoteStart + 1);
    if (quoteEnd == -1) {
        endPos = text.length();
        return text.mid(quoteStart + 1);
    }
    
    endPos = quoteEnd + 1;
    return text.mid(quoteStart + 1, quoteEnd - quoteStart - 1);
}

} // namespace architect
