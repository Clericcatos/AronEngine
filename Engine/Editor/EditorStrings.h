#pragma once

namespace AronEngine
{
    namespace EditorStrings
    {
        // Menu Bar
        constexpr const char* MENU_FILE = u8"파일";
        constexpr const char* MENU_EDIT = u8"편집";
        constexpr const char* MENU_GAMEOBJECT = u8"게임오브젝트";
        constexpr const char* MENU_WINDOW = u8"창";
        constexpr const char* MENU_HELP = u8"도움말";
        
        // File Menu
        constexpr const char* FILE_NEW_SCENE = u8"새 씬";
        constexpr const char* FILE_OPEN_SCENE = u8"씬 열기";
        constexpr const char* FILE_SAVE_SCENE = u8"씬 저장";
        constexpr const char* FILE_EXIT = u8"종료";
        
        // Edit Menu
        constexpr const char* EDIT_UNDO = u8"실행 취소";
        constexpr const char* EDIT_REDO = u8"다시 실행";
        constexpr const char* EDIT_CUT = u8"잘라내기";
        constexpr const char* EDIT_COPY = u8"복사";
        constexpr const char* EDIT_PASTE = u8"붙여넣기";
        
        // GameObject Menu
        constexpr const char* GO_CREATE_EMPTY = u8"빈 오브젝트 생성";
        constexpr const char* GO_2D_OBJECT = u8"2D 오브젝트";
        constexpr const char* GO_SPRITE = u8"스프라이트";
        constexpr const char* GO_SQUARE = u8"사각형";
        constexpr const char* GO_CIRCLE = u8"원";
        
        // Window Menu
        constexpr const char* WIN_SCENE_HIERARCHY = u8"씬 계층구조";
        constexpr const char* WIN_INSPECTOR = u8"인스펙터";
        constexpr const char* WIN_CONSOLE = u8"콘솔";
        constexpr const char* WIN_ASSET_BROWSER = u8"에셋 브라우저";
        constexpr const char* WIN_SCENE_VIEW = u8"씬 뷰";
        constexpr const char* WIN_GAME_VIEW = u8"게임 뷰";
        constexpr const char* WIN_DEMO = u8"데모 창";
        
        // Help Menu
        constexpr const char* HELP_ABOUT = u8"AronEngine 정보";
        
        // Window Titles
        constexpr const char* TITLE_SCENE_HIERARCHY = u8"씬 계층구조";
        constexpr const char* TITLE_INSPECTOR = u8"인스펙터";
        constexpr const char* TITLE_CONSOLE = u8"콘솔";
        constexpr const char* TITLE_ASSET_BROWSER = u8"에셋 브라우저";
        constexpr const char* TITLE_SCENE_VIEW = u8"씬 뷰";
        constexpr const char* TITLE_GAME_VIEW = u8"게임 뷰";
        
        // Scene Hierarchy
        constexpr const char* SCENE_PREFIX = u8"씬: ";
        constexpr const char* GAME_OBJECTS = u8"게임 오브젝트:";
        constexpr const char* NO_SCENE_LOADED = u8"로드된 씬이 없습니다";
        constexpr const char* DELETE = u8"삭제";
        constexpr const char* CREATE_SPRITE = u8"스프라이트 생성";
        constexpr const char* CREATE_CAMERA = u8"카메라 생성";
        
        // Inspector
        constexpr const char* NO_OBJECT_SELECTED = u8"선택된 오브젝트가 없습니다";
        constexpr const char* SELECT_OBJECT_HINT = u8"씬 계층구조에서 오브젝트를 선택하여 속성을 확인하세요.";
        constexpr const char* GAMEOBJECT_PREFIX = u8"게임오브젝트: ";
        constexpr const char* ACTIVE = u8"활성화";
        constexpr const char* TRANSFORM = u8"트랜스폼";
        constexpr const char* POSITION = u8"위치";
        constexpr const char* ROTATION = u8"회전";
        constexpr const char* SCALE = u8"크기";
        constexpr const char* SPRITE_RENDERER = u8"스프라이트 렌더러";
        constexpr const char* COLOR = u8"색상";
        constexpr const char* SIZE = u8"크기";
        
        // Console
        constexpr const char* CONSOLE_TITLE = u8"AronEngine 콘솔";
        constexpr const char* EXECUTE = u8"실행";
        constexpr const char* INFO_ENGINE_STARTED = u8"[정보] AronEngine이 성공적으로 시작되었습니다";
        constexpr const char* DEBUG_IMGUI_INIT = u8"[디버그] ImGui 에디터 시스템이 초기화되었습니다";
        constexpr const char* INFO_SCENE_LOADED = u8"[정보] 씬 로드됨: 메인 씬";
        constexpr const char* WARNING_NO_AUDIO = u8"[경고] 오디오 장치를 찾을 수 없습니다";
        constexpr const char* ERROR_TEXTURE_LOAD = u8"[오류] 텍스처 로드 실패: missing_texture.png";
        
