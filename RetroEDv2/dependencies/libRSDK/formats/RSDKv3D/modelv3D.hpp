#pragma once

#include "qlist.h"
#include "utils/vectors.hpp"

namespace RSDKv3D
{

class Model
{
public:
    struct AnimatorPart {
        QString name;
        ushort *indices;
        ushort numIndices;
        float x;
        float y;
        float z;
        float ZPosing[0x64];
        float YPosing[0x64];
        float XPosing[0x64];
    };

    struct AnimatorState {
        QString name;
        byte frameCount;
        ushort indices[0x80];
        byte loopIndex;
        byte frameDuration;
    };

    struct Animator {
        AnimatorPart nodes[36];
        AnimatorState states[10];
        byte *nodeIndices;
        ushort nodeCount;
        byte animationID;
        byte nextAnimation;
        ushort frameID;
        ushort nextFrame;
        ushort frameTimer;
    };

    struct Vertex {
        float x;
        float y;
        float z;
        float nx;
        float ny;
        float nz;
        float tu;
        float tv;

        Vertex() {}

        Vertex(const QVector3D &v, const QVector3D &n, float tu, float tv)
        {
            this->x  = v.x();
            this->y  = v.y();
            this->z  = v.z();
            this->nx = n.x();
            this->ny = n.y();
            this->nz = n.z();
            this->tu = tu;
            this->tv = tv;
        }
    };

    struct TMF {
        QList<Vertex> vertices;
        ushort numVertices;
        QList<ushort> indices;
        ushort numIndices;
    };

    Model() {}
    Model(QString filename) { read(filename); }
    Model(Reader &reader) { read(reader); }

    inline void read(QString filename)
    {
        Reader reader(filename);
        read(reader);
    }
    void read(Reader &reader);

    inline void write(QString filename = "")
    {
        if (filename == "")
            filename = filePath;
        if (filename == "")
            return;
        Writer writer(filename);
        write(writer);
    }
    void write(Writer &writer);

    void writeAsOBJ(QString filePath, int exportFrame = -1);

    void loadPLY(QString filePath);
    void writeAsPLY(QString filePath, int exportFrame = -1);

    byte signature[4] = { 'R', '3', 'D', 0 };

    QString filePath = "";

    TMF mdl;
    TMF mdlBase;

private:
    enum PLYPropTypes {
        UNKNOWN,
        INT8,
        UINT8,
        INT16,
        UINT16,
        INT32,
        UINT32,
        FLOAT,
        DOUBLE,
        LIST,
    };

    struct PLYProperty {
        PLYProperty() {}

        QString sizeType = ""; // used for list types
        QString itemType = ""; // used for list types
        QString type     = "";
        QString name     = "";
    };

    struct PLYPropertyValue {
        PLYPropertyValue() {}

        QList<QVariant> values; // if a list property exists, assume ONLY a list can exist
    };

    struct PLYElement {
        PLYElement() {}

        QList<PLYProperty> properties;
        QList<PLYPropertyValue> propertyValues;
        QString name = "";
    };

    int getPLYPropertyType(QString propertyType);
};

} // namespace RSDKv3D


