#include "NodeItem.hpp"
#include "EdgeItem.hpp"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QFontMetrics>
#include <QCursor>
#include <QSvgRenderer>
#include <QFile>
#include <QDebug>

NodeItem::NodeItem(const QString& nodeId, const QString& label, 
                   const QString& nodeType, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_nodeId(nodeId)
    , m_label(label)
    , m_nodeType(nodeType)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setCursor(Qt::OpenHandCursor);
    
    // Disable caching to prevent ghosting during drag
    setCacheMode(QGraphicsItem::NoCache);
    
    updateGeometry();
}

void NodeItem::setLabel(const QString& label) {
    if (m_label != label) {
        m_label = label;
        updateGeometry();
        update();
    }
}

void NodeItem::setTestStatus(architect::TestStatus status) {
    if (m_testStatus != status) {
        m_testStatus = status;
        update();
    }
}

void NodeItem::setImagePath(const QString& imagePath) {
    if (m_imagePath != imagePath) {
        m_imagePath = imagePath;
        
        // Clean up old renderer
        delete m_svgRenderer;
        m_svgRenderer = nullptr;
        
        if (!imagePath.isEmpty()) {
            // Convert various path formats to Qt resource path
            // Input formats:
            //   "qrc:/shapes/software/api.svg" -> ":/shapes/software/api.svg"
            //   "shapes/software/api.svg"      -> ":/shapes/software/api.svg"
            //   ":/shapes/software/api.svg"    -> unchanged
            QString resourcePath = imagePath;
            if (resourcePath.startsWith("qrc:/")) {
                resourcePath = resourcePath.mid(3); // Remove "qrc" prefix, keep ":/..."
            } else if (resourcePath.startsWith(":/")) {
                // Already in correct format
            } else {
                // Relative path like "shapes/software/api.svg"
                resourcePath = ":/" + resourcePath;
            }
            
            qDebug() << "NodeItem: Loading image from:" << resourcePath << "(original:" << imagePath << ")";
            
            m_svgRenderer = new QSvgRenderer(resourcePath, this);
            
            if (!m_svgRenderer->isValid()) {
                qWarning() << "NodeItem: Failed to load SVG image:" << resourcePath;
                delete m_svgRenderer;
                m_svgRenderer = nullptr;
            } else {
                qDebug() << "NodeItem: Successfully loaded SVG:" << resourcePath;
            }
        }
        
        updateGeometry();
        update();
    }
}

void NodeItem::addEdge(EdgeItem* edge) {
    if (!m_edges.contains(edge)) {
        m_edges.append(edge);
    }
}

void NodeItem::removeEdge(EdgeItem* edge) {
    m_edges.removeAll(edge);
}

QRectF NodeItem::boundingRect() const {
    return QRectF(-m_width/2 - 2, -m_height/2 - 2, m_width + 4, m_height + 4);
}

