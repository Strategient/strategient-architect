#include "DiagramScene.hpp"
#include "NodeItem.hpp"
#include "EdgeItem.hpp"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>

DiagramScene::DiagramScene(QObject* parent)
    : QGraphicsScene(parent)
{
    // Set a default scene rect
    setSceneRect(-2000, -2000, 4000, 4000);
    
    // Set background color to match theme
    setBackgroundBrush(QColor("#1a1d21"));
}

void DiagramScene::loadDiagram(const QString& plantuml, 
                               const architect::PageMetadata& metadata) {
    clearDiagram();
    
    if (plantuml.isEmpty()) {
        return;
    }
    
    // Parse PlantUML
    architect::ParsedDiagram diagram = m_parser.parse(plantuml);
    
    // Create graphics items
    createNodesAndEdges(diagram, metadata);
    
    // Apply saved layout or auto-layout
    if (!metadata.layout.isEmpty()) {
        applyLayout(metadata.layout);
    } else {
        autoLayoutNodes();
    }
}

void DiagramScene::clearDiagram() {
    // Clear edges first (they reference nodes)
    for (EdgeItem* edge : m_edges) {
        if (edge->fromNode()) {
            edge->fromNode()->removeEdge(edge);
        }
        if (edge->toNode()) {
            edge->toNode()->removeEdge(edge);
        }
    }
    m_edges.clear();
    m_nodeMap.clear();
    
    // Clear all items from scene
    clear();
}

void DiagramScene::createNodesAndEdges(const architect::ParsedDiagram& diagram,
                                       const architect::PageMetadata& metadata) {
    // Create nodes
    for (const architect::ParsedNode& parsedNode : diagram.nodes) {
        // Skip container nodes for now (just use their children)
        if (parsedNode.type == "rectangle" || parsedNode.type == "package" ||
            parsedNode.type == "cloud" || parsedNode.type == "node" ||
            parsedNode.type == "frame" || parsedNode.type == "folder") {
            continue;
        }
        
        // Get display name from metadata or use parsed label
        QString displayName = parsedNode.label;
        architect::TestStatus testStatus = architect::TestStatus::NotRun;
        
        if (metadata.nodes.contains(parsedNode.id)) {
            const architect::NodeMetadata& nodeMeta = metadata.nodes[parsedNode.id];
            if (!nodeMeta.displayName.isEmpty()) {
                displayName = nodeMeta.displayName;
            }
            testStatus = nodeMeta.testStatus;
        }
        
        // Create node item
        NodeItem* nodeItem = new NodeItem(parsedNode.id, displayName, parsedNode.type);
        nodeItem->setTestStatus(testStatus);
        
        // Connect signals
        connect(nodeItem, &NodeItem::positionChanged,
                this, &DiagramScene::onNodePositionChanged);
        connect(nodeItem, &NodeItem::clicked,
                this, &DiagramScene::nodeClicked);
        connect(nodeItem, &NodeItem::doubleClicked,
                this, &DiagramScene::nodeDoubleClicked);
        
        addItem(nodeItem);
        m_nodeMap[parsedNode.id] = nodeItem;
    }
    
    // Create edges
    for (const architect::ParsedEdge& parsedEdge : diagram.edges) {
        NodeItem* fromNode = m_nodeMap.value(parsedEdge.fromId, nullptr);
        NodeItem* toNode = m_nodeMap.value(parsedEdge.toId, nullptr);
        
        if (fromNode && toNode) {
            EdgeItem* edgeItem = new EdgeItem(fromNode, toNode);
            edgeItem->setArrowType(parsedEdge.arrowType);
            
            // Get label from parsed edge or metadata
            QString edgeId = QString("%1->%2").arg(parsedEdge.fromId, parsedEdge.toId);
            if (metadata.edges.contains(edgeId)) {
                edgeItem->setLabel(metadata.edges[edgeId].label);
            } else if (!parsedEdge.label.isEmpty()) {
                edgeItem->setLabel(parsedEdge.label);
            }
            
            addItem(edgeItem);
            m_edges.append(edgeItem);
        }
    }
}

void DiagramScene::autoLayoutNodes() {
    // Simple grid layout for nodes without saved positions
    int col = 0;
    int row = 0;
    const int maxCols = 4;
    const qreal xSpacing = 200;
    const qreal ySpacing = 100;
    const qreal startX = -300;
    const qreal startY = -200;
    
    for (auto it = m_nodeMap.begin(); it != m_nodeMap.end(); ++it) {
        NodeItem* node = it.value();
        qreal x = startX + col * xSpacing;
        qreal y = startY + row * ySpacing;
        node->setPos(x, y);
        
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
    
    // Update all edges
    for (EdgeItem* edge : m_edges) {
        edge->updatePath();
    }
}

QMap<QString, architect::LayoutPosition> DiagramScene::getLayout() const {
    QMap<QString, architect::LayoutPosition> layout;
    
    for (auto it = m_nodeMap.constBegin(); it != m_nodeMap.constEnd(); ++it) {
        architect::LayoutPosition pos;
        pos.x = it.value()->scenePos().x();
        pos.y = it.value()->scenePos().y();
        layout[it.key()] = pos;
    }
    
    return layout;
}

void DiagramScene::applyLayout(const QMap<QString, architect::LayoutPosition>& layout) {
    for (auto it = layout.constBegin(); it != layout.constEnd(); ++it) {
        NodeItem* node = m_nodeMap.value(it.key(), nullptr);
        if (node) {
            node->setPos(it.value().x, it.value().y);
        }
    }
    
    // Update all edges
    for (EdgeItem* edge : m_edges) {
        edge->updatePath();
    }
}

NodeItem* DiagramScene::nodeById(const QString& nodeId) const {
    return m_nodeMap.value(nodeId, nullptr);
}

QVector<NodeItem*> DiagramScene::allNodes() const {
    QVector<NodeItem*> nodes;
    for (NodeItem* node : m_nodeMap) {
        nodes.append(node);
    }
    return nodes;
}

QVector<EdgeItem*> DiagramScene::allEdges() const {
    return m_edges;
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
    }
    QGraphicsScene::mousePressEvent(event);
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_isDragging) {
        m_isDragging = false;
        // Emit layout changed after drag ends
        emit layoutChanged();
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void DiagramScene::onNodePositionChanged(const QString& nodeId, const QPointF& newPos) {
    emit nodePositionChanged(nodeId, newPos);
}

