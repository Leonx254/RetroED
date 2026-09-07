#include "includes.hpp"
#include "ui_scenelayerproperties.h"
#include "scenelayerproperties.hpp"

#include "tools/sceneviewer.hpp"

SceneLayerProperties::SceneLayerProperties(QWidget *parent)
    : QWidget(parent), ui(new Ui::SceneLayerProperties)
{
    ui->setupUi(this);
}

SceneLayerProperties::~SceneLayerProperties() { delete ui; }

void SceneLayerProperties::setupUI(SceneViewer *viewer, byte layerID)
{
    unsetUI();

    auto *tileLayer = &viewer->layers[layerID];
    if (layerID == 0) {
        ui->type->setDisabled(true);
        ui->parallaxFactor->setDisabled(true);
        ui->scrollSpeed->setDisabled(true);

        ui->width->setValue(tileLayer->width);
        ui->height->setValue(tileLayer->height);
        ui->type->setCurrentIndex(1);
        ui->parallaxFactor->setValue(1.0f);
        ui->scrollSpeed->setValue(0.0f);
    }
    else {
        ui->type->setDisabled(false);
        ui->parallaxFactor->setDisabled(false);
        ui->scrollSpeed->setDisabled(false);

        ui->width->setValue(tileLayer->width);
        ui->height->setValue(tileLayer->height);
        ui->type->setCurrentIndex(tileLayer->type);
        ui->parallaxFactor->setValue(tileLayer->parallaxFactor);
        ui->scrollSpeed->setValue(tileLayer->scrollSpeed);
    }

    connect(ui->width, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
        emit updateLayerSize(v - tileLayer->width);
    });

    connect(ui->height, QOverload<int>::of(&QSpinBox::valueChanged), [=](int v) {
        emit updateLayerSize(v - tileLayer->height, true);
    });

    if (layerID > 0) {
        connect(ui->type, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int v){ emit updateType(v);});
        connect(ui->parallaxFactor, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double v) { emit updateParallax(v - tileLayer->parallaxFactor);});
        connect(ui->scrollSpeed, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double v) { updateScroll(v - tileLayer->scrollSpeed);});
    }
}

void SceneLayerProperties::unsetUI()
{
    disconnect(ui->width, nullptr, nullptr, nullptr);
    disconnect(ui->height, nullptr, nullptr, nullptr);
    disconnect(ui->type, nullptr, nullptr, nullptr);
    disconnect(ui->parallaxFactor, nullptr, nullptr, nullptr);
    disconnect(ui->scrollSpeed, nullptr, nullptr, nullptr);
}

#include "moc_scenelayerproperties.cpp"
