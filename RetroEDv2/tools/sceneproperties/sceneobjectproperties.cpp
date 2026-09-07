#include "includes.hpp"
#include "ui_sceneobjectproperties.h"
#include "sceneobjectproperties.hpp"

#include "tools/sceneeditor.hpp"
#include "tools/sceneviewer.hpp"

#include "tools/sceneproperties/sceneincludesv5.hpp"

#include "tools/compiler/compilerv2.hpp"
#include "tools/compiler/compilerv3.hpp"
#include "tools/compiler/compilerv4.hpp"

#include <RSDKv4/scenev4.hpp>


SceneObjectProperties::SceneObjectProperties(QWidget *parent)
    : QWidget(parent), ui(new Ui::SceneObjectProperties)
{
    ui->setupUi(this);

    properties = new PropertyBrowser;
    ui->gridLayout->addWidget(properties);
}

SceneObjectProperties::~SceneObjectProperties() { delete ui; }

void SceneObjectProperties::setupUI(SceneEntity *entity, int entityID, Compilerv2::Entity *entityv2,
                                    Compilerv3::Entity *entityv3, Compilerv4::Entity *entityv4,
                                    byte ver)
{
    unsetUI();
    entityPtr = entity;
    gameType = ver;
    SceneObject &object = entityPtr->type < scnEditor->viewer->objects.count() ? scnEditor->viewer->objects[entityPtr->type] : scnEditor->viewer->objects[0];

    QList<PropertyValue> objNames;
    for (int o = 0; o < scnEditor->viewer->objects.count(); ++o) {
        PropertyValue value;
        value.name  = scnEditor->viewer->objects[o].name;
        value.value = o;
        objNames.append(value);
    }

    QList<Property *> entityGroup = {
        new Property("object"),
        new Property("position"),
    };

    QList<Property *> infoGroup = {
        new Property("type", objNames, &entityPtr->type, Property::BYTE_MANAGER),
        new Property("slot", &entityPtr->slotID),
        new Property(object.variablesAliases[VAR_ALIAS_PROPVAL], &entityPtr->propertyValue),
    };

    //infoGroup[1]->setRange(0, scnEditor->viewer->entities.count() - 1);

    connect(infoGroup[0], &Property::changed, [this, infoGroup] {
        emit entityChanged(entityPtr, infoGroup[0]->prevValue, 0);
    });

    connect(infoGroup[1], &Property::changed, [this, infoGroup] {
        if (entityPtr->slotID >= scnEditor->viewer->entities.count())
            entityPtr->slotID = scnEditor->viewer->entities.count() - 1;
        emit entityChanged(entityPtr, infoGroup[1]->prevValue, 1);
    });

    connect(infoGroup[2], &Property::changed, [this, infoGroup] {
        emit entityChanged(entityPtr, infoGroup[2]->prevValue, 2);
    });

    QList<Property *> posGroup = { new Property("x", &entityPtr->pos.x),
                                   new Property("y", &entityPtr->pos.y) };

    connect(posGroup[0], &Property::changed, [this, posGroup] {
        emit entityChanged(entityPtr, posGroup[0]->prevValue, 3);
    });

    connect(posGroup[1], &Property::changed, [this, posGroup] {
        emit entityChanged(entityPtr, posGroup[1]->prevValue, 4);
    });


    QList<PropertyValue> flipFlags     = { PropertyValue("No Flip", 0), PropertyValue("Flip X", 1),
                                           PropertyValue("Flip Y", 2), PropertyValue("Flip XY", 3) };
    QList<PropertyValue> inkEffects    = { PropertyValue("No Ink", 0), PropertyValue("Blended", 1),
                                           PropertyValue("Alpha", 2), PropertyValue("Additive", 3),
                                           PropertyValue("Subtractive", 4) };
    QList<PropertyValue> priorityFlags = { PropertyValue("Bounds", 0),
                                           PropertyValue("Active", 1),
                                           PropertyValue("Always", 2),
                                           PropertyValue("X Bounds", 3),
                                           PropertyValue("X Bounds (Destroy)", 4),
                                           PropertyValue("Inactive", 5),
                                           PropertyValue("Bounds (Small)", 6),
                                           PropertyValue("Active (Small)", 7) };

    QList<Property *> varGroup = {};

    Property *variable[0xF];

    // Variables
    for (int v = 0; v < 0xF; ++v) {
        QString name = RSDKv4::objectVariableNames[v];
        if (v >= 11)
            name = object.variablesAliases[VAR_ALIAS_VAL0 + (v - 11)];

        Property *group = new Property(name);
        switch (v) {
            default: {
                variable[v] =
                    new Property(RSDKv4::objectVariableTypes[v], &entityPtr->sceneVariables[v].value);
                break;
            }

            case 1:
                variable[v] = new Property(RSDKv4::objectVariableTypes[v], flipFlags,
                                           &entityPtr->sceneVariables[v].value, Property::BYTE_MANAGER);
                break;
            case 5:
                variable[v] = new Property(RSDKv4::objectVariableTypes[v], priorityFlags,
                                           &entityPtr->sceneVariables[v].value, Property::BYTE_MANAGER);
                break;
            case 10:
                variable[v] = new Property(RSDKv4::objectVariableTypes[v], inkEffects,
                                           &entityPtr->sceneVariables[v].value, Property::BYTE_MANAGER);
                break;
        }

        QList<Property *> valGroup = { variable[v] };

        disconnect(variable[v], nullptr, nullptr, nullptr);
        connect(variable[v], &Property::changed, [=] {
            emit entityVarChanged(entityPtr, variable[v]->prevValue, v, false);
        });

        group->setSubProperties(valGroup);
        varGroup.append(group);
    }

    if (ver == ENGINE_v4) {
        entityGroup.append(new Property("object variables"));
        entityGroup[2]->setSubProperties(varGroup);
    }

    Property *userVars = new Property("editor variables");

    for (int v = 0; v < entityPtr->variables.count(); ++v) {
        auto &var = entityPtr->variables[v];

        SceneObject *object   = &scnEditor->viewer->objects[entityPtr->type];
        VariableInfo &varInfo = scnEditor->viewer->objects[entityPtr->type].variables[v];
        Property *group       = new Property(varInfo.name);
        QList<Property *> valGroup;

        QList<PropertyValue> aliases;
        if (object) {
            for (auto &value : varInfo.values) {
                PropertyValue val;
                val.name  = value.name;
                val.value = value.value;
                aliases.append(val);
            }
        }

        var.value_int32 = callRSDKEdit(scnEditor, true, entityPtr->gameEntitySlot, v, 0);

        if (aliases.count()) {
            valGroup.append(new Property("enum", aliases, &var.value_int32, Property::INT_MANAGER));
        }
        else {
            valGroup.append(new Property("int32", &var.value_int32));
        }

        Property *prop = valGroup.last();
        disconnect(prop, nullptr, nullptr, nullptr);
        connect(prop, &Property::changed,[this, prop, v, entity] {
            emit entityVarChanged(entity, prop->prevValue, v, true);
        });

        group->setSubProperties(valGroup);
        userVars->subProperties.append(group);
    }

    entityGroup[0]->setSubProperties(infoGroup);
    entityGroup[1]->setSubProperties(posGroup);
    properties->setPropertySet(entityGroup);

    properties->addProperty(userVars->p);
    properties->propertySet.append(userVars);
}

