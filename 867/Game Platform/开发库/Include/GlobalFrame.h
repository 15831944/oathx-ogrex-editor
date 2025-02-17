#ifndef GLOBAL_FRAME_HEAD_FILE
#define GLOBAL_FRAME_HEAD_FILE

#pragma once

#include "GlobalDef.h"
#include "GlobalProperty.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏状态
#define GS_FREE							0								//空闲状态
#define GS_PLAYING						100								//游戏状态

//////////////////////////////////////////////////////////////////////////
//IPC 网络事件

#define IPC_MAIN_SOCKET					1								//网络消息

#define IPC_SUB_SOCKET_SEND				1								//网络发送
#define IPC_SUB_SOCKET_RECV				2								//网络接收

//IPC 网络包结构
struct IPC_SocketPackage
{
	CMD_Command							Command;						//命令信息
	BYTE								cbBuffer[SOCKET_PACKET];		//数据缓冲
};

//////////////////////////////////////////////////////////////////////////
//IPC 配置信息

#define IPC_MAIN_CONFIG					2								//配置信息

#define IPC_SUB_SERVER_INFO				1								//房间信息
#define IPC_SUB_COLUMN_INFO				2								//列表信息

//游戏信息
struct IPC_GF_ServerInfo
{
	DWORD								dwUserID;						//用户 I D
	WORD								wTableID;						//桌子号码
	WORD								wChairID;						//椅子号码
	WORD								wKindID;						//类型标识
	WORD								wTypeID;						//游戏类型
	WORD								wServerID;						//房间标识
	WORD								wGameGenre;						//游戏类型
	WORD								wChairCount;					//椅子数目
	BYTE								cbHideUserInfo;					//隐藏信息
	DWORD								dwVideoAddr;					//视频地址
	TCHAR								szKindName[KIND_LEN];			//类型名字
	TCHAR								szServerName[SERVER_LEN];		//房间名称
};

//////////////////////////////////////////////////////////////////////////
//IPC 用户信息

#define IPC_MAIN_USER					3								//用户信息

#define IPC_SUB_USER_COME				1								//用户信息
#define IPC_SUB_USER_STATUS				2								//用户状态
#define IPC_SUB_USER_SCORE				3								//用户积分
#define IPC_SUB_GAME_START				4								//游戏开始
#define IPC_SUB_GAME_FINISH				5								//游戏结束
#define IPC_SUB_UPDATE_FACE				6								//更新头像
#define IPC_SUB_MEMBERORDER				7								//会员等级
#define IPC_SUB_FLASHUSERINFO			8								//刷新用户信息


//用户状态
struct IPC_UserStatus
{
	DWORD								dwUserID;						//用户 I D
	WORD								wNetDelay;						//网络延时
	BYTE								cbUserStatus;					//用户状态
};

//用户分数
struct IPC_UserScore
{
	LONG								lLoveliness;					//用户魅力
	DWORD								dwUserID;						//用户 I D
	tagUserScore						UserScore;						//用户积分
};

//会员等级
struct IPC_MemberOrder
{
	BYTE								cbMemberOrder;					//会员等级
	DWORD								dwUserID;						//用户 I D
};

//用户分数
struct IPC_UpdateFace
{
	DWORD								dwCustomFace;					//用户 I D
};

//////////////////////////////////////////////////////////////////////////
//IPC 控制信息

#define IPC_MAIN_CONCTROL				4								//控制信息

#define IPC_SUB_START_FINISH			1								//启动完成
#define IPC_SUB_CLOSE_FRAME				2								//关闭框架
#define IPC_SUB_JOIN_IN_GAME			3								//加入游戏

//加入游戏
struct IPC_JoinInGame
{
	WORD								wTableID;						//桌子号码
	WORD								wChairID;						//椅子号码
};

//////////////////////////////////////////////////////////////////////////
//网络命令码

#define MDM_GF_GAME						100								//游戏消息
#define MDM_GF_FRAME					101								//框架消息
#define	MDM_GF_PRESENT					102								//礼物消息
#define	MDM_GF_BANK						103								//银行消息

