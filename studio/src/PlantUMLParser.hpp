#ifndef PLANTUMLPARSER_HPP
#define PLANTUMLPARSER_HPP

#include <QString>
#include <QVector>
#include <QMap>

namespace architect {

// Represents a parsed node from PlantUML
struct ParsedNode {
    QString id;           // The alias (e.g., "MARKET_FEED")
    QString label;        // Display label (e.g., "Market Data Feed")
    QString type;         // "component", "database", "cloud", "storage", "queue", "node", "rectangle"
    QString parentId;     // Parent container if nested
};

// Represents a parsed edge/relationship from PlantUML
struct ParsedEdge {
    QString fromId;
    QString toId;
    QString label;        // Optional edge label
    QString arrowType;    // "-->", "->", "--", etc.
};

// Result of parsing PlantUML
struct ParsedDiagram {
    QString title;
    QVector<ParsedNode> nodes;
    QVector<ParsedEdge> edges;
    QMap<QString, QString> containerChildren;  // Maps child ID to parent ID
};

/**
 * PlantUMLParser - Parses a subset of PlantUML for component-style diagrams.
 * 
 * Supported syntax:
 *   - component "Label" as ALIAS
 *   - database "Label" as ALIAS
 *   - cloud "Label" as ALIAS { ... }
 *   - node "Label" as ALIAS { ... }
 *   - rectangle "Label" as ALIAS { ... }
 *   - storage "Label" as ALIAS
 *   - queue "Label" as ALIAS
 *   - ALIAS --> ALIAS : label
 *   - ALIAS -> ALIAS
 *   - title TitleText
 */
class PlantUMLParser {
public:
    PlantUMLParser() = default;
    
    // Parse PlantUML source and return parsed diagram
    ParsedDiagram parse(const QString& source);
    
private:
    void parseLine(const QString& line, ParsedDiagram& diagram, QString& currentContainer);
    bool parseNode(const QString& line, ParsedNode& node);
    bool parseEdge(const QString& line, ParsedEdge& edge);
    bool parseContainer(const QString& line, ParsedNode& container);
    
    // Helper to extract quoted string
    QString extractQuotedString(const QString& text, int startPos, int& endPos);
};

} // namespace architect

#endif // PLANTUMLPARSER_HPP

