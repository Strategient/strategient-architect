#include "EdgeItem.hpp"
#include "NodeItem.hpp"

#include <QPainter>
#include <QtMath>

EdgeItem::EdgeItem(NodeItem* fromNode, NodeItem* toNode, QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
    , m_fromNode(fromNode)
    , m_toNode(toNode)
{
    setZValue(-1);  // Draw behind nodes
    setPen(QPen(m_color, m_lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setFlag(QGraphicsItem::ItemIsSelectable);
    
    // Register with nodes
    if (m_fromNode) {
        m_fromNode->addEdge(this);
    }
    if (m_toNode) {
        m_toNode->addEdge(this);
    }
    
    updatePath();
}

QString EdgeItem::edgeId() const {
    QString fromId = m_fromNode ? m_fromNode->nodeId() : "null";
    QString toId = m_toNode ? m_toNode->nodeId() : "null";
    return QString("%1->%2").arg(fromId, toId);
}

void EdgeItem::setLabel(const QString& label) {
    if (m_label != label) {
        m_label = label;
        update();
    }
}

void EdgeItem::setArrowType(const QString& arrowType) {
    m_arrowType = arrowType;
    
    // Adjust line style based on arrow type
    Qt::PenStyle style = Qt::SolidLine;
    if (arrowType.contains("..")) {
        style = Qt::DotLine;
    } else if (arrowType.contains("--") && !arrowType.contains(">")) {
        style = Qt::DashLine;
    }
    
    QPen p = pen();
    p.setStyle(style);
    setPen(p);
    
    update();
}

void EdgeItem::updatePath() {
    calculatePath();
}

void EdgeItem::calculatePath() {
    if (!m_fromNode || !m_toNode) {
        setPath(QPainterPath());
        return;
    }
    
    QPointF fromCenter = m_fromNode->scenePos();
    QPointF toCenter = m_toNode->scenePos();
    
    // Get connection points on the edge of each node
    QPointF startPoint = connectionPoint(m_fromNode, toCenter);
    QPointF endPoint = connectionPoint(m_toNode, fromCenter);
    
    // Create the path
    QPainterPath path;
    path.moveTo(startPoint);
    
    // Use a bezier curve for smooth routing
    qreal dx = endPoint.x() - startPoint.x();
    qreal dy = endPoint.y() - startPoint.y();
    
    // Control points for the curve
    QPointF ctrl1, ctrl2;
    
    if (qAbs(dx) > qAbs(dy)) {
        // More horizontal - curve horizontally
        ctrl1 = startPoint + QPointF(dx * 0.4, 0);
        ctrl2 = endPoint - QPointF(dx * 0.4, 0);
    } else {
        // More vertical - curve vertically
        ctrl1 = startPoint + QPointF(0, dy * 0.4);
        ctrl2 = endPoint - QPointF(0, dy * 0.4);
    }
    
    path.cubicTo(ctrl1, ctrl2, endPoint);
    
    setPath(path);
}

QPointF EdgeItem::connectionPoint(NodeItem* node, const QPointF& otherCenter) const {
    QPointF nodeCenter = node->scenePos();
    QRectF rect = node->boundingRect();
    
    // Adjust rect to scene coordinates
    qreal halfW = rect.width() / 2 - 2;
    qreal halfH = rect.height() / 2 - 2;
    
    // Calculate angle to other center
    qreal dx = otherCenter.x() - nodeCenter.x();
    qreal dy = otherCenter.y() - nodeCenter.y();
    qreal angle = qAtan2(dy, dx);
    
    // Find intersection with node boundary
    qreal tanAngle = qTan(angle);
    qreal x, y;
    
    if (qAbs(tanAngle) < halfH / halfW) {
        // Intersects left or right edge
        if (dx >= 0) {
            x = halfW;
        } else {
            x = -halfW;
        }
        y = x * tanAngle;
    } else {
        // Intersects top or bottom edge
        if (dy >= 0) {
            y = halfH;
        } else {
            y = -halfH;
        }
        x = y / tanAngle;
    }
    
    return nodeCenter + QPointF(x, y);
}

void EdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
                     QWidget* widget) {
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    // Draw the path
    QPen linePen = pen();
    if (isSelected()) {
        linePen.setColor(QColor("#4A9EFF"));
        linePen.setWidth(m_lineWidth + 1);
    }
    painter->setPen(linePen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path());
    
    // Draw arrow head if needed
    if (m_arrowType.contains(">") && !path().isEmpty()) {
        QPainterPath p = path();
        qreal pathLen = p.length();
        if (pathLen > 0) {
            QPointF tip = p.pointAtPercent(1.0);
            QPointF beforeTip = p.pointAtPercent(qMax(0.0, 1.0 - 15.0/pathLen));
            drawArrowHead(painter, tip, beforeTip);
        }
    }
    
    // Draw label if present
    if (!m_label.isEmpty()) {
        QPainterPath p = path();
        if (p.length() > 0) {
            QPointF midPoint = p.pointAtPercent(0.5);
            
            // Draw label background
            QFont font("Inter", 9);
            painter->setFont(font);
            QFontMetrics fm(font);
            QRectF textRect = fm.boundingRect(m_label);
            textRect.moveCenter(midPoint);
            textRect.adjust(-4, -2, 4, 2);
            
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor("#E8EBEF"));
            painter->drawRoundedRect(textRect, 3, 3);
            
            // Draw label text
            painter->setPen(QColor("#4A5568"));
            painter->drawText(textRect, Qt::AlignCenter, m_label);
        }
    }
}

void EdgeItem::drawArrowHead(QPainter* painter, const QPointF& tip, const QPointF& from) {
    qreal arrowSize = 10;
    
    // Calculate angle
    qreal dx = tip.x() - from.x();
    qreal dy = tip.y() - from.y();
    qreal angle = qAtan2(dy, dx);
    
    // Arrow head points
    QPointF p1 = tip - QPointF(qCos(angle - M_PI/6) * arrowSize,
                                qSin(angle - M_PI/6) * arrowSize);
    QPointF p2 = tip - QPointF(qCos(angle + M_PI/6) * arrowSize,
                                qSin(angle + M_PI/6) * arrowSize);
    
    // Draw filled arrow head
    QPolygonF arrowHead;
    arrowHead << tip << p1 << p2;
    
    painter->setPen(Qt::NoPen);
    painter->setBrush(isSelected() ? QColor("#4A9EFF") : m_color);
    painter->drawPolygon(arrowHead);
}

