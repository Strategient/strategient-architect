#ifndef RUNCONSOLE_HPP
#define RUNCONSOLE_HPP

#include <QWidget>

class QTextEdit;

class RunConsole : public QWidget {
    Q_OBJECT

public:
    explicit RunConsole(QWidget* parent = nullptr);
    ~RunConsole() override = default;

    void appendLog(const QString& message);
    void clear();

private:
    QTextEdit* m_textEdit{nullptr};
};

#endif // RUNCONSOLE_HPP

