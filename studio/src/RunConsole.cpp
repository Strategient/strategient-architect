#include "RunConsole.hpp"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QDateTime>
#include <QFont>

RunConsole::RunConsole(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_textEdit = new QTextEdit(this);
    m_textEdit->setObjectName("runConsole");
    m_textEdit->setReadOnly(true);
    m_textEdit->setMinimumHeight(120);
    
    // Use monospace font for console output
    QFont consoleFont("JetBrains Mono", 11);
    consoleFont.setStyleHint(QFont::Monospace);
    QStringList fallbacks;
    fallbacks << "Fira Code" << "Consolas" << "Monaco" << "monospace";
    consoleFont.setFamilies(QStringList() << "JetBrains Mono" << fallbacks);
    m_textEdit->setFont(consoleFont);

    layout->addWidget(m_textEdit);
}

void RunConsole::appendLog(const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_textEdit->append(QString("[%1] %2").arg(timestamp, message));
}

void RunConsole::clear() {
    m_textEdit->clear();
}

