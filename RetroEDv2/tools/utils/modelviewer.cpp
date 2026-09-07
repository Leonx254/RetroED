#include "includes.hpp"

#include "modelviewer.hpp"

#include <RSDKv4/modelv4.hpp>
#include <RSDKv5/modelv5.hpp>
#include <RSDKv3D/modelv3D.hpp>

#define TO_RADIAN(degree) ((degree) * (RSDK_PI / 180.0f))

ModelViewer::ModelViewer(QWidget *parent) : QOpenGLWidget(parent)
{
    setMouseTracking(true);

    this->setFocusPolicy(Qt::WheelFocus);

    model = RSDKv5::Model();

    // renderTimer = new QTimer(this);
    // connect(renderTimer, &QTimer::timeout, this, [this] { this->repaint(); });
    // renderTimer->start(1000 / 60.f);
}

ModelViewer::~ModelViewer()
{
    curFrame  = nullptr;
    nextFrame = nullptr;
}

void ModelViewer::setModel(RSDKv5::Model m, QString tex)
{
    model   = m;
    texFile = tex;
    reload  = true;
}

void ModelViewer::setModel(RSDKv4::Model m, QString tex)
{
    model.faceVerticesCount = 3;
    model.indices           = m.indices;

    model.colors.clear();
    model.hasColors = false;

    model.texCoords.clear();
    for (auto &uv : m.texCoords) {
        RSDKv5::Model::TexCoord v5uv;
        v5uv.x = uv.x;
        v5uv.y = uv.y;
        model.texCoords.append(v5uv);
    }
    model.hasTextures = true;

    model.frames.clear();
    for (auto &f : m.frames) {
        RSDKv5::Model::Frame v5f;
        for (auto &v : f.vertices) {
            RSDKv5::Model::Frame::Vertex v5v;
            v5v.x  = v.x;
            v5v.y  = v.y;
            v5v.z  = v.z;
            v5v.nx = v.nx;
            v5v.ny = v.ny;
            v5v.nz = v.nz;
            v5f.vertices.append(v5v);
        }
        model.frames.append(v5f);
    }
    model.hasNormals = true;

    texFile = tex;
}

RSDKv4::Model ModelViewer::getModelv4()
{
    RSDKv4::Model mdl;

    mdl.texCoords.clear();
    if (model.hasTextures) {
        for (auto &texCoord : model.texCoords) {
            RSDKv4::Model::TexCoord coord;
            coord.x = texCoord.x;
            coord.y = texCoord.y;
            mdl.texCoords.append(coord);
        }
    }
    else {
        int vertCnt = 0;
        if (model.frames.count() >= 1)
            vertCnt = model.frames[0].vertices.count();

        for (int t = 0; t < vertCnt; ++t) {
            RSDKv4::Model::TexCoord coord;
            coord.x = 0;
            coord.y = 0;
            mdl.texCoords.append(coord);
        }
    }

    mdl.indices.clear();
    for (auto &index : model.indices) mdl.indices.append(index);

    mdl.frames.clear();
    for (auto &frame : model.frames) {
        RSDKv4::Model::Frame f;

        for (auto &vert : frame.vertices) {
            RSDKv4::Model::Frame::Vertex vertex;
            vertex.x = vert.x;
            vertex.y = vert.y;
            vertex.z = vert.z;

            vertex.nx = 0;
            vertex.ny = 0;
            vertex.nz = 0;

            if (model.hasNormals) {
                vertex.nx = vert.nx;
                vertex.ny = vert.ny;
                vertex.nz = vert.nz;
            }

            f.vertices.append(vertex);
        }

        mdl.frames.append(f);
    }

    return mdl;
}

