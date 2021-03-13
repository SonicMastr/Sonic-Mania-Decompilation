#ifndef OBJ_TITLEEGGMAN_H
#define OBJ_TITLEEGGMAN_H

#include "../SonicMania.h"

#if RETRO_USE_PLUS
// Object Class
typedef struct {
	RSDK_OBJECT
} ObjectTitleEggman;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityTitleEggman;

// Object Struct
extern ObjectTitleEggman *TitleEggman;

// Standard Entity Events
void TitleEggman_Update();
void TitleEggman_LateUpdate();
void TitleEggman_StaticUpdate();
void TitleEggman_Draw();
void TitleEggman_Create(void* data);
void TitleEggman_StageLoad();
void TitleEggman_EditorDraw();
void TitleEggman_EditorLoad();
void TitleEggman_Serialize();

// Extra Entity Functions
#endif

#endif //!OBJ_TITLEEGGMAN_H