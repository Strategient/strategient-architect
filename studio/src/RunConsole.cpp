#include "RunConsole.hpp"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QDateTime>

RunConsole::RunConsole(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setMinimumHeight(120);
    m_textEdit->setStyleSheet(
        "QTextEdit { "
        "  background-color: #1e1e1e; "
        "  color: #d4d4d4; "
        "  font-family: 'JetBrains Mono', 'Fira Code', 'Consolas', monospace; "
        "  font-size: 12px; "
        "  border: none; "
        "  padding: 8px; "
        "}");

    layout->addWidget(m_textEdit);
}

void RunConsole::appendLog(const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_textEdit->append(QString("[%1] %2").arg(timestamp, message));
}

void RunConsole::clear() {
    m_textEdit->clear();
}

