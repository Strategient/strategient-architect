#ifndef PROJECTMODEL_HPP
#define PROJECTMODEL_HPP

#include <QString>
#include <QVector>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace architect {

// Page types supported in v1
enum class PageType {
    PipelineDiagram,
    InfraConfig,
    Scheduler,
    BacktestSweeps,
    DiagramOnly
};

QString pageTypeToString(PageType type);
PageType pageTypeFromString(const QString& str);

// Test status for nodes
enum class TestStatus {
    NotRun,
    Pass,
    Fail
};

QString testStatusToString(TestStatus status);
TestStatus testStatusFromString(const QString& str);

// Task associated with a node
struct Task {
    QString id;
    QString title;
    QString status;  // "pending", "in_progress", "completed"

    QJsonObject toJson() const;
    static Task fromJson(const QJsonObject& json);
};

// Layout position for a node
struct LayoutPosition {
    double x = 0.0;
    double y = 0.0;

    QJsonObject toJson() const;
    static LayoutPosition fromJson(const QJsonObject& json);
};

// Metadata for a single node (keyed by node ID/alias from DOT)
struct NodeMetadata {
    QString displayName;           // Optional override
    QString agentPromptText;       // Prompt for agent
    QVector<QString> ownershipPaths;  // Paths for agent guardrails
    QVector<QString> codeRefs;     // File paths or commands
    TestStatus testStatus = TestStatus::NotRun;
    QVector<Task> tasks;

    QJsonObject toJson() const;
    static NodeMetadata fromJson(const QJsonObject& json);
};

// Metadata for an edge (keyed by edge_id or "from->to")
struct EdgeMetadata {
    QString label;
    QString description;

    QJsonObject toJson() const;
    static EdgeMetadata fromJson(const QJsonObject& json);
};

// All metadata for a page
struct PageMetadata {
    QMap<QString, NodeMetadata> nodes;    // keyed by node_id/alias
    QMap<QString, EdgeMetadata> edges;    // keyed by edge_id or "from->to"
    QMap<QString, LayoutPosition> layout; // keyed by node_id

    QJsonObject toJson() const;
    static PageMetadata fromJson(const QJsonObject& json);
};

// A single page in the project
struct Page {
    QString pageId;
    QString title;
    PageType pageType = PageType::DiagramOnly;
    QString graphviz;          // Canonical Graphviz DOT source
    PageMetadata metadata;

    QJsonObject toJson() const;
    static Page fromJson(const QJsonObject& json);
};

// The top-level project
struct Project {
    static constexpr int CURRENT_SCHEMA_VERSION = 1;

    int schemaVersion = CURRENT_SCHEMA_VERSION;
    QString projectId;
    QString title;
    QVector<Page> pages;

    QJsonObject toJson() const;
    static Project fromJson(const QJsonObject& json);

    // Convenience
    QJsonDocument toJsonDocument() const;
    static Project fromJsonDocument(const QJsonDocument& doc);
};

} // namespace architect

#endif // PROJECTMODEL_HPP

