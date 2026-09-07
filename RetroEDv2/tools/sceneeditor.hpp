#pragma once

#include <QWidget>

#include "tools/sceneviewer.hpp"

#include "tools/compiler/compilerv2.hpp"
#include "tools/compiler/compilerv3.hpp"
#include "tools/compiler/compilerv4.hpp"

class SceneProperties;
class SceneLayerProperties;
class SceneTileProperties;
class SceneObjectProperties;
class SceneScrollProperties;
class ChunkEditor;
class ChunkReplaceOptions;
class ChunkSelector;
class TilesetEditor;
class ScenePreviewPalette;
class StageImport;

#include <RSDKv4/tileconfigv4.hpp>

namespace Ui
{
class SceneEditor;
}

class ChunkMap
{
public:
    ChunkMap() {}
    ChunkMap(const ChunkMap &other){
        pos = other.pos;
        id = other.id;
    };
    ChunkMap &operator=(const ChunkMap &other){
        ChunkMap *ret = new ChunkMap();
        ret->pos = other.pos;
        ret->id = other.id;
        return *ret;
    };
    Vector2<int> pos;
    int id;
};

inline bool operator==(const ChunkMap &c1, const ChunkMap &c2)
{
    return c1.id == c2.id && c1.pos == c2.pos;
}

inline uint qHash(const ChunkMap &key, uint seed)
{
    return qHash(((key.pos.x + key.pos.y) * 0.5) * (key.pos.x + key.pos.y + 1) + key.pos.y, seed) ^ key.id;
}

class SceneEditor : public QWidget
{
    Q_OBJECT

public:

    class EntityCommand : public QUndoCommand
    {
    public:
        explicit EntityCommand(SceneEntity ent, QVariant prevValue, int varID, SceneEditor *parent = nullptr);
        explicit EntityCommand(SceneEntity ent, QVariant prevValue, int varPos, bool isCustomVar, SceneEditor *parent = nullptr);
        void undo() override { ResetEntity(false); };
        void redo() override { ResetEntity(true); };
        void ResetEntity(bool isRedo);

    private:
        SceneEntity prevEnt;
        SceneEntity curEnt;

        Compilerv2::Entity prevEntv2;
        Compilerv3::Entity prevEntv3;
        Compilerv4::Entity prevEntv4;

        Compilerv2::Entity curEntv2;
        Compilerv3::Entity curEntv3;
        Compilerv4::Entity curEntv4;

        int swapSlot = -1;
        int slotID;
        SceneEditor *scnEditor = nullptr;
    };

    class EntityMoveCommand : public QUndoCommand
    {
    public:
        explicit EntityMoveCommand(QList<int> entityIDs, QList<Vector2<float>> curPos, QList<Vector2<float>> prevPos, SceneEditor *parent = nullptr);
        void undo() override { MoveEntity(false); };
        void redo() override { MoveEntity(true); };
        void MoveEntity(bool isRedo);

    private:
        QList<int> IDs;
        QList<Vector2<float>> curPos;
        QList<Vector2<float>> prevPos;
        SceneEditor *scnEditor = nullptr;
    };

    class EntityAddRemoveCommand : public QUndoCommand
    {
    public:
        explicit EntityAddRemoveCommand(int id, SceneEditor *parent = nullptr);
        explicit EntityAddRemoveCommand(QList<int> IDs, SceneEditor *parent = nullptr);
        explicit EntityAddRemoveCommand(SceneEntity entity, SceneEditor *parent = nullptr);
        explicit EntityAddRemoveCommand(QList<SceneEntity> entities, SceneEditor *parent = nullptr);
        void AddRemoveEntity(bool isRedo);
        void AddEntity(SceneEntity ent, int slotID);
        void RemoveEntity(int id);
        void undo() override { AddRemoveEntity(false); };
        void redo() override { AddRemoveEntity(true); };


    private:
        QList<int> entIDs;
        QList<SceneEntity> entities;
        QList<Compilerv2::Entity> entitiesv2;
        QList<Compilerv3::Entity> entitiesv3;
        QList<Compilerv4::Entity> entitiesv4;
        bool isNewEntity = false;
        SceneEditor *scnEditor = nullptr;
    };

