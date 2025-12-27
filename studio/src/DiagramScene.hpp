#ifndef DIAGRAMSCENE_HPP
#define DIAGRAMSCENE_HPP

#include <QGraphicsScene>
#include <QMap>
#include "DotParser.hpp"
#include "LayoutOptimizer.hpp"
#include "ProjectModel.hpp"

class NodeItem;
class EdgeItem;
class DocumentModel;

/**
 * DiagramScene - Manages the graphical representation of a Graphviz DOT diagram.
 * 
 * Responsibilities:
 * - Creates NodeItem and EdgeItem from parsed DOT source
 * - Applies layout positions from metadata
 * - Emits signals when layout changes (for persistence)
 * - Handles selection and interaction
 */
class DiagramScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit DiagramScene(QObject* parent = nullptr);
    ~DiagramScene() override = default;
    
    // Load diagram from DOT source and metadata
    void loadDiagram(const QString& dotSource, const architect::PageMetadata& metadata);
    
    // Force auto-layout using Graphviz (discards saved positions)
    void forceAutoLayout(const QString& dotSource);
    
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
    void autoLayoutWithGraphviz(const QString& dotSource);
    QMap<QString, QPointF> computeGraphvizLayout(const QString& dotSource);
    
    architect::DotParser m_parser;
    architect::LayoutOptimizer m_layoutOptimizer;
    QMap<QString, NodeItem*> m_nodeMap;
    QVector<EdgeItem*> m_edges;
    QString m_lastDotSource;  // Cache for re-layout
    
    bool m_isDragging = false;
};

#endif // DIAGRAMSCENE_HPP