void SceneObjectProperties::unsetUI()
{
    properties->clear();
    entityPtr = nullptr;
}

void SceneObjectProperties::hideUI()
{
    properties->setHidden(true);
    entityPtr = nullptr;
}

void SceneObjectProperties::updateUI(bool updateEnum)
{
    if (!entityPtr)
        return;
    int varGroup = gameType == ENGINE_v4 ? 3 : 2;

   //cheap way of removing the qcombobox generated by the enum-type properties
   //prevents the visual of the temp combobox not matching the real value when using redo/undo
    if (properties->currentItem() && updateEnum){
        properties->currentItem()->property()->setEnabled(false);
        properties->currentItem()->property()->setEnabled(true);
    }

    for (auto &prop : properties->propertySet){
        for (auto &subProp : prop->subProperties){
            subProp->blockSignals(true);
            subProp->updateValue();
            subProp->blockSignals(false);
            for (auto &subPropVal : subProp->subProperties){
                subPropVal->blockSignals(true);
                subPropVal->updateValue();
                subPropVal->blockSignals(false);
            }
        }
    }
    for (int v = 0; v < entityPtr->variables.count(); ++v) {
        auto &var = entityPtr->variables[v];
        // the custom enums for some reason detach the pointer if i just update the value?
        properties->propertySet[varGroup]->subProperties[v]->subProperties[0]->blockSignals(true);
        properties->propertySet[varGroup]->subProperties[v]->subProperties[0]->setValuePtr(&var.value_int32);
        properties->propertySet[varGroup]->subProperties[v]->subProperties[0]->updateValue();
        properties->propertySet[varGroup]->subProperties[v]->subProperties[0]->blockSignals(false);
    }
    properties->blockSignals(false);
}

