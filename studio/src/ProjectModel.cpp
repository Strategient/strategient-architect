#include "ProjectModel.hpp"

namespace architect {

// --- PageType ---
QString pageTypeToString(PageType type) {
    switch (type) {
        case PageType::PipelineDiagram: return "PipelineDiagram";
        case PageType::InfraConfig:     return "InfraConfig";
        case PageType::Scheduler:       return "Scheduler";
        case PageType::BacktestSweeps:  return "BacktestSweeps";
        case PageType::DiagramOnly:     return "DiagramOnly";
    }
    return "DiagramOnly";
}

PageType pageTypeFromString(const QString& str) {
    if (str == "PipelineDiagram") return PageType::PipelineDiagram;
    if (str == "InfraConfig")     return PageType::InfraConfig;
    if (str == "Scheduler")       return PageType::Scheduler;
    if (str == "BacktestSweeps")  return PageType::BacktestSweeps;
    return PageType::DiagramOnly;
}

// --- TestStatus ---
QString testStatusToString(TestStatus status) {
    switch (status) {
        case TestStatus::NotRun: return "not_run";
        case TestStatus::Pass:   return "pass";
        case TestStatus::Fail:   return "fail";
    }
    return "not_run";
}

TestStatus testStatusFromString(const QString& str) {
    if (str == "pass") return TestStatus::Pass;
    if (str == "fail") return TestStatus::Fail;
    return TestStatus::NotRun;
}

// --- Task ---
QJsonObject Task::toJson() const {
    return QJsonObject{
        {"id", id},
        {"title", title},
        {"status", status}
    };
}

Task Task::fromJson(const QJsonObject& json) {
    Task t;
    t.id = json["id"].toString();
    t.title = json["title"].toString();
    t.status = json["status"].toString("pending");
    return t;
}

// --- LayoutPosition ---
QJsonObject LayoutPosition::toJson() const {
    return QJsonObject{
        {"x", x},
        {"y", y}
    };
}

LayoutPosition LayoutPosition::fromJson(const QJsonObject& json) {
    LayoutPosition lp;
    lp.x = json["x"].toDouble(0.0);
    lp.y = json["y"].toDouble(0.0);
    return lp;
}

// --- NodeMetadata ---
QJsonObject NodeMetadata::toJson() const {
    QJsonObject obj;
    
    if (!displayName.isEmpty()) {
        obj["display_name"] = displayName;
    }
    if (!agentPromptText.isEmpty()) {
        obj["agent_prompt_text"] = agentPromptText;
    }
    if (!ownershipPaths.isEmpty()) {
        QJsonArray arr;
        for (const auto& p : ownershipPaths) arr.append(p);
        obj["ownership_paths"] = arr;
    }
    if (!codeRefs.isEmpty()) {
        QJsonArray arr;
        for (const auto& r : codeRefs) arr.append(r);
        obj["code_refs"] = arr;
    }
    obj["test_status"] = testStatusToString(testStatus);
    if (!tasks.isEmpty()) {
        QJsonArray arr;
        for (const auto& t : tasks) arr.append(t.toJson());
        obj["tasks"] = arr;
    }
    
    return obj;
}

NodeMetadata NodeMetadata::fromJson(const QJsonObject& json) {
    NodeMetadata nm;
    nm.displayName = json["display_name"].toString();
    nm.agentPromptText = json["agent_prompt_text"].toString();
    nm.testStatus = testStatusFromString(json["test_status"].toString());
    
    for (const auto& v : json["ownership_paths"].toArray()) {
        nm.ownershipPaths.append(v.toString());
    }
    for (const auto& v : json["code_refs"].toArray()) {
        nm.codeRefs.append(v.toString());
    }
    for (const auto& v : json["tasks"].toArray()) {
        nm.tasks.append(Task::fromJson(v.toObject()));
    }
    
    return nm;
}

// --- EdgeMetadata ---
QJsonObject EdgeMetadata::toJson() const {
    QJsonObject obj;
    if (!label.isEmpty()) obj["label"] = label;
    if (!description.isEmpty()) obj["description"] = description;
    return obj;
}

EdgeMetadata EdgeMetadata::fromJson(const QJsonObject& json) {
    EdgeMetadata em;
    em.label = json["label"].toString();
    em.description = json["description"].toString();
    return em;
}

// --- PageMetadata ---
QJsonObject PageMetadata::toJson() const {
    QJsonObject obj;
    
    if (!nodes.isEmpty()) {
        QJsonObject nodesObj;
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            nodesObj[it.key()] = it.value().toJson();
        }
        obj["nodes"] = nodesObj;
    }
    
    if (!edges.isEmpty()) {
        QJsonObject edgesObj;
        for (auto it = edges.begin(); it != edges.end(); ++it) {
            edgesObj[it.key()] = it.value().toJson();
        }
        obj["edges"] = edgesObj;
    }
    
    if (!layout.isEmpty()) {
        QJsonObject layoutObj;
        for (auto it = layout.begin(); it != layout.end(); ++it) {
            layoutObj[it.key()] = it.value().toJson();
        }
        obj["layout"] = layoutObj;
    }
    
    return obj;
}

PageMetadata PageMetadata::fromJson(const QJsonObject& json) {
    PageMetadata pm;
    
    QJsonObject nodesObj = json["nodes"].toObject();
    for (auto it = nodesObj.begin(); it != nodesObj.end(); ++it) {
        pm.nodes[it.key()] = NodeMetadata::fromJson(it.value().toObject());
    }
    
    QJsonObject edgesObj = json["edges"].toObject();
    for (auto it = edgesObj.begin(); it != edgesObj.end(); ++it) {
        pm.edges[it.key()] = EdgeMetadata::fromJson(it.value().toObject());
    }
    
    QJsonObject layoutObj = json["layout"].toObject();
    for (auto it = layoutObj.begin(); it != layoutObj.end(); ++it) {
        pm.layout[it.key()] = LayoutPosition::fromJson(it.value().toObject());
    }
    
    return pm;
}

// --- Page ---
QJsonObject Page::toJson() const {
    return QJsonObject{
        {"page_id", pageId},
        {"title", title},
        {"page_type", pageTypeToString(pageType)},
        {"plantuml", plantuml},
        {"metadata", metadata.toJson()}
    };
}

Page Page::fromJson(const QJsonObject& json) {
    Page p;
    p.pageId = json["page_id"].toString();
    p.title = json["title"].toString();
    p.pageType = pageTypeFromString(json["page_type"].toString());
    p.plantuml = json["plantuml"].toString();
    p.metadata = PageMetadata::fromJson(json["metadata"].toObject());
    return p;
}

// --- Project ---
QJsonObject Project::toJson() const {
    QJsonArray pagesArray;
    for (const auto& page : pages) {
        pagesArray.append(page.toJson());
    }
    
    return QJsonObject{
        {"schema_version", schemaVersion},
        {"project_id", projectId},
        {"title", title},
        {"pages", pagesArray}
    };
}

Project Project::fromJson(const QJsonObject& json) {
    Project proj;
    proj.schemaVersion = json["schema_version"].toInt(1);
    proj.projectId = json["project_id"].toString();
    proj.title = json["title"].toString();
    
    for (const auto& v : json["pages"].toArray()) {
        proj.pages.append(Page::fromJson(v.toObject()));
    }
    
    return proj;
}

QJsonDocument Project::toJsonDocument() const {
    return QJsonDocument(toJson());
}

Project Project::fromJsonDocument(const QJsonDocument& doc) {
    return fromJson(doc.object());
}

} // namespace architect

