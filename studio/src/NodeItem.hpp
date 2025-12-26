#ifndef NODEITEM_HPP
#define NODEITEM_HPP

#include <QGraphicsObject>
#include <QColor>
#include "ProjectModel.hpp"

class EdgeItem;

/**
 * NodeItem - A draggable node in the diagram canvas.
 * 
 * Displays:
 * - Node label (from metadata.displayName or DOT node ID)
 * - Test status badge (Pass/Fail/NotRun)
 * - Type icon indicator
 * 
 * Emits positionChanged signal when dragged.
 */
class NodeItem : public QGraphicsObject {
    Q_OBJECT

public:
    enum { Type = UserType + 1 };
    
    explicit NodeItem(const QString& nodeId, 
                      const QString& label,
                      const QString& nodeType,
                      QGraphicsItem* parent = nullptr);
    ~NodeItem() override = default;
    
    int type() const override { return Type; }
    
    // Node identity
    QString nodeId() const { return m_nodeId; }
    QString label() const { return m_label; }
    QString nodeType() const { return m_nodeType; }
    
    // Appearance
    void setLabel(const QString& label);
    void setTestStatus(architect::TestStatus status);
    architect::TestStatus testStatus() const { return m_testStatus; }
    
    // Edge management
    void addEdge(EdgeItem* edge);
    void removeEdge(EdgeItem* edge);
    QVector<EdgeItem*> edges() const { return m_edges; }
    
    // Geometry
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
               QWidget* widget) override;
    
    // Connection point (center of node)
    QPointF centerPoint() const;
    
signals:
    void positionChanged(const QString& nodeId, const QPointF& newPos);
    void clicked(const QString& nodeId);
    void doubleClicked(const QString& nodeId);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    void updateGeometry();
    QColor statusColor() const;
    QString statusIcon() const;
    QColor typeColor() const;
    
    QString m_nodeId;
    QString m_label;
    QString m_nodeType;
    architect::TestStatus m_testStatus = architect::TestStatus::NotRun;
    
    QVector<EdgeItem*> m_edges;
    
    // Cached geometry
    qreal m_width = 160;
    qreal m_height = 50;
    qreal m_cornerRadius = 8;
    bool m_hovered = false;
};

#endif // NODEITEM_HPP