void SceneObjectProperties::updateUI(SceneEntity *entity, bool updateEnum)
{
    if (!entity)
        return;

    //cheap way of removing the qcombobox generated by the enum-type properties
    //prevents the visual of the temp combobox not matching the real value when using redo/undo
    if (properties->currentItem() && updateEnum){
        properties->currentItem()->property()->setEnabled(false);
        properties->currentItem()->property()->setEnabled(true);
    }

    entityPtr = entity;

    properties->blockSignals(true);
    for (auto &prop : properties->propertySet){
        for (auto &subProp : prop->subProperties){
            subProp->blockSignals(true);
            for (auto &subPropVal : subProp->subProperties){
                subPropVal->blockSignals(true);
            }
        }
    }
    properties->propertySet[0]->subProperties[0]->setValuePtr(&entityPtr->type);
    properties->propertySet[0]->subProperties[1]->setValuePtr(&entityPtr->slotID);
    properties->propertySet[0]->subProperties[2]->setValuePtr(&entityPtr->propertyValue);
    properties->propertySet[1]->subProperties[0]->setValuePtr(&entityPtr->pos.x);
    properties->propertySet[1]->subProperties[1]->setValuePtr(&entityPtr->pos.y);
    if (gameType == ENGINE_v4){
        for (int i = 0; i < 0xF; i++)
            properties->propertySet[2]->subProperties[i]->subProperties[0]->setValuePtr(&entityPtr->sceneVariables[i].value);
    }

    int startGroup = gameType == ENGINE_v4 ? 3 : 2;
    for (int i = properties->propertySet[startGroup]->subProperties.count() - 1; i >= 0; i--)
        properties->propertySet[startGroup]->removeSubProperty(properties->propertySet[startGroup]->subProperties[i]);

    SceneObject *object   = &scnEditor->viewer->objects[entityPtr->type];
    for (int v = 0; v < object->variables.count(); ++v) {
        VariableInfo &varInfo = object->variables[v];
        Property *group       = new Property(varInfo.name);
        QList<Property *> valGroup;

        QList<PropertyValue> aliases;
        for (auto &value : varInfo.values) {
            PropertyValue val;
            val.name  = value.name;
            val.value = value.value;
            aliases.append(val);
        }

        auto &var = entityPtr->variables[v];
        var.value_int32 = callRSDKEdit(scnEditor, true, entityPtr->slotID, v, 0);

        if (aliases.count()) {
            valGroup.append(new Property("enum", aliases, &var.value_int32, Property::INT_MANAGER));
        }
        else {
            valGroup.append(new Property("int32", &var.value_int32));
        }

        Property *prop = valGroup.last();
        disconnect(prop, nullptr, nullptr, nullptr);
        connect(prop, &Property::changed,[this, prop, v] {
            emit entityVarChanged(entityPtr, prop->prevValue, v, true);
        });

        group->setSubProperties(valGroup);
        properties->propertySet[startGroup]->addSubProperty(group);
    }

    for (auto &prop : properties->propertySet){
        for (auto &subProp : prop->subProperties){
            subProp->blockSignals(false);
            for (auto &subPropVal : subProp->subProperties){
                subPropVal->blockSignals(false);
            }
        }
    }
    properties->updateDelegates();
    properties->blockSignals(false);
    properties->setHidden(false);
}

int SceneObjectProperties::callRSDKEdit(void *e, bool shouldReturnVal, int entityID, int variableID,
                                        int variableValue, bool *called)
{
    SceneEditor *editor = (SceneEditor *)e;

    editor->viewer->variableID                = variableID;
    editor->viewer->variableValue             = variableValue;
    editor->viewer->returnVariable            = shouldReturnVal;
    editor->compilerv3->scriptEng.checkResult = -1;
    editor->compilerv4->scriptEng.checkResult = -1;
    bool c                         = editor->CallGameEvent(SceneViewer::EVENT_EDIT, entityID);
    editor->viewer->returnVariable = false;

    if (called) {
        *called = c;

        if (variableID == -1 && shouldReturnVal) {
            if (editor->viewer->gameType == ENGINE_v4)
                *called = c && editor->compilerv4->scriptEng.checkResult >= 0;
            else if (editor->viewer->gameType == ENGINE_v3)
                *called = c && editor->compilerv3->scriptEng.checkResult >= 0;
        }
    }

    if (editor->viewer->gameType == ENGINE_v4)
        return editor->compilerv4->scriptEng.checkResult;
    else if (editor->viewer->gameType == ENGINE_v3)
        return editor->compilerv3->scriptEng.checkResult;

    return 0;
}

#include "moc_sceneobjectproperties.cpp"
