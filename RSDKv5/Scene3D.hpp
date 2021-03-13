#ifndef SCENE3D_H
#define SCENE3D_H

#define SCENE3D_MAX (0x20)
#define MODEL_MAX   (0x100)

enum Scene3DDrawTypes {
    S3D_TYPE_WORLD_WIREFRAME = 0x0,
    S3D_TYPE_WORLD         = 0x1,
    S3D_TYPE_2         = 0x2,
    S3D_TYPE_3         = 0x3,
    S3D_TYPE_4         = 0x4,
    S3D_TYPE_5         = 0x5,
    S3D_TYPE_6         = 0x6,
    S3D_TYPE_7         = 0x7,
    S3D_TYPE_8         = 0x8,
    S3D_TYPE_9         = 0x9,
    S3D_TYPE_A         = 0xA,
    S3D_TYPE_B         = 0xB,
};

struct ScanEdge {
    int start;
    int end;
    int start1;
    int end1;
    int start2;
    int end2;
    int start3;
    int end3;
};

struct Matrix {
    int values[4][4];
};

struct ModelVertex {
    int x;
    int y;
    int z;
    int nx;
    int ny;
    int nz;
};

struct TexCoord {
    int x;
    int y;
};

struct Model {
    uint hash[4];
    ModelVertex *vertices;
    TexCoord *texCoords;
    Colour *colours;
    ushort *indices;
    ushort vertCount;
    ushort indexCount;
    ushort frameCount;
    byte flags;
    byte faceVertCount;
    byte scope;
};

struct Scene3DVertex {
    int x;
    int y;
    int z;
    int nx;
    int ny;
    int nz;
    int tx;
    int ty;
    uint colour;
};

struct ZBufferEntry {
    int depth;
    int index;
};

struct Scene3D {
    uint hash[4];
    Scene3DVertex *vertices;
    int field_14;
    ZBufferEntry *zBuffer;
    byte *faceVertCounts;
    int unknown1;
    int unknown2;
    int unknown3;
    int unknown4;
    int unknown5;
    int unknown6;
    int unknown7;
    int unknown8;
    int unknown9;
    int unknown10;
    int unknown11;
    ushort field_4C;
    ushort indexCount;
    ushort indCnt;
    byte drawMode;
    byte scope;
};

extern Model modelList[MODEL_MAX];
extern Scene3D scene3DList[SCENE3D_MAX];

extern ScanEdge scanEdgeBuffer[SCREEN_YSIZE];

void ProcessScanEdge(int x1, int y1, int x2, int y2);
void ProcessScanEdgeUV(int u, int v, int x1, int y1, int x2, int y2);

void setIdentityMatrix(Matrix *matrix);
void matrixMultiply(Matrix *dest, Matrix *matrixA, Matrix *matrixB);
void matrixTranslateXYZ(Matrix *Matrix, int x, int y, int z, bool32 setIdentity);
void matrixScaleXYZ(Matrix *matrix, int scaleX, int scaleY, int scaleZ);
void matrixRotateX(Matrix *matrix, short angle);
void matrixRotateY(Matrix *matrix, short angle);
void matrixRotateZ(Matrix *matrix, short angle);
void matrixRotateXYZ(Matrix *matrix, short rotationX, short rotationY, short rotationZ);
void matrixInverse(Matrix *dest, Matrix *matrix);
void matrixCopy(Matrix *matDst, Matrix* matSrc);

ushort LoadMesh(const char *filepath, Scopes scope);
ushort Create3DScene(const char *name, ushort faceCnt, Scopes scope);
inline void Init3DScene(ushort sceneID)
{
    if (sceneID < SCENE3D_MAX) {
        Scene3D *scn  = &scene3DList[sceneID];
        scn->indexCount = 0;
    }
}

inline void SetModelAnimation(ushort model, EntityAnimationData *data, short animSpeed, byte loopIndex, bool32 forceApply, ushort frameID)
{
    if (model >= MODEL_MAX) {
        if (data)
            data->framePtrs = 0;
        return;
    }
    if (!data)
        return;

    if (data->animationID == model && !forceApply)
        return;
    data->framePtrs       = (SpriteFrame *)1;
    data->animationTimer  = 0;
    data->frameID         = frameID;
    data->frameCount      = modelList[model].frameCount;
    data->animationSpeed  = animSpeed;
    data->prevAnimationID = data->animationID;
    data->frameDelay      = 0x100;
    data->loopIndex       = loopIndex;
    data->animationID     = model;
}
inline void View_Something1(ushort sceneID, byte x, byte y, byte z)
{
    if (sceneID < SCENE3D_MAX) {
        Scene3D *scn  = &scene3DList[sceneID];
        scn->unknown3 = x;
        scn->unknown4 = y;
        scn->unknown5 = z;
    }
}
inline void View_Something2(ushort sceneID, byte x, byte y, byte z)
{
    if (sceneID < SCENE3D_MAX) {
        Scene3D *scn  = &scene3DList[sceneID];
        scn->unknown6 = x;
        scn->unknown7 = y;
        scn->unknown8 = z;
    }
}
inline void View_Something3(ushort sceneID, byte x, byte y, byte z)
{
    if (sceneID < SCENE3D_MAX) {
        Scene3D *scn   = &scene3DList[sceneID];
        scn->unknown9  = x;
        scn->unknown10 = y;
        scn->unknown11 = z;
    }
}
void SetupMesh(ushort animID, ushort sceneID, byte drawMode, Matrix *matWorld, Matrix *matView, uint colour);
void SetupMeshAnimation(ushort animID, ushort sceneID, EntityAnimationData *data, byte drawMode, Matrix *matWorld, Matrix *matView, uint colour);
void Draw3DScene(ushort sceneID);

#endif