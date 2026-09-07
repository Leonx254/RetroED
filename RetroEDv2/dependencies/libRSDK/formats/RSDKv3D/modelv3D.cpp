#include "libRSDK.hpp"

#include "modelv3D.hpp"

void RSDKv3D::Model::read(Reader &reader)
{
    filePath = reader.filePath;

    ushort numVertices = reader.read<ushort>();
    mdl.vertices.clear();
    for (int i = 0; i < numVertices; i++){
        mdl.vertices.append(reader.read<Vertex>());
    }

    mdlBase.vertices = mdl.vertices;

    ushort indexCount = reader.read<ushort>();
    mdl.indices.clear();
    for (int i = 0; i < indexCount * 3; ++i) mdl.indices.append(reader.read<ushort>());

    mdlBase.indices = mdl.indices;
}

void RSDKv3D::Model::write(Writer &writer)
{
    filePath = writer.filePath;
    writer.write(signature, 4);
    writer.flush();
}

void RSDKv3D::Model::writeAsOBJ(QString filePath, int exportFrame)
{
}

int RSDKv3D::Model::getPLYPropertyType(QString propertyType)
{
    QString type = propertyType;
    type         = type.toLower();

    if (type == "char" || type == "int8") {
        return PLYPropTypes::INT8;
    }
    else if (type == "uchar" || type == "uint8") {
        return PLYPropTypes::UINT8;
    }
    else if (type == "short" || type == "int16") {
        return PLYPropTypes::INT16;
    }
    else if (type == "ushort" || type == "uint16") {
        return PLYPropTypes::UINT16;
    }
    else if (type == "int32" || type == "int") {
        return PLYPropTypes::INT32;
    }
    else if (type == "uint32" || type == "uint") {
        return PLYPropTypes::UINT32;
    }
    else if (type == "float" || type == "float32") {
        return PLYPropTypes::FLOAT;
    }
    else if (type == "double64" || type == "double" || type == "float64") {
        return PLYPropTypes::DOUBLE;
    }
    else if (type == "list") {
        return PLYPropTypes::LIST;
    }

    return PLYPropTypes::UNKNOWN;
}

void RSDKv3D::Model::loadPLY(QString filePath)
{
}

void RSDKv3D::Model::writeAsPLY(QString filePath, int exportFrame)
{
}