    class LayerChangeCommand : public QUndoCommand
    {
    public:
        enum { Id = 3 };
        explicit LayerChangeCommand(Vector2<int> shift, bool keepDimensions, bool shiftEnt, SceneEditor *parent = nullptr);
        explicit LayerChangeCommand(Vector2<int> shift, SceneEditor *parent = nullptr);
        explicit LayerChangeCommand(float val, byte type, SceneEditor *parent = nullptr);
        explicit LayerChangeCommand(byte val, SceneEditor *parent = nullptr);
        explicit LayerChangeCommand(QSet<ChunkMap> map, int layer, SceneEditor *parent = nullptr);

        bool mergeWith(const QUndoCommand *command) override{
            const LayerChangeCommand *cmd = static_cast<const LayerChangeCommand *>(command);
            float commandParallax = cmd->shiftParallaxVal;
            float commandScroll   = cmd->shiftScrollVal;
            if (id() != cmd->id())
                return false;
            bool merge = false;
            if (commandParallax == 1 || commandParallax == -1)
                merge = true;
            else if (commandScroll == 1 || commandScroll == -1)
                merge = true;

            if (merge){
                if (commandParallax != shiftParallaxVal || commandScroll != shiftScrollVal){
                    shiftParallaxVal += commandParallax;
                    shiftScrollVal   += commandScroll;
                }
                return true;
            }

            return false;
        };
        int id() const override { return Id; }
        void ResizeLayer(bool isRedo);
        void ShiftLayer(bool isRedo);
        void ChangeLayerSettings(bool isRedo);
        void ChangeLayout(bool isRedo);
        void undo() override {
            if (this->text() == "Modified Layout") ChangeLayout(false);
            if (this->text() == "Resized Layer") ResizeLayer(false);
            if (this->text() == "Shifted Layer") ShiftLayer(false);
            if (this->text() == "Changed Layer Parallax" || this->text() == "Changed Layer Scroll Speed" || this->text() == "Changed Layer Type")
                ChangeLayerSettings(false);
        };
        void redo() override {
            if (this->text() == "Modified Layout") ChangeLayout(true);
            if (this->text() == "Resized Layer") ResizeLayer(true);
            if (this->text() == "Shifted Layer") ShiftLayer(true);
            if (this->text() == "Changed Layer Parallax" || this->text() == "Changed Layer Scroll Speed" || this->text() == "Changed Layer Type")
                ChangeLayerSettings(true);
        };

    private:
        float shiftParallaxVal = 0.0f;
        float shiftScrollVal = 0.0f;
        byte prevLyrType;
        byte curLyrType;

        QSet<ChunkMap> prevLayout;
        QSet<ChunkMap> curLayout;
        int count;
        byte layer;
        Vector2<int> lyrShift;
        bool keepDimensions;
        bool shiftEntities;
        byte commandType;
        SceneEditor *scnEditor = nullptr;
    };
    class ParallaxCommand : public QUndoCommand
    {
    public:
        explicit ParallaxCommand(int entry, bool isVert, bool remove, SceneEditor *parent = nullptr);
        explicit ParallaxCommand(int entry, bool isVert, int instRow, bool remove, SceneEditor *parent = nullptr);
        explicit ParallaxCommand(int entry, bool isVert, float value, byte option, SceneEditor *parent = nullptr);
        explicit ParallaxCommand(int entry, bool isVert, int instRow, int value, byte option, SceneEditor *parent = nullptr);

        void ChangeParallaxInstance(bool isRedo);
        void ChangeParallaxEntry(bool isRedo);
        void undo() override { ChangeParallaxEntry(false); };
        void redo() override { ChangeParallaxEntry(true); };
    private:
        SceneHelpers::TileLayer::ScrollIndexInfo prevEntry;
        SceneHelpers::TileLayer::ScrollIndexInfo curEntry;
        int entryRow = 0;
        bool isNewEntry = false;
        bool vEntry     = false;
        bool instEdit   = false;
        SceneEditor *scnEditor = nullptr;
    };

    class ActionState
    {
    public:
        QString name = "Action";

