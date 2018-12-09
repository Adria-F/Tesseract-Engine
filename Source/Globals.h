#ifndef __GLOBALS_H__
#define __GLOBALS_H__

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>

typedef unsigned int uint;

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

// Deletes a buffer
#define RELEASE( x ) \
    {                        \
    if( x != NULL )        \
	    {                      \
      delete x;            \
	  x = NULL;              \
	    }                      \
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
    {                              \
    if( x != NULL )              \
	    {                            \
      delete[] x;                \
	  x = NULL;                    \
	    }                            \
                              \
}

#define START_RNG_SEED() startRNGSeed();
void startRNGSeed();

//Float between 0 and 1
#define GET_RANDOM() getRandom();
float getRandom();

#define GET_RANDOM_BETWEEN(min, max) getRandomBetween(min, max);
int getRandomBetween(int min, int max);

#define GENERATE_UID() generateUID()
uint generateUID();

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI 3.14159265358979323846


enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1580
#define SCREEN_HEIGHT 950
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC false
#define TITLE "3D Engine"

// Files PATHS
#define PATHS_AMOUNT 8
#define ASSETS_FOLDER "Assets/"
#define LIBRARY_FOLDER "Library/"
#define MESHES_FOLDER "Library/Meshes/"
#define ANIMATIONS_FOLDER "Library/Animations/"
#define BONES_FOLDER "Library/Animations/Bones/"
#define TEXTURES_FOLDER "Library/Textures/"
#define FBX_FOLDER "Library/Scenes/"
#define SETTINGS_FOLDER "Settings/"
#define MATERIALS_FOLDER "Materials/"

// Own format files
#define MESH_EXTENSION ".tesseractMesh"
#define ANIMATION_EXTENSION ".tesseractAnimation"
#define BONE_EXTENSION ".tesseractBone"
#define TEXTURES_EXTENSION ".dds"
#define SCENES_EXTENSION ".tesseractScene"
#define MATERIAL_EXTENSION ".tesseractMat"
#define META_EXTENSION ".meta"

#endif // !__GLOBALS_H__
