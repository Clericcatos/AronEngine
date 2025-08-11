#pragma once
#include <string>

// Force recompile

namespace AronEngine 
{
    namespace Korean 
    {
        // 메뉴
        extern const char* MENU_FILE;
        extern const char* MENU_EDIT;
        extern const char* MENU_GAMEOBJECT;
        extern const char* MENU_WINDOW;
        extern const char* MENU_HELP;
        
        // 파일 메뉴
        extern const char* FILE_NEW_SCENE;
        extern const char* FILE_OPEN_SCENE;
        extern const char* FILE_SAVE_SCENE;
        extern const char* FILE_EXIT;
        
        // 편집 메뉴
        extern const char* EDIT_UNDO;
        extern const char* EDIT_REDO;
        extern const char* EDIT_CUT;
        extern const char* EDIT_COPY;
        extern const char* EDIT_PASTE;
        
        // 게임오브젝트 메뉴
        extern const char* GO_CREATE_EMPTY;
        extern const char* GO_2D_OBJECT;
        extern const char* GO_SPRITE;
        extern const char* GO_SQUARE;
        extern const char* GO_CIRCLE;
        
        // 창 메뉴
        extern const char* WIN_SCENE_HIERARCHY;
        extern const char* WIN_INSPECTOR;
        extern const char* WIN_CONSOLE;
        extern const char* WIN_ASSET_BROWSER;
        extern const char* WIN_SCENE_VIEW;
        extern const char* WIN_GAME_VIEW;
        extern const char* WIN_DEMO;
        
        // 도움말
        extern const char* HELP_ABOUT;
        
        // 인스펙터
        extern const char* INSPECTOR_GAMEOBJECT;
        extern const char* INSPECTOR_ACTIVE;
        extern const char* INSPECTOR_TRANSFORM;
        extern const char* INSPECTOR_POSITION;
        extern const char* INSPECTOR_ROTATION;
        extern const char* INSPECTOR_SCALE;
        extern const char* INSPECTOR_SPRITE_RENDERER;
        extern const char* INSPECTOR_COLOR;
        extern const char* INSPECTOR_SIZE;
        extern const char* INSPECTOR_TEXTURE_PATH;
        extern const char* INSPECTOR_LOAD_TEXTURE;
        extern const char* INSPECTOR_CLEAR_TEXTURE;
        extern const char* INSPECTOR_DEFAULT_TEXTURES;
        
        // 게임 컨트롤
        extern const char* GAME_CONTROLS_DEBUG;
        extern const char* GAME_CONTROLS_EXIT;
        extern const char* GAME_CONTROLS_MOVEMENT;
        extern const char* GAME_CONTROLS_JUMP;
        extern const char* GAME_CONTROLS_EDITOR;
        extern const char* GAME_CONTROLS_AUDIO;
        extern const char* GAME_CONTROLS_BEEP;
        
        // 상태 메시지
        extern const char* STATUS_AUDIO_READY;
        extern const char* STATUS_AUDIO_ALT;
        extern const char* STATUS_EDITOR_ON;
        extern const char* STATUS_EDITOR_OFF;
        extern const char* STATUS_PLAYER;
        extern const char* STATUS_VELOCITY;
        extern const char* STATUS_JUMPING;
        extern const char* STATUS_ON_GROUND;
        extern const char* STATUS_MOUSE;
    }
}