        QList<PaletteColor> tilePalette;
        QList<QImage> tiles;
        QList<QImage> chunks;

        FormatHelpers::GameConfig gameConfig;

        FormatHelpers::Scene scene;
        FormatHelpers::Background background;
        FormatHelpers::Chunks chunkset;
        FormatHelpers::StageConfig stageConfig;

        RSDKv4::TileConfig tileconfig;

        QList<SceneObject> objects;
        QList<SceneEntity> entities;

        // General Editing
        byte curTool            = SceneViewer::TOOL_MOUSE;
        bool selecting          = false;
        Vector2<float> mousePos = Vector2<float>(0.0f, 0.0f);

        // Layer Editing
        Vector2<float> tilePos = Vector2<float>(0.0f, 0.0f);
        Vector2<bool> tileFlip = Vector2<bool>(false, false);
        int selectedTile       = -1;
        int selectedLayer      = -1;

        // Collision
        bool showPlaneA = false;
        bool showPlaneB = false;

        // Entity Editing
        int selectedObject = -1; // placing
        int selectedEntity = -1; // viewing

        // Parallax Editing
        bool showParallax      = false;
        int selectedScrollInfo = -1;

        // Camera
        Vector2<float> camPos = Vector2<float>(0.0f, 0.0f);

        bool showTileGrid = false;
    };

    explicit SceneEditor(QWidget *parent = nullptr);
    ~SceneEditor();

    bool mouseDownL = false;
    bool mouseDownM = false;
    bool mouseDownR = false;
    bool ctrlDownL  = false;
    bool altDownL   = false;
    bool shiftDownL = false;

    bool useDCFormat  = false;
    bool viewerActive = false; // prevents shortcut windows from opening twice when sceneViewer is active

    Vector2<float> selectionOffset = Vector2<float>(0.0f, 0.0f);

    SceneViewer *viewer               = nullptr;
    SceneProperties *scnProp          = nullptr;
    SceneLayerProperties *lyrProp     = nullptr;
    SceneTileProperties *tileProp     = nullptr;
    SceneObjectProperties *objProp    = nullptr;
    SceneScrollProperties *scrProp    = nullptr;
    ChunkSelector *chkProp            = nullptr;
    TilesetEditor *tsetEdit           = nullptr;
    ScenePreviewPalette *palView      = nullptr;

    SceneEntity createTempEntity;

    FormatHelpers::GameConfig gameConfig;

    FormatHelpers::Scene scene;
    FormatHelpers::Background background;
    FormatHelpers::Chunks chunkset;
    FormatHelpers::StageConfig stageConfig;

    void CreateNewScene(QString scnPath, byte scnVer, bool loadGC, QString gcPath);
    void LoadScene(QString scnPath, QString gcfPath, byte gameType);
    bool SaveScene(bool forceSaveAs = false);

    void UnloadGameLinks();
    void InitGameLink();

    bool CallGameEvent(byte eventID, int id);
    ushort LoadSpriteSheet(QString filename);
    void DrawSpriteFlipped(float XPos, float YPos, float width, float height, float sprX, float sprY,
                           int direction, InkEffects inkEffect, int alpha, int sheetID,
                           bool screenRelative);
    void DrawSpriteRotozoom(float XPos, float YPos, float pivotX, float pivotY, float width,
                            float height, float sprX, float sprY, int scaleX, int scaleY, int direction,
                            short rotation, InkEffects inkEffect, int alpha, int sheetID,
                            bool screenRelative);
    Compilerv2 *compilerv2 = nullptr;
    Compilerv3 *compilerv3 = nullptr;
    Compilerv4 *compilerv4 = nullptr;
    bool scriptError       = false;

    // Event Handlers
    QSet<ChunkMap> chunkLayoutStore;
    QList<Vector2<float>> entityMoveStore;
    void SetChunk(float x, float y, QSet<ChunkMap> &prevLayout);
    void ResetTools(byte tool);

    bool HandleKeyPress(QKeyEvent *event);
    bool HandleKeyRelease(QKeyEvent *event);

    void CreateScrollList(bool update = false);

    // Event stuff
    bool waitForRelease = false;

