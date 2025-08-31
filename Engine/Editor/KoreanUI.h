#pragma once
#include <string>

namespace AronEngine
{
    namespace UI
    {
        // Menu Bar
        constexpr const wchar_t* MENU_FILE = L"File";
        constexpr const wchar_t* MENU_EDIT = L"Edit";
        constexpr const wchar_t* MENU_ASSETS = L"Assets";
        constexpr const wchar_t* MENU_GAMEOBJECT = L"GameObject";
        constexpr const wchar_t* MENU_COMPONENT = L"Component";
        constexpr const wchar_t* MENU_WINDOW = L"Window";
        constexpr const wchar_t* MENU_HELP = L"Help";
        
        // File Menu
        constexpr const wchar_t* FILE_NEW_SCENE = L"New Scene";
        constexpr const wchar_t* FILE_OPEN_SCENE = L"Open Scene...";
        constexpr const wchar_t* FILE_SAVE = L"Save";
        constexpr const wchar_t* FILE_SAVE_AS = L"Save As...";
        constexpr const wchar_t* FILE_BUILD_SETTINGS = L"Build Settings...";
        constexpr const wchar_t* FILE_BUILD_RUN = L"Build and Run";
        constexpr const wchar_t* FILE_EXIT = L"Exit";
        
        // Edit Menu
        constexpr const wchar_t* EDIT_UNDO = L"Undo";
        constexpr const wchar_t* EDIT_REDO = L"Redo";
        constexpr const wchar_t* EDIT_CUT = L"Cut";
        constexpr const wchar_t* EDIT_COPY = L"Copy";
        constexpr const wchar_t* EDIT_PASTE = L"Paste";
        constexpr const wchar_t* EDIT_DUPLICATE = L"Duplicate";
        constexpr const wchar_t* EDIT_DELETE = L"Delete";
        constexpr const wchar_t* EDIT_SELECT_ALL = L"Select All";
        
        // GameObject Menu
        constexpr const wchar_t* GO_CREATE_EMPTY = L"Create Empty";
        constexpr const wchar_t* GO_CREATE_CHILD = L"Create Empty Child";
        constexpr const wchar_t* GO_2D_OBJECT = L"2D Object";
        constexpr const wchar_t* GO_3D_OBJECT = L"3D Object";
        constexpr const wchar_t* GO_SPRITE = L"Sprite";
        constexpr const wchar_t* GO_CAMERA = L"Camera";
        constexpr const wchar_t* GO_AUDIO_SOURCE = L"Audio Source";
        
        // Panel Names
        constexpr const wchar_t* PANEL_HIERARCHY = L"Hierarchy";
        constexpr const wchar_t* PANEL_SCENE = L"Scene";
        constexpr const wchar_t* PANEL_GAME = L"Game";
        constexpr const wchar_t* PANEL_INSPECTOR = L"Inspector";
        constexpr const wchar_t* PANEL_PROJECT = L"Project";
        constexpr const wchar_t* PANEL_CONSOLE = L"Console";
        
        // Inspector
        constexpr const wchar_t* INSPECTOR_TRANSFORM = L"Transform";
        constexpr const wchar_t* INSPECTOR_POSITION = L"Position";
        constexpr const wchar_t* INSPECTOR_ROTATION = L"Rotation";
        constexpr const wchar_t* INSPECTOR_SCALE = L"Scale";
        constexpr const wchar_t* INSPECTOR_ADD_COMPONENT = L"Add Component";
        constexpr const wchar_t* INSPECTOR_SPRITE_RENDERER = L"Sprite Renderer";
        constexpr const wchar_t* INSPECTOR_COLOR = L"Color";
        constexpr const wchar_t* INSPECTOR_SPRITE = L"Sprite";
        
        // Tools
        constexpr const wchar_t* TOOL_HAND = L"Hand";
        constexpr const wchar_t* TOOL_MOVE = L"Move";
        constexpr const wchar_t* TOOL_ROTATE = L"Rotate";
        constexpr const wchar_t* TOOL_SCALE = L"Scale";
        constexpr const wchar_t* TOOL_RECT = L"Rect";
        
        // Other UI Text
        constexpr const wchar_t* UI_SEARCH = L"Search...";
        constexpr const wchar_t* UI_NOTHING_SELECTED = L"Nothing Selected";
        constexpr const wchar_t* UI_NO_OBJECTS = L"No objects in scene";
        constexpr const wchar_t* UI_RIGHT_CLICK_CREATE = L"Right-click to create";
        constexpr const wchar_t* UI_DRAG_ASSETS = L"Drag assets here";
        constexpr const wchar_t* UI_RENAME_TIP = L"Rename (F2)";
        constexpr const wchar_t* UI_DELETE_TIP = L"Delete (Del)";
        constexpr const wchar_t* UI_DUPLICATE_TIP = L"Duplicate (Ctrl+D)";
    }
}