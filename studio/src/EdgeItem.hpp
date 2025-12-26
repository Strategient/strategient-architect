#ifndef EDGEITEM_HPP
#define EDGEITEM_HPP

#include <QGraphicsPathItem>
#include <QColor>

class NodeItem;

/**
 * EdgeItem - A connecting line/arrow between two nodes.
 * 
 * Uses orthogonal (right-angle) routing for clean diagram appearance.
 * Automatically updates its path when connected nodes move.
 * Supports different arrow styles and optional labels.
 */
class EdgeItem : public QGraphicsPathItem {
public:
    enum { Type = UserType + 2 };
    
    // Connection side for orthogonal routing
    enum Side { Top, Bottom, Left, Right };
    
    explicit EdgeItem(NodeItem* fromNode, NodeItem* toNode, QGraphicsItem* parent = nullptr);
    ~EdgeItem() override = default;
    
    int type() const override { return Type; }
    
    // Connected nodes
    NodeItem* fromNode() const { return m_fromNode; }
    NodeItem* toNode() const { return m_toNode; }
    
    // Edge identity
    QString edgeId() const;
    
    // Appearance
    void setLabel(const QString& label);
    QString label() const { return m_label; }
    
    void setArrowType(const QString& arrowType);
    QString arrowType() const { return m_arrowType; }
    
    // Update the path (called when nodes move)
    void updatePath();
    
    // Painting
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
               QWidget* widget) override;

private:
    void calculatePath();
    
    // Orthogonal routing helpers
    Side bestConnectionSide(NodeItem* node, const QPointF& targetPos) const;
    QPointF connectionPointForSide(NodeItem* node, Side side) const;
    
    // Legacy method (still used internally)
    QPointF connectionPoint(NodeItem* node, const QPointF& otherCenter) const;
    
    void drawArrowHead(QPainter* painter, const QPointF& tip, const QPointF& from);
    
    NodeItem* m_fromNode;
    NodeItem* m_toNode;
    QString m_label;
    QString m_arrowType = "-->";
    
    QColor m_color = QColor("#6B7280");
    qreal m_lineWidth = 1.5;
};

#endif // EDGEITEM_HPP