void ModelViewer::setModel(RSDKv3D::Model::TMF mdl, QString tex)
{
    model.faceVerticesCount = 3;
    model.indices           = mdl.indices;

    model.colors.clear();
    model.hasColors = false;

    model.texCoords.clear();
    for (auto &uv : mdl.vertices) {
        RSDKv5::Model::TexCoord v5uv;
        v5uv.x = uv.tu;
        v5uv.y = uv.tv;
        model.texCoords.append(v5uv);
    }
    model.hasTextures = true;

    model.frames.clear();
    RSDKv5::Model::Frame v5f;
    for (auto &v : mdl.vertices) {
        RSDKv5::Model::Frame::Vertex v5v;
        v5v.x  = v.x;
        v5v.y  = v.y;
        v5v.z  = v.z;
        v5v.nx = v.nx;
        v5v.ny = v.ny;
        v5v.nz = v.nz;
        v5f.vertices.append(v5v);
    }
    model.frames.append(v5f);
    model.hasNormals = true;

    texFile = tex;
}

void ModelViewer::getAnimations(RSDKv3D::Model::Animator *animator, QString path)
{
    Reader reader(path);

    byte nodeCount = reader.read<byte>();
    ushort poseCount = reader.read<ushort>();

    for (int i = 0; i < nodeCount; ++i){
        RSDKv3D::Model::AnimatorPart *node = &animator->nodes[i];
        byte nameLength = reader.read<byte>();
        char cname[0x100];
        for (int n = 0; n < nameLength; ++n)
            cname[n] = reader.read<byte>();
        node->name = QString::fromUtf8(cname);
        node->x = reader.read<float>();
        node->y = reader.read<float>();
        node->z = reader.read<float>();

        node->numIndices = reader.read<ushort>();
        node->indices = new ushort[node->numIndices];
        for (int i = 0; i < node->numIndices; ++i)
            node->indices[i] = reader.read<ushort>();

        byte poseFlipped = 0;
        ushort poseValue = 0;
        for (int p = 0; p < poseCount; ++p){
            poseFlipped = reader.read<byte>();
            poseValue   = reader.read<ushort>();

            node->ZPosing[p] = -TO_RADIAN(poseValue);
            if (!poseFlipped)
                node->ZPosing[p] = -node->ZPosing[p];

            poseFlipped = reader.read<byte>();
            poseValue   = reader.read<ushort>();

            node->YPosing[p] = -TO_RADIAN(poseValue);
            if (!poseFlipped)
                node->YPosing[p] = -node->YPosing[p];

            poseFlipped = reader.read<byte>();
            poseValue = reader.read<ushort>();

            node->XPosing[p] = -TO_RADIAN(poseValue);
            if (!poseFlipped)
                node->XPosing[p] = -node->XPosing[p];

        }
    }

    animator->nodeCount = reader.read<ushort>();
    animator->nodeIndices = new byte[animator->nodeCount];

    for (int i = 0; i < animator->nodeCount; ++i)
        animator->nodeIndices[i] = reader.read<byte>();

    byte numStates = reader.read<byte>();

    for (int i = 0; i < numStates; ++i){
        RSDKv3D::Model::AnimatorState *state = &animator->states[i];
        byte nameLength = reader.read<byte>();
        QByteArray cname = reader.readByteArray(nameLength);
        state->name = QString::fromUtf8(cname);

        state->frameDuration = reader.read<byte>();
        state->loopIndex = reader.read<byte>();
        state->frameCount = reader.read<byte>();
        for (int f = 0; f < state->frameCount; ++f)
            state->indices[f] = reader.read<ushort>();
    }

    animator->animationID = 0;
    animator->nextAnimation = 0;
}
void ModelViewer::setPlayerVertexPositions(int nodeID)
{
    RSDKv3D::Model::Animator *anim = &S3DAni;
    RSDKv3D::Model::AnimatorPart *node = &anim->nodes[nodeID];

    QMatrix4x4 *matrix = &matModel;

    for (int i = 0; i < node->numIndices; ++i){
        RSDKv3D::Model::Vertex *vert = &S3Dmodel.mdl.vertices[node->indices[i]];
        RSDKv3D::Model::Vertex *baseVert = &S3Dmodel.mdlBase.vertices[node->indices[i]];
        QVector4D fstRow = matrix->row(0);
        QVector4D sndRow = matrix->row(1);
        QVector4D thrRow = matrix->row(2);
        vert->x = fstRow.x() * baseVert->x + fstRow.y() * baseVert->y + fstRow.z() * baseVert->z + fstRow.w();
        vert->y = sndRow.x() * baseVert->x + sndRow.y() * baseVert->y + sndRow.z() * baseVert->z + sndRow.w();
        vert->z = thrRow.x() * baseVert->x + thrRow.y() * baseVert->y + thrRow.z() * baseVert->z + thrRow.w();
    }
}
void ModelViewer::setPlayerVertexNormals(int nodeID)
{
    RSDKv3D::Model::Animator *anim = &S3DAni;
    RSDKv3D::Model::AnimatorPart *node = &anim->nodes[nodeID];

    QMatrix4x4 *matrix = &matModel;

    for (int i = 0; i < node->numIndices; ++i){
        RSDKv3D::Model::Vertex *vert = &S3Dmodel.mdl.vertices[node->indices[i]];
        RSDKv3D::Model::Vertex *baseVert = &S3Dmodel.mdlBase.vertices[node->indices[i]];
        QVector4D fstRow = matrix->row(0);
        QVector4D sndRow = matrix->row(1);
        QVector4D thrRow = matrix->row(2);
        vert->nx = fstRow.x() * baseVert->nx + fstRow.y() * baseVert->ny + fstRow.z() * baseVert->nz + fstRow.w();
        vert->ny = sndRow.x() * baseVert->nx + sndRow.y() * baseVert->ny + sndRow.z() * baseVert->nz + sndRow.w();
        vert->nz = thrRow.x() * baseVert->nx + thrRow.y() * baseVert->ny + thrRow.z() * baseVert->nz + thrRow.w();
    }
}

