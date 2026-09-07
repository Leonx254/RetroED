#pragma once

#include <QWidget>

class SceneViewer;

namespace Ui
{
class SceneLayerProperties;
}

class SceneLayerProperties : public QWidget
{
    Q_OBJECT

public:
    explicit SceneLayerProperties(QWidget *parent = nullptr);
    ~SceneLayerProperties();

    void setupUI(SceneViewer *viewer, byte layerID);
    void unsetUI();
signals:
    void updateLayerSize(int newSize, bool isHeight = false);
    void updateParallax(double v);
    void updateScroll(double v);
    void updateType(int v);
private:
    Ui::SceneLayerProperties *ui;
};