    inline void UpdateTitle(bool modified)
    {
        this->modified = modified;
        if (modified)
            emit TitleChanged(tabTitle + " *", tabPath);
        else
            emit TitleChanged(tabTitle, tabPath);
    }

    void UndoAction();
    void RedoAction();
    void ResetAction();
    void DoAction(QString name = "Action", bool setModified = true);
    void ClearActions();


signals:
    void TitleChanged(QString title, QString tabFullPath);
    void calcAngles(RSDKv5::TileConfig::CollisionMask *outputAngles, RSDKv1::TileConfig::CollisionMask *inputMask);
public slots:
    void updateType(SceneEntity *entity, byte type, bool keepVals = false);
protected:
    bool event(QEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

private:
    enum SceneManagerCopyTypes {
        COPY_NONE,
        COPY_LAYER,
        COPY_CHUNK,
        COPY_ENTITY,
        COPY_ENTITY_SELECT,
        COPY_SCROLLINFO,
    };
    void *clipboard    = nullptr;
    QList<int> clipboardIDs;
    QList<Vector2<float>> clipboardOffset;
    Vector2<float> clipPosCenter;
    byte clipboardType = COPY_NONE;
    int clipboardInfo  = 0;

    void DeleteEntity(int slot, bool updateUI = false);

    void FilterObjectList(QString filter);
    void FilterEntityList(QString filter);

    void CreateEntityList(int startSlot = -1);
    void CenterCameraToEntity(int entityID);

    // XML Management
    void ParseGameXML(QString path);

    inline void writeXMLIndentation(Writer &writer, int tabCount)
    {
        for (int t = 0; t < tabCount; ++t) writer.write<char>('\t');
    }

    void ReadXMLScrollInfo(QXmlStreamReader &xmlReader, int layerID, byte mode = 0);
    void ReadXMLLayout(QXmlStreamReader &xmlReader, int layerID, byte mode = 0);
    void ReadXMLLayers(QXmlStreamReader &xmlReader);

    void WriteXMLScrollInfo(Writer &writer, int layerID, int indentPos);
    void WriteXMLLayout(Writer &writer, int layerID, int indentPos);
    void WriteXMLLayer(Writer &writer, int layerID, int indentPos);

    void WriteXMLObject(Writer &writer, int objID, int indentPos);
    void WriteXMLEntity(Writer &writer, int entityID, int indentPos);

    void WriteXMLChunk(Writer &writer, int chunkID, int indentPos);

    void WriteXMLScene(Writer &writer);

    Ui::SceneEditor *ui;

    ChunkEditor *chunkEdit = nullptr;
    //ChunkReplaceOptions *chunkRpl = nullptr;
    StageImport *stgImp = nullptr;
    QList<ActionState> actions;
    int actionIndex = 0;

    bool modified    = false;
    QString tabTitle = "Scene Editor";
    QString tabPath  = "";

    QUndoStack *undoStack = nullptr;
};

class ChunkLabel : public QLabel
{
    Q_OBJECT
public:
    ChunkLabel(ushort *sel, int index, QWidget *parent)
        : QLabel(parent), selectedChunk(sel), chunkIndex(index)
    {
    }

signals:
    void requestRepaint();

protected:
    void mousePressEvent(QMouseEvent *) override
    {
        *selectedChunk = chunkIndex;
        emit requestRepaint();
    }
    void paintEvent(QPaintEvent *event) override
    {
        QLabel::paintEvent(event);
        QPainter p(this);
        if (chunkIndex == *selectedChunk) {
            p.setBrush(qApp->palette().highlight());
            p.setOpacity(0.5);
            p.drawRect(this->rect());
        }
    }
    QSize sizeHint() const override { return QSize(0, 0); }

private:
    ushort *selectedChunk = nullptr;
    int chunkIndex;
};

class ChunkSelector : public QWidget
{
    Q_OBJECT
public:
    ChunkSelector(QWidget *parent = nullptr);

    void RefreshList();
    void SetCurrentChunk(int chunkID);

    SceneEditor *parentWidget = nullptr;

private:
    ChunkLabel *labels[0x200];

    QScrollArea *scrollArea;
};
