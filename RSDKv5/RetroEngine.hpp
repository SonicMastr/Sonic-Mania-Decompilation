#ifndef RETROENGINE_H
#define RETROENGINE_H

// ================
// STANDARD LIBS
// ================
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <ctime>

// ================
// STANDARD TYPES
// ================
typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned int bool32;

enum GamePlatforms {
    PLATFORM_WIN    = 0,
    PLATFORM_PS4    = 1,
    PLATFORM_XB1    = 2,
    PLATFORM_SWITCH = 3,
    PLATFORM_DEV    = 0xFF,
};

enum GameLanguages {
    LANGUAGE_EN,
    LANGUAGE_FR,
    LANGUAGE_IT,
    LANGUAGE_GE,
    LANGUAGE_SP,
    LANGUAGE_JP,
    LANGUAGE_KO,
    LANGUAGE_SC,
    LANGUAGE_TC,
};

enum GameRegions {
    REGION_US,
    REGION_JP,
    REGION_EU,
};

#define RETRO_USE_ORIGINAL_CODE (0)

#define RETRO_WIN     (0)
#define RETRO_PS4     (1)
#define RETRO_XB1     (2)
#define RETRO_SWITCH  (3)
#define RETRO_OSX     (4)
#define RETRO_LINUX   (5)
#define RETRO_iOS     (6)
#define RETRO_ANDROID (7)
#define RETRO_UWP     (8)

#if defined _WIN32

#if defined WINAPI_FAMILY
#if WINAPI_FAMILY != WINAPI_FAMILY_APP
#define RETRO_PLATFORM   (RETRO_WIN)
#else
#define RETRO_PLATFORM   (RETRO_UWP)
#endif
#else
#define RETRO_PLATFORM   (RETRO_WIN)
#endif

#elif defined __APPLE__
#if __IPHONEOS__
#define RETRO_PLATFORM   (RETRO_iOS)
#else
#define RETRO_PLATFORM   (RETRO_OSX)
#endif
#else
#define RETRO_PLATFORM   (RETRO_WIN)
#endif

#define SCREEN_XMAX (1280)
#define SCREEN_YSIZE (240)

#define BASE_PATH ""

#define RETRO_USING_SDL2      (0) // general
#define RETRO_USING_DIRECTX9  (0) // windows
#define RETRO_USING_DIRECTX11 (0) // xbox one

#if RETRO_PLATFORM == RETRO_WIN || RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_LINUX
#undef RETRO_USING_SDL2
#define RETRO_USING_SDL2 (1)

#else // Since its an else & not an elif these platforms probably aren't supported yet
#define RETRO_USING_SDL2 (0)
#endif

#if RETRO_PLATFORM == RETRO_WIN || RETRO_PLATFORM == RETRO_UWP
#undef RETRO_USING_DIRECTX9
#define RETRO_USING_DIRECTX9 (1)

#if RETRO_USING_SDL2
#include <SDL.h>
#endif
#include <vorbis/vorbisfile.h>
#endif

#if RETRO_PLATFORM == RETRO_XB1
#undef RETRO_USING_DIRECTX11
#define RETRO_USING_DIRECTX11 (1)
#endif

#if RETRO_PLATFORM == RETRO_OSX
#include <SDL2/SDL.h>
//#include <vorbis/vorbisfile.h>
#endif


#define RETRO_REV02 (1)

enum EngineStates {
    ENGINESTATE_LOAD,
    ENGINESTATE_REGULAR,
    ENGINESTATE_PAUSED,
    ENGINESTATE_FROZEN,
    ENGINESTATE_STEPOVER      = 4,
    ENGINESTATE_DEVMENU       = 8,
    ENGINESTATE_VIDEOPLAYBACK = 9,
    ENGINESTATE_SHOWPNG,
#if RETRO_REV02
    ENGINESTATE_ERRORMSG,
    ENGINESTATE_ERRORMSG_FATAL,
#endif
    ENGINESTATE_NULL,
};

// Utils
#include "iniparser/iniparser.h"

enum SeverityModes {
    SEVERITY_NONE,
    SEVERITY_WARN,
    SEVERITY_ERROR,
    SEVERITY_FATAL,
};

#include "Storage.hpp"
#include "Math.hpp"
#include "Text.hpp"
#include "Reader.hpp"
#include "Animation.hpp"
#include "Audio.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "Palette.hpp"
#include "Drawing.hpp"
#include "Shader.hpp"
#include "Scene3D.hpp"
#include "Collision.hpp"
#include "Scene.hpp"
#include "Sprite.hpp"
#include "Userdata.hpp"
#include "Debug.hpp"
#include "Link.hpp"

#include "DefaultObject.hpp"
#if RETRO_REV02
#include "DevOutput.hpp"
#endif
#include "TestObject.hpp"

struct RetroEngine {
    RetroEngine() {}
    char gameName[0x80];
    char gameSubName[0x80];
    char gameVersion[0x20];

    bool32 useExternalCode = true;

    bool32 devMenu    = false;
    bool32 printConsole = false;

    bool32 hasPlus     = true;  // are sonic mania plus features enabled?
    bool32 confirmFlip = false; // swaps A/B, used for nintendo and etc controllers

    int prevEngineMode      = ENGINESTATE_LOAD;
    bool32 running          = false;
    int gameSpeed           = 1;
    int fastForwardSpeed    = 8;
    bool32 frameStep        = false;
    bool32 showPaletteOverlay = false;
    bool32 drawLayerVisible[DRAWLAYER_COUNT];

    bool32 isFullScreen = false;

    bool32 windowActive    = false;
    bool32 startFullScreen = false;
    bool32 borderless      = false;
    bool32 exclusiveFS     = false;
    bool32 vsync           = false;
    bool32 tripleBuffer    = false;
    int windowWidth        = 424;
    int windowHeight       = SCREEN_YSIZE;
    int fsWidth            = 0;
    int fsHeight           = 0;
    int gameHeight         = SCREEN_YSIZE;
    int refreshRate        = 60;

    //Image/Video support
    float displayTime;
    float imageDelta;
    bool32 (*skipCallback)();

    bool32 shaderSupport = true;
    int shaderID         = 0;
    int prevShaderID         = 0;
    int screenCount      = 1;
    int dimTimer         = 0;
    int dimLimit         = 0;
    float dimPercent     = 1.0;
    float dimMax         = 1.0;

    bool32 streamsEnabled = true;
    float streamVolume  = 1.0;
    float soundFXVolume = 1.0;

#if RETRO_USING_SDL2
    SDL_Window *window     = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *screenBuffer[SCREEN_MAX];

    SDL_Texture *imageTexture = nullptr;

    SDL_Event sdlEvents;
#endif
};

extern RetroEngine engine;

bool initRetroEngine();
void runRetroEngine();

void parseArguments(int argc, char *argv[]);

bool startGameObjects();

bool LoadGameConfig();
void InitScriptSystem();

inline void SetEngineState(byte state)
{
    bool32 stepOver = (sceneInfo.state & ENGINESTATE_STEPOVER) == ENGINESTATE_STEPOVER;
    sceneInfo.state = state;
    if (stepOver)
        sceneInfo.state |= ENGINESTATE_STEPOVER;
}

extern byte *gameOptionsPtr;

inline void InitGameOptions(void **options, int size)
{
    AllocateStorage(size, options, DATASET_STG, true);
    gameOptionsPtr = (byte *)*options;
}

#if RETRO_USING_DIRECTX9

#endif

#if RETRO_USING_DIRECTX11

#endif

#include "Link.hpp"

#endif //! RETROENGINE_H
