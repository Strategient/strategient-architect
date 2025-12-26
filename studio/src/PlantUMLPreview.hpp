#ifndef PLANTUMLPREVIEW_HPP
#define PLANTUMLPREVIEW_HPP

#include <QWidget>

class QSvgWidget;
class QLabel;
class QStackedWidget;

class PlantUMLPreview : public QWidget {
    Q_OBJECT

public:
    explicit PlantUMLPreview(QWidget* parent = nullptr);
    ~PlantUMLPreview() override = default;

    void loadSvg(const QString& filePath);
    void clear();

private:
    QStackedWidget* m_stack{nullptr};
    QSvgWidget* m_svgWidget{nullptr};
    QLabel* m_placeholder{nullptr};
    QString m_currentPath;
};

#endif // PLANTUMLPREVIEW_HPP

