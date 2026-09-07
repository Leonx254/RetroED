#pragma once

#include <QWidget>

#include "tools/compiler/compilerv2.hpp"
#include "tools/compiler/compilerv3.hpp"
#include "tools/compiler/compilerv4.hpp"
#include "tools/sceneproperties/sceneincludesv5.hpp"

class SceneEntity;

namespace Ui
{
class SceneObjectProperties;
}

class SceneObjectProperties : public QWidget
{
    Q_OBJECT

public:
    explicit SceneObjectProperties(QWidget *parent = nullptr);
    ~SceneObjectProperties();

    void setupUI(SceneEntity *entity, int entityID, Compilerv2::Entity *entityv2,
                 Compilerv3::Entity *entityv3, Compilerv4::Entity *entityv4, byte ver);
    void unsetUI();
    void hideUI();

    void updateUI(bool updateEnum = false);
    void updateUI(SceneEntity *entity, bool updateEnum = false);

    int callRSDKEdit(void *e, bool shouldReturnVal, int entityID, int variableID, int variableValue,
                     bool *called = nullptr);

    PropertyBrowser *properties = nullptr;
signals:
    void entityChanged(SceneEntity *entity, QVariant prevValue, int varID);
    void entityMoved(SceneEntity *entity);
    void entityVarChanged(SceneEntity *entity, QVariant prevValue, int varPos, bool isCustomVar);
private:
    Ui::SceneObjectProperties *ui;

    SceneEntity *entityPtr = nullptr;
    byte gameType = 4;
    QMessageBox *msgBox = nullptr;
};


