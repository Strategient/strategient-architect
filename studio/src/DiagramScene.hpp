#ifndef DIAGRAMSCENE_HPP
#define DIAGRAMSCENE_HPP

#include <QGraphicsScene>
#include <QMap>
#include "PlantUMLParser.hpp"
#include "ProjectModel.hpp"

class NodeItem;
class EdgeItem;
class DocumentModel;

/**
 * DiagramScene - Manages the graphical representation of a PlantUML diagram.
 * 
 * Responsibilities:
 * - Creates NodeItem and EdgeItem from parsed PlantUML
 * - Applies layout positions from metadata
 * - Emits signals when layout changes (for persistence)
 * - Handles selection and interaction
 */
class DiagramScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit DiagramScene(QObject* parent = nullptr);
    ~DiagramScene() override = default;
    
    // Load diagram from PlantUML and metadata
    void loadDiagram(const QString& plantuml, const architect::PageMetadata& metadata);
    
    // Clear all items
    void clearDiagram();
    
    // Get current layout positions (for saving)
    QMap<QString, architect::LayoutPosition> getLayout() const;
    
    // Apply layout positions
    void applyLayout(const QMap<QString, architect::LayoutPosition>& layout);
    
    // Node access
    NodeItem* nodeById(const QString& nodeId) const;
    QVector<NodeItem*> allNodes() const;
    QVector<EdgeItem*> allEdges() const;
    
signals:
    void nodePositionChanged(const QString& nodeId, const QPointF& newPos);
    void nodeClicked(const QString& nodeId);
    void nodeDoubleClicked(const QString& nodeId);
    void selectionChanged();
    void layoutChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private slots:
    void onNodePositionChanged(const QString& nodeId, const QPointF& newPos);
    
private:
    void createNodesAndEdges(const architect::ParsedDiagram& diagram, 
                             const architect::PageMetadata& metadata);
    void autoLayoutNodes();
    
    architect::PlantUMLParser m_parser;
    QMap<QString, NodeItem*> m_nodeMap;
    QVector<EdgeItem*> m_edges;
    
    bool m_isDragging = false;
};

#endif // DIAGRAMSCENE_HPP