void ModelViewer::setAnimationFrame(){

    RSDKv3D::Model::Animator *anim = &S3DAni;
    QMatrix4x4 matWorld;

    ushort id = anim->animationID;
    if (anim->animationID != anim->nextAnimation)
        id = anim->nextAnimation;
    anim->frameTimer += anim->states[id].frameDuration;
    if (anim->frameTimer >= 240){
        anim->frameTimer -= 240;

        if (++anim->frameID >= anim->states[anim->animationID].frameCount)
            anim->frameID = anim->states[anim->animationID].loopIndex;

        if (anim->animationID != anim->nextAnimation){
            anim->animationID = anim->nextAnimation;
            anim->frameID = 0;
        }

        anim->nextFrame = anim->frameID + 1;
        if (anim->nextFrame >= anim->states[anim->animationID].frameCount)
            anim->nextFrame = anim->states[anim->animationID].loopIndex;
    }
    matrixIdentity.setToIdentity();
    float timer = anim->frameTimer / 240.0f;
    for (int i = 0; i < 36; ++i){
        RSDKv3D::Model::AnimatorPart *node = &anim->nodes[i];

        RSDKv3D::Model::AnimatorState *state = &anim->states[anim->animationID];
        RSDKv3D::Model::AnimatorState *stateNext = &anim->states[anim->nextAnimation];
        memcpy(&matrixSonicNodeRotation[i], &matModel, sizeof(matrixSonicNodeRotation[i]));
        float ZPosAnim = node->ZPosing[state->indices[anim->frameID]];
        float ZPosNext = node->ZPosing[stateNext->indices[anim->nextFrame]];
        matWorld.rotate((1.0f - timer) * ZPosAnim + (1.0f * timer) * ZPosNext, 0.0f, 0.0f, 1.0f);
        matrixSonicNodeRotation[i] *= matWorld;
        float YPosAnim = node->YPosing[state->indices[anim->frameID]];
        float YPosNext = node->YPosing[stateNext->indices[anim->nextFrame]];
        matWorld.rotate((1.0f - timer) * YPosAnim + (1.0f * timer) * YPosNext, 0.0f, 1.0f, 0.0f);
        matrixSonicNodeRotation[i] *= matWorld;

        float XPosAnim = node->XPosing[state->indices[anim->frameID]];
        float XPosNext = node->XPosing[stateNext->indices[anim->nextFrame]];
        matWorld.rotate((1.0f - timer) * XPosAnim + (1.0f * timer) * XPosNext, 1.0f, 0.0f, 0.0f);
        matrixSonicNodeRotation[i] *= matWorld;

        memcpy(&matrixSonicNodeTransform[i], &matModel, sizeof(matrixSonicNodeTransform[i]));
        matWorld.translate(-node->x, -node->y, -node->z);
        matrixSonicNodeTransform[i] *= matWorld;
        matrixSonicNodeTransform[i] *= matrixSonicNodeRotation[i];

        matWorld.translate(node->x, node->y, node->z);
        matrixSonicNodeTransform[i] *= matWorld;
    }
    bool parented = false;
    for (int i = 0; i < anim->nodeCount; ++i) {
        if (!parented){
            if (anim->nodeIndices[i] == 0xFE || anim->nodeIndices[i] == 0xFF){
                parented = (anim->nodeIndices[i] == 0xFE);
                continue;
            }

            memcpy(&matModel, &matrixIdentity, sizeof(matModel));
            for (int k = i; anim->nodeIndices[k] < 0xFE; ++k) {
                matModel *= matrixSonicNodeTransform[anim->nodeIndices[k]];
            }
            setPlayerVertexPositions(anim->nodeIndices[i]);

            memcpy(&matModel, &matrixIdentity, sizeof(matModel));
            for (int k = i; anim->nodeIndices[k] < 0xFE; ++k) {
                matModel *= matrixSonicNodeRotation[anim->nodeIndices[k]];
            }
            setPlayerVertexNormals(anim->nodeIndices[i]);
        }
        else {
            if (anim->nodeIndices[i] == 0xFE || anim->nodeIndices[i] == 0xFF) {
                parented = (anim->nodeIndices[i] == 0xFE);
                continue;
            }

            memcpy(&matModel, &matrixSonicNodeTransform[anim->nodeIndices[i]], sizeof(matModel));
            setPlayerVertexPositions(anim->nodeIndices[i]);

            memcpy(&matModel, &matrixSonicNodeRotation[anim->nodeIndices[i]], sizeof(matModel));
            setPlayerVertexNormals(anim->nodeIndices[i]);
        }
    }


    model.frames.clear();
    RSDKv5::Model::Frame v5f;
    for (auto &v : S3Dmodel.mdl.vertices) {
        RSDKv5::Model::Frame::Vertex v5v;
        v5v.x  = v.x;
        v5v.y  = v.y;
        v5v.z  = v.z;
        v5v.nx = v.nx;
        v5v.ny = v.ny;
        v5v.nz = v.nz;
        v5f.vertices.append(v5v);
    }
    model.frames.append(v5f);
    model.hasNormals = true;
}

