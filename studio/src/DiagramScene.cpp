#include "DiagramScene.hpp"
#include "DotParser.hpp"
#include "NodeItem.hpp"
#include "EdgeItem.hpp"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>
#include <QRegularExpression>

DiagramScene::DiagramScene(QObject* parent)
    : QGraphicsScene(parent)
{
    // Set a default scene rect
    setSceneRect(-2000, -2000, 4000, 4000);
    
    // Use NoIndex to prevent BSP tree issues during drag (smoother updates)
    setItemIndexMethod(QGraphicsScene::NoIndex);
    
    // Set background color to match theme (slate blue)
    setBackgroundBrush(QColor("#2D3A47"));
}

void DiagramScene::loadDiagram(const QString& dotSource, 
                               const architect::PageMetadata& metadata) {
    clearDiagram();
    
    if (dotSource.isEmpty()) {
        return;
    }
    
    // Cache the DOT source for potential re-layout
    m_lastDotSource = dotSource;
    
    // Parse DOT source
    architect::ParsedDiagram diagram = m_parser.parse(dotSource);
    
    // Create graphics items
    createNodesAndEdges(diagram, metadata);
    
    // Apply saved layout or auto-layout using Graphviz
    if (!metadata.layout.isEmpty()) {
        applyLayout(metadata.layout);
    } else {
        // Use Graphviz for intelligent auto-layout
        autoLayoutWithGraphviz(dotSource);
    }
}

void DiagramScene::forceAutoLayout(const QString& dotSource) {
    if (dotSource.isEmpty() && m_lastDotSource.isEmpty()) {
        qWarning() << "[DiagramScene] No DOT source for auto-layout";
        return;
    }
    
    QString source = dotSource.isEmpty() ? m_lastDotSource : dotSource;
    m_lastDotSource = source;
    
    qDebug() << "[DiagramScene] Force auto-layout requested";
    autoLayoutWithGraphviz(source);
    
    // Emit layout changed to trigger save
    emit layoutChanged();
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
    // Create nodes for all parsed items
    for (const architect::ParsedNode& parsedNode : diagram.nodes) {
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
        
        // Set image path if specified in DOT
        if (!parsedNode.imagePath.isEmpty()) {
            nodeItem->setImagePath(parsedNode.imagePath);
        }
        
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

void DiagramScene::autoLayoutWithGraphviz(const QString& dotSource) {
    qDebug() << "[DiagramScene] Computing Graphviz layout for" << m_nodeMap.size() << "nodes";
    
    // Use LayoutOptimizer to analyze and select best engine
    QString optimizedDot = m_layoutOptimizer.optimizeDot(dotSource);
    architect::LayoutConfig config = m_layoutOptimizer.lastConfig();
    architect::GraphMetrics metrics = m_layoutOptimizer.lastMetrics();
    
    qDebug() << "[DiagramScene] LayoutOptimizer selected engine:" << config.engine
             << "for" << metrics.nodeCount << "nodes," << metrics.edgeCount << "edges,"
             << metrics.clusterCount << "clusters";
    
    // Compute positions using Graphviz
    QMap<QString, QPointF> positions = computeGraphvizLayout(optimizedDot);
    
    if (positions.isEmpty()) {
        qWarning() << "[DiagramScene] Graphviz layout failed, falling back to grid layout";
        autoLayoutNodes();
        return;
    }
    
    // Apply computed positions
    for (auto it = positions.constBegin(); it != positions.constEnd(); ++it) {
        NodeItem* node = m_nodeMap.value(it.key(), nullptr);
        if (node) {
            node->setPos(it.value());
        }
    }
    
    // Update all edges
    for (EdgeItem* edge : m_edges) {
        edge->updatePath();
    }
    
    qDebug() << "[DiagramScene] Applied Graphviz layout to" << positions.size() << "nodes";
}

QMap<QString, QPointF> DiagramScene::computeGraphvizLayout(const QString& dotSource) {
    QMap<QString, QPointF> positions;
    
    // Get the selected layout engine
    architect::LayoutConfig config = m_layoutOptimizer.lastConfig();
    QString engine = config.engine;
    
    // Write DOT to temp file
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(true);
    if (!tempFile.open()) {
        qWarning() << "[DiagramScene] Failed to create temp file for Graphviz";
        return positions;
    }
    
    tempFile.write(dotSource.toUtf8());
    tempFile.flush();
    
    // Run Graphviz with -Tplain to get node positions
    // Plain format outputs: node <name> <x> <y> <width> <height> <label> ...
    QProcess process;
    QStringList args = {"-Tplain", "-K" + engine, tempFile.fileName()};
    
    qDebug() << "[DiagramScene] Running: dot" << args.join(" ");
    
    process.start("dot", args);
    if (!process.waitForFinished(10000)) {
        qWarning() << "[DiagramScene] Graphviz timed out";
        return positions;
    }
    
    if (process.exitCode() != 0) {
        QString errorOutput = QString::fromUtf8(process.readAllStandardError());
        qWarning() << "[DiagramScene] Graphviz error:" << errorOutput;
        return positions;
    }
    
    // Parse plain format output
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    QStringList lines = output.split('\n');
    
    qDebug() << "[DiagramScene] Graphviz plain output:" << lines.size() << "lines";
    if (!lines.isEmpty()) {
        qDebug() << "[DiagramScene] First line:" << lines[0];
    }
    
    // Plain format: coordinates are in inches
    // Convert to pixels: 1 inch = 72 points, but we want more spread
    // Use 100 pixels per inch for nice spacing
    const double pixelsPerInch = 100.0;
    
    // Parse node positions
    // Format: node <name> <x> <y> <width> <height> <label> <style> <shape> <color> <fillcolor>
    static QRegularExpression nodeRe(
        R"(^node\s+(\S+)\s+([\d.-]+)\s+([\d.-]+)\s+([\d.-]+)\s+([\d.-]+))"
    );
    
    double minY = 0, maxY = 0;
    bool first = true;
    
    for (const QString& line : lines) {
        QRegularExpressionMatch match = nodeRe.match(line);
        if (match.hasMatch()) {
            QString nodeId = match.captured(1);
            
            // Remove quotes if present
            if (nodeId.startsWith('"') && nodeId.endsWith('"')) {
                nodeId = nodeId.mid(1, nodeId.length() - 2);
            }
            
            // Graphviz outputs coordinates in inches
            double xInches = match.captured(2).toDouble();
            double yInches = match.captured(3).toDouble();
            
            // Convert to pixels
            double x = xInches * pixelsPerInch;
            double y = yInches * pixelsPerInch;
            
            // Track Y range for flipping
            if (first) {
                minY = maxY = y;
                first = false;
            } else {
                minY = qMin(minY, y);
                maxY = qMax(maxY, y);
            }
            
            positions[nodeId] = QPointF(x, y);
            qDebug() << "[DiagramScene] Node" << nodeId << "raw:" << xInches << "," << yInches << "-> pixels:" << x << "," << y;
        }
    }
    
    // Flip Y axis (Graphviz Y goes up, Qt Y goes down)
    // Transform: y' = maxY - (y - minY) = maxY + minY - y
    double yOffset = maxY + minY;
    for (auto it = positions.begin(); it != positions.end(); ++it) {
        it.value().setY(yOffset - it.value().y());
    }
    
    qDebug() << "[DiagramScene] Parsed" << positions.size() << "node positions";
    
    return positions;
}


