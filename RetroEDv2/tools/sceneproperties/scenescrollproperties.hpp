#pragma once

#include <QWidget>

#include "sceneincludesv5.hpp"

namespace Ui
{
class SceneScrollProperties;
}

class SceneScrollProperties : public QWidget
{
    Q_OBJECT

public:
    explicit SceneScrollProperties(QWidget *parent = nullptr);
    ~SceneScrollProperties();

    void setupUI(SceneHelpers::TileLayer::ScrollIndexInfo *info);
    void unsetUI();

signals:
    void manageInst(int cnt, bool removeInst = false);
    void editEntry(float value, byte option);
    void editDeform(bool enable);
    void editInst(int row, int value, byte option);
private:
    Ui::SceneScrollProperties *ui;
};