void ModelViewer::loadTexture(QString texturePath)
{
    texFile = texturePath;
    reload  = true;

    repaint();
}

void ModelViewer::setFrame(int frameID)
{
    curFrame = nullptr;
    if (frameID >= 0 && frameID < model.frames.count()) {
        curFrame = &model.frames[frameID];
        if (frameID + 1 >= model.frames.count())
            nextFrame = &model.frames[loopIndex];
        else
            nextFrame = &model.frames[frameID + 1];

        reload = true;
    }

    repaint();
}

void ModelViewer::setWireframe(bool wireframe)
{
    this->wireframe = wireframe;
    repaint();
}
void ModelViewer::setNormalsVisible(bool show)
{
    showNormals = show;
    repaint();
}

void ModelViewer::setZoom(float zoom)
{
    this->zoom = zoom;

    repaint();
}

void ModelViewer::initializeGL()
{
    // Set up the rendering context, load shaders and other resources, etc.
    glFuncs = context()->functions();

    QSurfaceFormat fmt = context()->format();

    const unsigned char *vendor     = glFuncs->glGetString(GL_VENDOR);
    const unsigned char *renderer   = glFuncs->glGetString(GL_RENDERER);
    const unsigned char *version    = glFuncs->glGetString(GL_VERSION);
    const unsigned char *sdrVersion = glFuncs->glGetString(GL_SHADING_LANGUAGE_VERSION);
    const unsigned char *extensions = glFuncs->glGetString(GL_EXTENSIONS);

    QString vendorStr     = reinterpret_cast<const char *>(vendor);
    QString rendererStr   = reinterpret_cast<const char *>(renderer);
    QString versionStr    = reinterpret_cast<const char *>(version);
    QString sdrVersionStr = reinterpret_cast<const char *>(sdrVersion);
    QString extensionsStr = reinterpret_cast<const char *>(extensions);

    PrintLog("ModelViewer OpenGL Details");
    PrintLog("GL Version:   " + QString::number(fmt.majorVersion()) + "."
             + QString::number(fmt.minorVersion()));
    PrintLog("Vendor:       " + vendorStr);
    PrintLog("Renderer:     " + rendererStr);
    PrintLog("Version:      " + versionStr);
    PrintLog("GLSL version: " + sdrVersionStr);
    PrintLog("Extensions:   " + (extensionsStr == "" ? "None" : extensionsStr));
    PrintLog("GLType:       "
             + QString(QOpenGLContext::currentContext()->isOpenGLES() ? "OpenGLES" : "OpenGL"));
    PrintLog("Valid:        " + QString(QOpenGLContext::currentContext()->isValid() ? "Yes" : "No"));

    glFuncs->glEnable(GL_DEPTH_TEST);
    glFuncs->glDisable(GL_DITHER);

    glFuncs->glEnable(GL_BLEND);
    glFuncs->glDisable(GL_SCISSOR_TEST);
    glFuncs->glDisable(GL_CULL_FACE);

    glFuncs->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glFuncs->glDepthFunc(GL_LESS);
    glFuncs->glDepthRangef(0.1f, 256);

    shader.loadShader(":/shaders/3d/default.vert", QOpenGLShader::Vertex);
    shader.loadShader(":/shaders/3d/default.frag", QOpenGLShader::Fragment);
    shader.link();
    shader.use();
    shader.setValue("tex", 0);

    VAO = new QOpenGLVertexArrayObject;
    VAO->create();
    VAO->bind();

    vertVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vertVBO->create();
    vertVBO->bind();
    vertVBO->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    shader.setAttributeBuffer("in_pos", GL_FLOAT, 0, 3);
    shader.enableAttributeArray("in_pos");

    normalVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    normalVBO->create();
    normalVBO->bind();
    normalVBO->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    shader.setAttributeBuffer("in_norm", GL_FLOAT, 0, 3);
    shader.enableAttributeArray("in_norm");

    colorVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    colorVBO->create();
    colorVBO->bind();
    colorVBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
    shader.setAttributeBuffer("in_color", GL_UNSIGNED_BYTE, 0, 4);
    shader.enableAttributeArray("in_color");

    texVBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    texVBO->create();
    texVBO->bind();
    texVBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
    shader.setAttributeBuffer("in_UV", GL_FLOAT, 0, 2);
    shader.enableAttributeArray("in_UV");

    indexVBO = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    indexVBO->create();
    indexVBO->bind();
    indexVBO->setUsagePattern(QOpenGLBuffer::StaticDraw);

    resetCamera();
}