#define SUB_GF_INFO						1								//游戏信息
#define SUB_GF_USER_READY				2								//用户同意
#define SUB_GF_LOOKON_CONTROL			3								//旁观控制
#define SUB_GF_KICK_TABLE_USER			4								//踢走用户
#define SUB_GF_BANK						5								//银行信息
#define SUB_GF_MODIFY_LOGIN_PWD			6								//修改登录密码
#define SUB_GF_MODIFY_BANK_PWD			7								//修改银行密码
#define SUB_GF_MODIFY_NICKNAME			8								//修改昵称
#define SUB_GF_TRANSFER_MONEY			9								//划拨攒款
#define SUB_GF_TRANSFER_LOG				10								//查询划拨记录
#define SUB_GF_TRANSFER_LOG_ITEM		11								//查询划拨记录
#define SUB_GF_FILL_MONEY				12								//充值
#define SUB_GF_OPTION					100								//游戏配置
#define SUB_GF_SCENE					101								//场景信息
#define SUB_GF_USER_CHAT				200								//用户聊天
#define SUB_GF_MESSAGE					300								//系统消息
//#define SUB_GF_GIFT						400								//赠送消息
#define SUB_GF_BANK_STORAGE				450								//银行存储
#define SUB_GF_BANK_GET					451								//银行提取

#define SUB_GF_FLOWER_ATTRIBUTE			500								//鲜花属性
#define SUB_GF_FLOWER					501								//鲜花消息
#define SUB_GF_EXCHANGE_CHARM			502								//兑换魅力

#define SUB_GF_PROPERTY					550								//道具消息
#define SUB_GF_PROPERTY_RESULT			551								//道具结果
#define SUB_GF_RESIDUAL_PROPERTY		552								//剩余道具
#define SUB_GF_PROP_ATTRIBUTE			553								//道具属性
#define SUB_GF_PROP_BUGLE				554								//喇叭道具
#define SUB_GF_QUERY_USERNAME_RET		555

//版本信息
struct CMD_GF_Info
{
	BYTE								bAllowLookon;					//旁观标志
};

//游戏配置
struct CMD_GF_Option
{
	BYTE								bGameStatus;					//游戏状态
	BYTE								bAllowLookon;					//允许旁观
};

//旁观控制
struct CMD_GF_LookonControl
{
	DWORD								dwUserID;						//用户标识
	BYTE								bAllowLookon;					//允许旁观
};

//踢走用户
struct CMD_GF_KickTableUser
{
	DWORD								dwUserID;						//用户 I D
};

//聊天结构
struct CMD_GF_UserChat
{
	WORD								wChatLength;					//信息长度
	COLORREF							crFontColor;					//信息颜色
	DWORD								dwSendUserID;					//发送用户
	DWORD								dwTargetUserID;					//目标用户
	TCHAR								szChatMessage[MAX_CHAT_LEN];	//聊天信息
};
//////////////////////////////////////////////////////////////////////////

//消息类型
#define SMT_INFO						0x0001							//信息消息
#define SMT_EJECT						0x0002							//弹出消息
#define SMT_GLOBAL						0x0004							//全局消息
#define SMT_CLOSE_GAME					0x1000							//关闭游戏

//消息数据包
struct CMD_GF_Message
{
	WORD								wMessageType;					//消息类型
	WORD								wMessageLength;					//消息长度
	TCHAR								szContent[1024];				//消息内容
};

//////////////////////////////////////////////////////////////////////////

//共享内存定义
struct tagShareMemory
{
	WORD								wDataSize;						//数据大小
	HWND								hWndGameFrame;					//框架句柄
	HWND								hWndGamePlaza;					//广场句柄
	HWND								hWndGameServer;					//房间句柄
};

//////////////////////////////////////////////////////////////////////////

//发送场所
#define	LOCATION_GAME_ROOM				1								//游戏房间
#define	LOCATION_PLAZA_ROOM				2								//大厅房间

//////////////////////////////////////////////////////////////////////////

//赠送结构
struct CMD_GF_Gift
{
	BYTE								cbSendLocation;					//发送场所
	DWORD								dwSendUserID;					//赠送者ID
	DWORD								dwRcvUserID;					//接受者ID
	WORD								wGiftID;						//礼物	ID
	WORD								wFlowerCount;					//鲜花数目
};

//道具结构
struct CMD_GF_Property
{
	BYTE								cbSendLocation;					//发送场所
	int									nPropertyID;					//道具ID
	DWORD								dwPachurseCount;				//购买数目
	DWORD								dwOnceCount;					//单次数目
	DWORD								dwSourceUserID;					//赠送玩家
	DWORD								dwTargetUserID;					//目标玩家
	TCHAR								szRcvUserName[32];				//用户名称
};

//喇叭结构
struct CMD_GF_BugleProperty
{
	BYTE								cbSendLocation;					//发送场所
	TCHAR								szUserName[32];					//玩家帐号
	COLORREF							crText;							//文字颜色
	TCHAR								szContext[BUGLE_MAX_CHAR];		//喇叭内容
};