QPainterPath NodeItem::shape() const {
    QPainterPath path;
    path.addRoundedRect(-m_width/2, -m_height/2, m_width, m_height, 
                        m_cornerRadius, m_cornerRadius);
    return path;
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
                     QWidget* widget) {
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    QRectF rect(-m_width/2, -m_height/2, m_width, m_height);
    
    // Background color based on type
    QColor bgColor = typeColor();
    QColor borderColor("#7088A8");
    
    // Selection/hover effects
    if (option->state & QStyle::State_Selected) {
        borderColor = QColor("#4A9EFF");
        bgColor = bgColor.lighter(110);
    } else if (m_hovered) {
        bgColor = bgColor.lighter(105);
    }
    
    // Draw shadow
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 30));
    painter->drawRoundedRect(rect.translated(2, 2), m_cornerRadius, m_cornerRadius);
    
    // Draw background
    painter->setBrush(bgColor);
    painter->setPen(QPen(borderColor, isSelected() ? 2 : 1));
    painter->drawRoundedRect(rect, m_cornerRadius, m_cornerRadius);
    
    // Draw image icon OR type icon (left side)
    QRectF iconRect(-m_width/2 + 8, -m_height/2 + 8, m_height - 16, m_height - 16);
    
    if (m_svgRenderer && m_svgRenderer->isValid()) {
        // Draw SVG image
        m_svgRenderer->render(painter, iconRect);
    } else {
        // Draw type icon (fallback)
        painter->setPen(QPen(QColor("#4A6080"), 1));
        painter->setBrush(Qt::NoBrush);
        
        // Center the icon rect vertically
        iconRect = QRectF(-m_width/2 + 8, -10, 20, 20);
        
        if (m_nodeType == "database") {
            // Database icon (cylinder)
            painter->drawEllipse(iconRect.left(), iconRect.top(), iconRect.width(), 8);
            painter->drawLine(iconRect.left(), iconRect.top() + 4, 
                              iconRect.left(), iconRect.bottom() - 4);
            painter->drawLine(iconRect.right(), iconRect.top() + 4, 
                              iconRect.right(), iconRect.bottom() - 4);
            painter->drawEllipse(iconRect.left(), iconRect.bottom() - 8, iconRect.width(), 8);
        } else if (m_nodeType == "cloud") {
            // Cloud icon
            painter->drawEllipse(iconRect.adjusted(2, 4, -2, -2));
        } else if (m_nodeType == "storage") {
            // Storage icon (folder)
            painter->drawRect(iconRect.adjusted(0, 4, 0, 0));
        } else {
            // Component icon (default box)
            painter->drawRect(iconRect.adjusted(2, 2, -2, -2));
            painter->drawLine(iconRect.left() + 2, iconRect.top() + 6,
                              iconRect.left() + 6, iconRect.top() + 6);
            painter->drawLine(iconRect.left() + 2, iconRect.bottom() - 6,
                              iconRect.left() + 6, iconRect.bottom() - 6);
        }
    }
    
    // Draw label text
    painter->setPen(QColor("#2A4060"));
    QFont font("Inter", 11);
    font.setWeight(QFont::Medium);
    painter->setFont(font);
    
    qreal textLeftMargin = m_svgRenderer ? (m_height - 8) : 32;
    QRectF textRect(-m_width/2 + textLeftMargin, -m_height/2, m_width - textLeftMargin - 28, m_height);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, m_label);
    
    // Draw test status badge (right side)
    QColor statusCol = statusColor();
    if (statusCol.isValid()) {
        QRectF badgeRect(m_width/2 - 24, -8, 16, 16);
        painter->setPen(Qt::NoPen);
        painter->setBrush(statusCol);
        painter->drawEllipse(badgeRect);
        
        // Draw status icon
        painter->setPen(QPen(Qt::white, 2));
        if (m_testStatus == architect::TestStatus::Pass) {
            // Checkmark
            painter->drawLine(badgeRect.center() + QPointF(-3, 0),
                              badgeRect.center() + QPointF(-1, 3));
            painter->drawLine(badgeRect.center() + QPointF(-1, 3),
                              badgeRect.center() + QPointF(4, -3));
        } else if (m_testStatus == architect::TestStatus::Fail) {
            // X mark
            painter->drawLine(badgeRect.center() + QPointF(-3, -3),
                              badgeRect.center() + QPointF(3, 3));
            painter->drawLine(badgeRect.center() + QPointF(3, -3),
                              badgeRect.center() + QPointF(-3, 3));
        }
    }
}

QPointF NodeItem::centerPoint() const {
    return scenePos();
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged) {
        // Update all connected edges
        for (EdgeItem* edge : m_edges) {
            edge->updatePath();
        }
        emit positionChanged(m_nodeId, value.toPointF());
    }
    return QGraphicsObject::itemChange(change, value);
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setCursor(Qt::ClosedHandCursor);
        emit clicked(m_nodeId);
    }
    QGraphicsObject::mousePressEvent(event);
}

void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(m_nodeId);
    }
    QGraphicsObject::mouseDoubleClickEvent(event);
}

void NodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    m_hovered = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    m_hovered = false;
    setCursor(Qt::OpenHandCursor);
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}

void NodeItem::updateGeometry() {
    // Calculate width based on label length
    QFont font("Inter", 11);
    font.setWeight(QFont::Medium);
    QFontMetrics fm(font);
    
    int textWidth = fm.horizontalAdvance(m_label);
    
    // Larger size when showing an image
    if (m_svgRenderer && m_svgRenderer->isValid()) {
        m_height = 56;  // Taller for image
        m_width = qMax(160.0, qreal(textWidth + m_height + 20));  // Extra space for image
    } else {
        m_height = 44;
        m_width = qMax(140.0, qreal(textWidth + 70));  // padding for icon and badge
    }
    
    prepareGeometryChange();
}

QColor NodeItem::statusColor() const {
    switch (m_testStatus) {
        case architect::TestStatus::Pass:
            return QColor("#4CAF50");  // Green
        case architect::TestStatus::Fail:
            return QColor("#F44336");  // Red
        case architect::TestStatus::NotRun:
        default:
            return QColor();  // No badge
    }
}

QString NodeItem::statusIcon() const {
    switch (m_testStatus) {
        case architect::TestStatus::Pass: return "✓";
        case architect::TestStatus::Fail: return "✗";
        default: return "";
    }
}

QColor NodeItem::typeColor() const {
    // Muted blue-grey colors for different types
    if (m_nodeType == "database") {
        return QColor("#D2D8DE");
    } else if (m_nodeType == "cloud") {
        return QColor("#D0DAE8");
    } else if (m_nodeType == "storage") {
        return QColor("#D4DAD8");
    } else if (m_nodeType == "queue") {
        return QColor("#D8D4DA");
    } else if (m_nodeType == "rectangle" || m_nodeType == "package") {
        return QColor("#D8DCE2");
    }
    // Default component color
    return QColor("#C8D4E4");
}