        // Asset Browser
        constexpr const char* ASSETS = u8"에셋";
        constexpr const char* FOLDER_TEXTURES = u8"📁 텍스처";
        constexpr const char* FOLDER_AUDIO = u8"📁 오디오";
        constexpr const char* FOLDER_SCRIPTS = u8"📁 스크립트";
        constexpr const char* FOLDER_SCENES = u8"📁 씬";
        constexpr const char* FOLDER_MATERIALS = u8"📁 머티리얼";
        constexpr const char* TEXTURE_HINT = u8"📄 (텍스처 파일을 여기에 드래그)";
        constexpr const char* TEXTURE_SUPPORT = u8"📷 지원: .png, .jpg, .bmp";
        constexpr const char* AUDIO_HINT = u8"🎵 (오디오 파일을 여기에 드래그)";
        constexpr const char* AUDIO_SUPPORT = u8"🔊 지원: .wav, .mp3, .ogg";
        constexpr const char* SCRIPTS_HINT = u8"📜 (C++ 스크립트)";
        constexpr const char* MATERIALS_HINT = u8"✨ (여기에 머티리얼 생성)";
        constexpr const char* IMPORT_HINT = u8"에셋을 가져오려면 우클릭하세요";
        constexpr const char* IMPORT_TEXTURE = u8"텍스처 가져오기...";
        constexpr const char* CREATE_MATERIAL = u8"머티리얼 생성";
        
        // Scene View
        constexpr const char* SCENE_VIEW_SUBTITLE = u8"씬 뷰 - 3D/2D 에디터";
        constexpr const char* TOOL_MOVE = u8"이동";
        constexpr const char* TOOL_ROTATE = u8"회전";
        constexpr const char* TOOL_SCALE = u8"크기";
        constexpr const char* VIEWPORT = u8"뷰포트: ";
        constexpr const char* VIEWPORT_AREA = u8"뷰포트 영역";
        
        // Game View
        constexpr const char* GAME_VIEW_SUBTITLE = u8"게임 뷰 - 런타임";
        constexpr const char* GAME_VIEWPORT = u8"게임 뷰포트: ";
        constexpr const char* GAME_VIEWPORT_AREA = u8"게임 뷰포트 영역";
        
        // Debug Messages
        constexpr const char* LOG_OBJECT_CREATED = u8"게임오브젝트 생성됨: ";
        constexpr const char* LOG_SPRITE_CREATED = u8"스프라이트 게임오브젝트 생성됨: ";
        constexpr const char* LOG_SQUARE_CREATED = u8"사각형 게임오브젝트 생성됨: ";
        constexpr const char* LOG_CIRCLE_CREATED = u8"원형 게임오브젝트 생성됨: ";
        constexpr const char* LOG_NEW_SCENE = u8"새 씬이 생성되었습니다";
        constexpr const char* LOG_SCENE_SAVE_FAILED = u8"scene.json 쓰기 모드로 열기 실패";
        constexpr const char* LOG_SCENE_SAVED = u8"씬이 scene.json에 저장되었습니다";
        constexpr const char* LOG_SCENE_NOT_FOUND = u8"씬 파일을 찾을 수 없음 - 빈 씬 생성";
        constexpr const char* LOG_SCENE_LOADED = u8"scene.json에서 %d개의 오브젝트를 로드했습니다";
        constexpr const char* LOG_IMGUI_WIN32_FAILED = u8"ImGui Win32 백엔드 초기화 실패";
        constexpr const char* LOG_IMGUI_DX11_FAILED = u8"ImGui DX11 백엔드 초기화 실패";
        constexpr const char* LOG_D3D11_FAILED = u8"ImGui용 D3D11 디바이스 생성 실패";
        constexpr const char* LOG_IMGUI_INIT_SUCCESS = u8"ImGui 에디터 시스템이 성공적으로 초기화되었습니다";
        constexpr const char* LOG_IMGUI_SHUTDOWN = u8"ImGui 에디터 시스템 종료";
        constexpr const char* LOG_TEXTURE_IMPORT = u8"텍스처 가져오기 대화상자가 열립니다";
        constexpr const char* LOG_MATERIAL_CREATE = u8"머티리얼 생성 대화상자가 열립니다";
        
        // GameObject names
        constexpr const char* NAME_GAMEOBJECT = u8"게임오브젝트";
        constexpr const char* NAME_SPRITE = u8"스프라이트";
        constexpr const char* NAME_SQUARE = u8"사각형";
        constexpr const char* NAME_CIRCLE = u8"원";
    }
}