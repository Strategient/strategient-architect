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
    
    // Disable caching to prevent ghosting during node drag
    setCacheMode(QGraphicsItem::NoCache);
    
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
    prepareGeometryChange();
    calculatePath();
    update();
}

// Determine which side of the node to connect from
EdgeItem::Side EdgeItem::bestConnectionSide(NodeItem* node, const QPointF& targetPos) const {
    QPointF nodeCenter = node->scenePos();
    QRectF rect = node->boundingRect();
    qreal halfW = rect.width() / 2;
    qreal halfH = rect.height() / 2;
    
    qreal dx = targetPos.x() - nodeCenter.x();
    qreal dy = targetPos.y() - nodeCenter.y();
    
    // Determine primary direction
    if (qAbs(dx) > qAbs(dy)) {
        // More horizontal
        return (dx > 0) ? Right : Left;
    } else {
        // More vertical
        return (dy > 0) ? Bottom : Top;
    }
}

QPointF EdgeItem::connectionPointForSide(NodeItem* node, Side side) const {
    QPointF center = node->scenePos();
    QRectF rect = node->boundingRect();
    qreal halfW = rect.width() / 2 - 2;
    qreal halfH = rect.height() / 2 - 2;
    
    switch (side) {
        case Top:    return center + QPointF(0, -halfH);
        case Bottom: return center + QPointF(0, halfH);
        case Left:   return center + QPointF(-halfW, 0);
        case Right:  return center + QPointF(halfW, 0);
    }
    return center;
}

void EdgeItem::calculatePath() {
    if (!m_fromNode || !m_toNode) {
        setPath(QPainterPath());
        return;
    }
    
    QPointF fromCenter = m_fromNode->scenePos();
    QPointF toCenter = m_toNode->scenePos();
    
    // Determine best connection sides
    Side fromSide = bestConnectionSide(m_fromNode, toCenter);
    Side toSide = bestConnectionSide(m_toNode, fromCenter);
    
    // Get connection points
    QPointF startPoint = connectionPointForSide(m_fromNode, fromSide);
    QPointF endPoint = connectionPointForSide(m_toNode, toSide);
    
    // Build orthogonal path
    QPainterPath path;
    path.moveTo(startPoint);
    
    // Calculate intermediate points for orthogonal routing
    qreal dx = endPoint.x() - startPoint.x();
    qreal dy = endPoint.y() - startPoint.y();
    
    // Spacing from nodes for the routing
    const qreal spacing = 25.0;
    
    // Route based on the connection sides
    if ((fromSide == Right && toSide == Left) || (fromSide == Left && toSide == Right)) {
        // Horizontal connection - go straight or with one bend
        qreal midX = (startPoint.x() + endPoint.x()) / 2;
        
        // Check if we need to route around (nodes overlapping horizontally)
        if ((fromSide == Right && endPoint.x() < startPoint.x() + spacing) ||
            (fromSide == Left && endPoint.x() > startPoint.x() - spacing)) {
            // Need to route around
            qreal routeY = (startPoint.y() < endPoint.y()) 
                ? qMin(startPoint.y(), endPoint.y()) - spacing * 2
                : qMax(startPoint.y(), endPoint.y()) + spacing * 2;
            
            path.lineTo(startPoint.x() + (fromSide == Right ? spacing : -spacing), startPoint.y());
            path.lineTo(startPoint.x() + (fromSide == Right ? spacing : -spacing), routeY);
            path.lineTo(endPoint.x() + (toSide == Left ? -spacing : spacing), routeY);
            path.lineTo(endPoint.x() + (toSide == Left ? -spacing : spacing), endPoint.y());
        } else {
            // Simple L or Z bend
            path.lineTo(midX, startPoint.y());
            path.lineTo(midX, endPoint.y());
        }
    }
    else if ((fromSide == Top && toSide == Bottom) || (fromSide == Bottom && toSide == Top)) {
        // Vertical connection
        qreal midY = (startPoint.y() + endPoint.y()) / 2;
        
        // Check if we need to route around
        if ((fromSide == Bottom && endPoint.y() < startPoint.y() + spacing) ||
            (fromSide == Top && endPoint.y() > startPoint.y() - spacing)) {
            // Need to route around
            qreal routeX = (startPoint.x() < endPoint.x())
                ? qMin(startPoint.x(), endPoint.x()) - spacing * 2
                : qMax(startPoint.x(), endPoint.x()) + spacing * 2;
            
            path.lineTo(startPoint.x(), startPoint.y() + (fromSide == Bottom ? spacing : -spacing));
            path.lineTo(routeX, startPoint.y() + (fromSide == Bottom ? spacing : -spacing));
            path.lineTo(routeX, endPoint.y() + (toSide == Top ? -spacing : spacing));
            path.lineTo(endPoint.x(), endPoint.y() + (toSide == Top ? -spacing : spacing));
        } else {
            // Simple L or Z bend
            path.lineTo(startPoint.x(), midY);
            path.lineTo(endPoint.x(), midY);
        }
    }
    else if ((fromSide == Right || fromSide == Left) && (toSide == Top || toSide == Bottom)) {
        // Horizontal to Vertical - single bend
        path.lineTo(endPoint.x(), startPoint.y());
    }
    else if ((fromSide == Top || fromSide == Bottom) && (toSide == Right || toSide == Left)) {
        // Vertical to Horizontal - single bend
        path.lineTo(startPoint.x(), endPoint.y());
    }
    else {
        // Same side connections (e.g., both Right) - need to route around
        qreal offset = spacing * 2;
        
        if (fromSide == Right || fromSide == Left) {
            qreal routeX = (fromSide == Right) 
                ? qMax(startPoint.x(), endPoint.x()) + offset
                : qMin(startPoint.x(), endPoint.x()) - offset;
            path.lineTo(routeX, startPoint.y());
            path.lineTo(routeX, endPoint.y());
        } else {
            qreal routeY = (fromSide == Bottom)
                ? qMax(startPoint.y(), endPoint.y()) + offset
                : qMin(startPoint.y(), endPoint.y()) - offset;
            path.lineTo(startPoint.x(), routeY);
            path.lineTo(endPoint.x(), routeY);
        }
    }
    
    path.lineTo(endPoint);
    setPath(path);
}

QPointF EdgeItem::connectionPoint(NodeItem* node, const QPointF& otherCenter) const {
    // Legacy method - kept for compatibility
    Side side = bestConnectionSide(node, otherCenter);
    return connectionPointForSide(node, side);
}

void EdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
                     QWidget* widget) {
    Q_UNUSED(widget);
    Q_UNUSED(option);
    
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
        int elementCount = p.elementCount();
        if (elementCount >= 2) {
            // Get the last segment for arrow direction
            QPointF tip = p.elementAt(elementCount - 1);
            QPointF beforeTip = p.elementAt(elementCount - 2);
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
            painter->setBrush(QColor("#384858"));  // Match theme
            painter->drawRoundedRect(textRect, 3, 3);
            
            // Draw label text
            painter->setPen(QColor("#AEB9C7"));  // Match theme
            painter->drawText(textRect, Qt::AlignCenter, m_label);
        }
    }
}

void EdgeItem::drawArrowHead(QPainter* painter, const QPointF& tip, const QPointF& from) {
    qreal arrowSize = 8;
    
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

