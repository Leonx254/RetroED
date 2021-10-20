#ifndef SCENEOBJECTPROPERTIES_V5_H
#define SCENEOBJECTPROPERTIES_V5_H

#include <QWidget>

class VariableValue
{
public:
    QString name = "Variable";
    int value    = 0;

    VariableValue() {}
};

class VariableInfo
{
public:
    QString name = "Variable";
    byte type    = 0;
    int offset   = -1;
    QByteArray hash;
    QList<VariableValue> values;

    VariableInfo()
    {
        hash = Utils::getMd5HashByteArray(name);
        values.clear();
    }
    VariableInfo(QString n, byte t, int o) : name(n), type(t), offset(o)
    {
        hash = Utils::getMd5HashByteArray(name);
        values.clear();
    }
};

struct SceneEntity {
    ushort slotID          = 0;
    ushort prevSlot        = 0;
    byte type              = 0;
    Vector2<float> pos     = Vector2<float>(0, 0);
    GameEntity *gameEntity = NULL;
    QList<RSDKv5::Scene::VariableValue> variables;

    SceneEntity() {}

    bool operator==(const SceneEntity &other) const
    {
        return slotID == other.slotID && type == other.type && pos.x == other.pos.x
               && pos.y == other.pos.y /*&& variables == other.variables*/;
    }
};

struct SceneObject {
    QString name = "";
    QList<VariableInfo> variables;

    SceneObject() {}

    bool operator==(const SceneObject &other) const
    {
        return name == other.name /*&& variables == other.variables*/;
    }
};

namespace Ui
{
class SceneObjectPropertiesv5;
}

class SceneObjectPropertiesv5 : public QWidget
{
    Q_OBJECT

public:
    explicit SceneObjectPropertiesv5(QWidget *parent = nullptr);
    ~SceneObjectPropertiesv5();

    void setupUI(SceneEntity *entity);
    void unsetUI();

    void updateUI();

    PropertyBrowser *properties = nullptr;

signals:
    void typeChanged(SceneEntity *entity, byte type);

private:
    Ui::SceneObjectPropertiesv5 *ui;

    SceneEntity *entityPtr = nullptr;

    QMessageBox *msgBox = nullptr;
};

#endif // SCENEOBJECTPROPERTIES_V5_H