//兑换结构
struct CMD_GF_ExchangeCharm
{
	BYTE								cbSendLocation;					//发送场所
	LONG								lLoveliness;					//魅力数值
	DWORD								lGoldValue;						//金币数值
};

//赠送通知
struct CMD_GF_GiftNotify
{
	BYTE								cbSendLocation;					//发送场所
	DWORD								dwSendUserID;					//赠送者ID
	DWORD								dwRcvUserID;					//接受者ID
	WORD								wGiftID;						//礼物	ID
	WORD								wFlowerCount;					//鲜花数目
};

//存储金币
struct CMD_GF_BankStorage
{
	LONG								lStorageValue;					//存储金币
	TCHAR								szPassword[PASS_LEN];			//用户密码
};

//提取金币
struct CMD_GF_BankGet
{
	LONG								lGetValue;						//提取金币
	TCHAR								szPassword[PASS_LEN];			//用户密码
};

//剩余道具
struct CMD_GF_ResidualProperty
{
	DWORD								dwResidualTime[PROPERTY_COUNT];	//剩余时间
};

#define	MDM_TOOLBOX							110				//工具
#define SUB_TOOLBOX_QUERYUSERNAME			1				//查询用户名
#define SUB_TOOLBOX_BANKOPERATING			2				//银行操作
#define SUB_TOOLBOX_TRANSFERMONEY			3				//转账
#define SUB_TOOLBOX_TRANSFERMONEY_LOG		4				//转账日志
#define SUB_TOOLBOX_MESSAGE					7				//消息

//查询用户名
struct CMD_TOOLBOX_QueryUserName
{
	long lGameID;
};

//查询用户名返回
struct CMD_TOOLBOX_QueryUserName_Ret
{
	long								lGameID;
	char								UserNmae[NAME_LEN];
	LONG								lErrorCode;						//是否成功
	TCHAR								szErrorDescribe[256];			//错误说明
};

//转账
struct CMD_TOOLBOX_TransferMoney
{
	__int64								sfMoneyNumber;
	DWORD								dwGameID;						//对方的ID号
	TCHAR								szAccount[NAME_LEN];			//对方用户名
	TCHAR								szPassword[PASS_LEN];			//用户密码
};

//转账返回
struct CMD_TOOLBOX_TransferMoney_Ret
{
	LONG								lErrorCode;						//是否成功
	__int64								sfMoneyNumber;					//自己口袋里剩余的银子
	__int64								sfLeftMoney;					//自己口袋里剩余的银子
	__int64								sfTax;							//税收
	TCHAR								szErrorDescribe[256];			//错误说明
};

enum BANKTASK
{
	BANKTASK_QUERY=0, //查询银行金币
	BANKTASK_DEPOSIT=1, //存钱
	BANKTASK_WITHDRAW=2,//取钱
};

//银行
struct CMD_TOOLBOX_BankTask
{
	LONG								lBankTask;						//打算进行的操作
	__int64								lMoneyNumber;					//打算操作的钱的数目
	TCHAR								szPassword[PASS_LEN];			//用户密码
};

//银行
struct CMD_TOOLBOX_BankTask_Ret
{
	LONG								lBankTask;						//进行的操作
	__int64								lMoneyNumber;					//实际操作的钱的数目
	__int64								lMoneyInBank;					//银行存款余额
	__int64								lNewScore;						//口袋里的银子数目
	LONG								lErrorCode;						//
	TCHAR								szErrorDescribe[256];			//错误说明
};

struct CMD_GF_Transfer_Log_Out
{
	DWORD								dwUserID;						//用户 I D
	WORD								wTableID;						//桌子ID
};

struct CMD_GF_Transfer_Log_Item
{
	DWORD								dwUserID;						//用户 I D
	TCHAR								szOutAccount[NAME_LEN];
	TCHAR								szInAccount[NAME_LEN];
	WORD								wTableID;						//桌子ID
	__int64								sfMoney;
	__int64								sfTax;
	SYSTEMTIME							TransTime;
	TCHAR								szIP[16];
};

//修改昵称返回结构体
struct CMD_GF_Modify_Nickname
{
	LONG								lErrorCode;						//是否成功
	TCHAR								szNickname[NAME_LEN];			//新昵称
	TCHAR								szErrorDescribe[256];			//错误说明
};

//////////////////////////////////////////////////////////////////////////
#endif