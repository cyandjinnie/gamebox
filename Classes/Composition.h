#ifndef __COMPOSITION_H__
#define __COMPOSITION_H__

#include "cocos2d.h"
#include "ui\CocosGUI.h"

///////////////////////////////////////
// Colors

#define WIDGET_BG_COLOR cocos2d::Color3B(26, 44, 81)
#define BLUE_COLOR cocos2d::Color3B(84, 178, 255)
#define DARK_BLUE_COLOR cocos2d::Color3B(26, 44, 81)
#define CYAN_COLOR cocos2d::Color3B(100, 170, 170)
#define AZURE_COLOR cocos2d::Color3B(84, 178, 128)
#define DARK_RED_COLOR cocos2d::Color3B(175, 95, 89)
#define BLACK_COLOR cocos2d::Color3B(25, 25, 25)
#define GOLDEN_COLOR cocos2d::Color3B(209, 201, 105)

///////////////////////////////////////
// Sizes

// This one isnt modified
#define SIZE_IMG_BUTTON cocos2d::Size(350, 100)

///////////////////////////////////////
// Images

// BACKGROUND

#define IMG_MENU_TAB_BG "res/menutabbg.png"
#define IMG_MAIN_MENU_BG "res/bg_v2.png"
#define IMG_TEXTFIELD_BG "res/textfield.png"
#define IMG_WIDGET_BG "res/widget_bg.png"

// TITLES

#define IMG_NEWGAME_TITLE "res/newgametitle.png"
#define IMG_GAME_TITLE "res/gametitle.png"

// BUTTONS

// Main menu  {

#define IMG_NEWGAME_INACTIVE "res/newgame_inactive.png"
#define IMG_NEWGAME_ACTIVE "res/newgame_active.png"

#define IMG_SEARCH_INACTIVE "res/search_inactive.png"
#define IMG_SEARCH_ACTIVE "res/search_active.png"

#define IMG_EXIT_INACTIVE "res/exit_inactive.png"
#define IMG_EXIT_ACTIVE "res/exit_active.png"

// }

// New game tab {

#define IMG_CONTINUE_INACTIVE "res/continue_inactive.png"
#define IMG_CONTINUE_ACTIVE "res/continue_active.png"

// }

// LobbyConnection {

#define IMG_REFRESH_INACTIVE "res/refresh_inactive.png"
#define IMG_REFRESH_ACTIVE "res/refresh_active.png"

#define IMG_CONNECT_INACTIVE "res/connect_inactive.png"
#define IMG_CONNECT_ACTIVE "res/connect_active.png"

#define IMG_CANCEL_INACTIVE "res/cancel_inactive.png"
#define IMG_CANCEL_ACTIVE "res/cancel_active.png"

// }

// LobbyUser && LobbyHost {

#define IMG_READY_INACTIVE "res/ready_inactive.png"
#define IMG_READY_ACTIVE "res/ready_active.png"

#define IMG_NOT_READY_INACTIVE "res/not_ready_inactive.png"
#define IMG_NOT_READY_ACTIVE "res/not_ready_active.png"

#define IMG_SEND_ACTIVE "res/send_active.png"
#define IMG_SEND_INACTIVE "res/send_inactive.png"

#define IMG_BUTTON_ACTIVE "res/button_active.png"
#define IMG_BUTTON_INACTIVE "res/button_inactive.png"

// }

// LobbyHost {

#define IMG_START_INACTIVE "res/start_inactive.png"
#define IMG_START_ACTIVE "res/start_active.png"

// }

// GameScene {

#define IMG_BLANK_TILE "res/blank_tile.png"
#define IMG_CROSS_TILE "res/cross_tile.png"
#define IMG_CIRCLE_TILE "res/circle_tile.png"

// }


#endif __COMPOSITION_H__