void ModelViewer::resetCamera()
{
    matModel.setToIdentity();

    zoom = 1.0f;
    camera.reset();
    camera.translate(0, 0, -128);
}

void ModelViewer::resizeGL(int w, int h)
{
    glFuncs = context()->functions();

    glFuncs->glViewport(0, 0, w, h);

    matWorld.setToIdentity();
    matWorld.perspective(45, w / (float)h, 0.1f, 256);
}

void ModelViewer::paintGL()
{
    glFuncs = context()->functions();
    QColor c = QApplication::palette().dark().color();
    glFuncs->glClearColor(c.redF(), c.greenF(), c.blueF(), 1.0f);
    glFuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!curFrame || !curFrame->vertices.count())
        return;

    shader.use();

    if (reload) {
        reload      = false;
        int vc      = curFrame->vertices.count();
        vertBuf     = new float[vc * 3];
        normBuf     = new float[vc * 3];
        auto colors = new RSDKv5::Model::Color[vc];
        auto uvs    = new RSDKv5::Model::TexCoord[vc];

        int i = 0;
        for (auto &c : model.colors) colors[i++] = c;

        i = 0;
        for (auto &uv : model.texCoords) uvs[i++] = uv;

        // 0 3 5 turned to 3:   0 3 5
        // 0 3 5 4 turned to 3: 0 3 5 5 4 0
        // etc
        int count       = 3 * model.faceVerticesCount - 3;
        int total       = count * (model.indices.count() / model.faceVerticesCount);
        auto indices    = new ushort[total];
        ushort *current = indices;

        i = 0;
        for (; i < model.indices.count() - 1; i += model.faceVerticesCount) {
            for (int j = 0; j < model.faceVerticesCount - 1; ++j) {
                current[j * 3 + 0] = model.indices[i + j];
                current[j * 3 + 1] = model.indices[i + j + 1];
                if (j + 1 < model.faceVerticesCount - 1)
                    current[j * 3 + 2] = model.indices[i + j + 2];
            }
            // if (model.faceVerticesCount != 3)
            current[count - 1] = model.indices[i];
            current            = &current[count];
        }

        vertVBO->bind();
        vertVBO->allocate(vc * sizeof(float) * 3);
        normalVBO->bind();
        normalVBO->allocate(vc * sizeof(float) * 3);
        colorVBO->bind();
        colorVBO->allocate(colors, vc * sizeof(RSDKv5::Model::Color));
        texVBO->bind();
        texVBO->allocate(uvs, vc * sizeof(RSDKv5::Model::TexCoord));
        indexVBO->bind();
        indexVBO->allocate(indices, total * sizeof(ushort));

        shader.setValue("useColor", model.hasColors);
        shader.setValue("useTextures", model.hasTextures);
        shader.setValue("useNormals", model.hasNormals);

        delete[] indices;
        delete[] colors;
        delete[] uvs;
    }

    if (texFile != curTex) {
        curTex = texFile;
        delete tex;
        QImage src(curTex);
        tex = new QOpenGLTexture(QOpenGLTexture::Target2D);
        tex->create();
        tex->bind();
        tex->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::WrapMode::Repeat);
        tex->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::WrapMode::Repeat);
        tex->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
        tex->setFormat(QOpenGLTexture::RGBA8_UNorm);
        tex->setSize(src.width(), src.height());
        tex->setData(src, QOpenGLTexture::GenerateMipMaps);
        glFuncs->glActiveTexture(GL_TEXTURE0);
    }

    // handle interpolation, set vertVBO properly using vertVBO->write

    for (int i = 0; i < curFrame->vertices.count(); ++i) {
        auto &cv           = curFrame->vertices.at(i);
        auto &nv           = nextFrame->vertices.at(i);
        float interp       = 1 - animTimer;
        float interp2      = animTimer;
        vertBuf[i * 3 + 0] = cv.x * interp + nv.x * interp2;
        vertBuf[i * 3 + 1] = cv.y * interp + nv.y * interp2;
        vertBuf[i * 3 + 2] = cv.z * interp + nv.z * interp2;
        normBuf[i * 3 + 0] = cv.nx * interp + nv.nx * interp2;
        normBuf[i * 3 + 1] = cv.ny * interp + nv.ny * interp2;
        normBuf[i * 3 + 2] = cv.nz * interp + nv.nz * interp2;
    }
    vertVBO->bind();
    vertVBO->write(0, vertBuf, curFrame->vertices.count() * sizeof(float) * 3);
    normalVBO->bind();
    normalVBO->write(0, normBuf, curFrame->vertices.count() * sizeof(float) * 3);

    matModel.setToIdentity();
    matModel.scale(zoom, zoom, zoom);

    shader.setValue("default_color", QVector4D(modelColor.redF(), modelColor.greenF(),
                                               modelColor.blueF(), modelColor.alphaF()));

    shader.setValue("projection", matWorld);
    shader.setValue("view", camera.toMatrix());
    shader.setValue("model", matModel);

    glFuncs->glDrawElements(wireframe ? GL_LINES : GL_TRIANGLES, indexVBO->size() / sizeof(ushort),
                            GL_UNSIGNED_SHORT, 0);
}

#include "moc_modelviewer.cpp"
