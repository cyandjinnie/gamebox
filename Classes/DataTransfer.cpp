#include "DataTransfer.h"

SOCKET DataTransfer::Socket = INVALID_SOCKET;
GameScene::UserStatus DataTransfer::Status = GameScene::UserStatus::CLIENT;
bool DataTransfer::MyTurnFirst = false;
std::string DataTransfer::RoomName = "";

ssize_t filesize_config;
std::string DataTransfer::NetMask = reinterpret_cast<char*>(cocos2d::FileUtils::getInstance()->getFileData("config/netconfig.txt", "r", &filesize_config));