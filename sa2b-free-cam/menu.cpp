#include "pch.h"
#include "SA2ModLoader.h"
#include "Trampoline.h"
#include "utilities.h"
#include "camera.h"

Trampoline* GameStateHandler_t = nullptr;
Trampoline* InitPauseMenu_t = nullptr;
Trampoline* FreePauseMenu_t = nullptr;

static const char* const CameraModeTexts[] {
    "CAMERA MODE", // jp
    "CAMERA MODE", // en
    "MODE DE LA CAM�RA", // fr
    "MODO DE LA C�MARA", // es
    "CAMERA MODE", // ge
    "CAMERA MODE"  // it
};

static const char* const AutoCameraTexts[] {
    "AUTO CAMERA", // jp
    "AUTO CAMERA", // en
    "CAM�RA AUTO", // fr
    "C�MARA AUTOM�TICA", // es
    "AUTO CAMERA", // ge
    "AUTO CAMERA"  // it
};

static const char* const FreeCameraTexts[] {
    "FREE CAMERA", // jp
    "FREE CAMERA", // en
    "CAM�RA LIBRE", // fr
    "C�MARA LIBRE", // es
    "FREE CAMERA", // ge
    "FREE CAMERA"  // it
};

static const char* const* const CameraMenuTexts[] {
    AutoCameraTexts,
    FreeCameraTexts
};

static void* CameraMenuFontTextures[] { nullptr, nullptr };

// Rewrite the pause menu display to add the camera submenu display
static void __cdecl PauseMenuDisplay_r()
{
    ScreenDrawMode = 0;
    njDisableFog();
    
    RenferInfo_->unknown1 = RenferInfo_->unknown1 & 0x3FFFFFF | 0x94000000;
    if (*(int*)0x1A558B8 == 1 || *(int*)0x1A558BC == 1)
    {
        if (*(int*)0x1A558BC)
        {
            DrawPauseMassage(MenuButtons_SomethingWasPressed, *(void**)0x1A558F0);
        }
        if (*(int*)0x1A558B8)
        {
            DrawPauseMassage(MenuButtons_SomethingWasPressed, *(void**)0x1A558EC);
        }
    }
    else
    {
        if (PauseMode == 1)
        {
            DrawPauseBackground(PauseOptionCount);
            for (int i = 0; i < PauseOptionCount; ++i)
            {
                void* v3;
                if (i >= 1 && PauseOptionCount == 5)
                {
                    v3 = PauseMenuMainFontTexs[i + 1];
                }
                else
                {
                    v3 = PauseMenuMainFontTexs[i];
                }
                DrawPauseText(v3, i, i);
            }
        }
        else if (PauseMode == 2)              // Help menu
        {
            DrawPauseBackground(PauseOptionCount);
            for (int j = 0; j < PauseOptionCount; ++j)
            {
                DrawPauseText(PauseMenuHelpFontTexs[j], j, j);
            }
        }
        else if (PauseMode == 3)
        {
            DrawSpriteThing(*(void**)0x1A55928, 168.0f, 180.0f, 320.0f, 164.0f, 0.99f, 0.0f, 0.0f, 1.0f, 1.0f, -1);
            for (int i = 0; i < PauseOptionCount; ++i)
            {
                DrawPauseText(CameraMenuFontTextures[i], i, i);
            }
        }
    }

    ScreenDrawMode = 1;
    njEnableFog();
}

static void ReturnToMainPauseMenu()
{
    PlaySoundProbably(32770, 0, 0, 0);
    PauseMode = 1;
    PauseOptionCount = (!TwoPlayerMode && Life_Count[0] > 0) ? 6 : 5;
    PauseSelection = PauseOptionCount == 6 ? 2 : 1;
    ControllersRaw[0].press = 0; // prevent original function from running wrong code
}

static void GoToCameraMenu()
{
    PlaySoundProbably(32770, 0, 0, 0);
    PauseOptionCount = 2;
    PauseMode = 3;
    ControllersRaw[0].press = 0; // prevent original function from running wrong code
    PauseSelection = GetFreeCamera(0);
}

// Hook the gamestate handler to add a submenu to the pause menu
static BOOL __cdecl GameStateHandler_r()
{
    if (GameState == GameStates_Pause)
    {
        // Main menu logic override:
        if (PauseMode == 1 && ControllersRaw[0].press & Buttons_A)
        {
            if ((PauseSelection == 2 && PauseOptionCount == 6) || (PauseSelection == 1 && PauseOptionCount == 5))
            {
                GoToCameraMenu();
            }
        }

        // Camera menu logic:
        if (PauseMode == 3)
        {
            if (ControllersRaw[0].press & Buttons_B)
            {
                ReturnToMainPauseMenu();
            }

            if (ControllersRaw[0].press & (Buttons_Start | Buttons_A))
            {
                SetFreeCamera(PauseSelection, 0);
                ReturnToMainPauseMenu();
            }
        }
    }

    return TARGET_DYNAMIC(GameStateHandler)();
}

static BOOL __cdecl InitPauseMenu_r()
{
    for (int i = 0; i < LengthOfArray(CameraMenuFontTextures); ++i)
    {
        CameraMenuFontTextures[i] = CreateFontTexture(CameraMenuTexts[i][TextLanguage], TextLanguage, -1, -1);
    }

    return TARGET_DYNAMIC(InitPauseMenu)();
}

static BOOL __cdecl FreePauseMenu_r()
{
    for (int i = 0; i < LengthOfArray(CameraMenuFontTextures); ++i)
    {
        FreeFontTexture(&CameraMenuFontTextures[i]);
    }

    return TARGET_DYNAMIC(FreePauseMenu)();
}

void InitMenu()
{   
    PauseMenuMainTextsJP[2] = CameraModeTexts[Language_Japanese];
    PauseMenuMainTextsUS[2] = CameraModeTexts[Language_English];
    PauseMenuMainTextsFR[2] = CameraModeTexts[Language_French];
    PauseMenuMainTextsES[2] = CameraModeTexts[Language_Spanish];
    PauseMenuMainTextsGE[2] = CameraModeTexts[Language_German];
    PauseMenuMainTextsIT[2] = CameraModeTexts[Language_Italian];

    WriteJump((void*)0x440AD0, PauseMenuDisplay_r);

    GameStateHandler_t = new Trampoline(0x43A780, 0x43A786, GameStateHandler_r);
    InitPauseMenu_t = new Trampoline(0x439610, 0x439616, InitPauseMenu_r);
    FreePauseMenu_t = new Trampoline(0x439C80, 0x439C85, FreePauseMenu_r);
}
