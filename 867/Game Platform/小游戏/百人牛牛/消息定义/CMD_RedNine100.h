#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define LIMIT_VALUE(x, min, max) {if ((x)<(min)) (x)=(min); if ((x)>(max)) (x)=(max);}

#define KIND_ID						104									//游戏 I D
#define GAME_PLAYER					100									//游戏人数
#define GAME_NAME					TEXT("百人牛牛")					//游戏名字

//状态定义
#define	GS_PLACE_JETTON				GS_PLAYING							//下注状态
#define	GS_GAME_END					GS_PLAYING+1						//结束状态

//区域索引
#define ID_TIAN_MEN					1									//顺门
#define ID_DI_MEN					2									//左边角
#define ID_XUAN_MEN					3									//桥
#define ID_HUANG_MEN				4									//对门

//玩家索引
#define BANKER_INDEX				0									//庄家索引
#define SHUN_MEN_INDEX				1									//顺门索引
#define DUI_MEN_INDEX				2									//对门索引
#define DAO_MEN_INDEX				3									//倒门索引
#define HUAN_MEN_INDEX				4									//倒门索引
#define AREA_COUNT					4									//区域数目


//记录信息
struct tagServerGameRecord
{
	bool							bWinShunMen;						//顺门胜利
	bool							bWinDuiMen;							//对门胜利
	bool							bWinDaoMen;							//倒门胜利
	bool							bWinHuang;							//倒门胜利
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_FREE				99									//游戏空闲
#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_PLACE_JETTON			101									//用户下注
#define SUB_S_GAME_END				102									//游戏结束
#define SUB_S_APPLY_BANKER			103									//申请庄家
#define SUB_S_CHANGE_BANKER			104									//切换庄家
#define SUB_S_CHANGE_USER_SCORE		105									//更新积分
#define SUB_S_SEND_RECORD			106									//游戏记录
#define SUB_S_PLACE_JETTON_FAIL		107									//下注失败
#define SUB_S_CANCEL_BANKER			108									//取消申请


//失败结构
struct CMD_S_PlaceJettonFail
{
	WORD							wPlaceUser;							//下注玩家
	BYTE							lJettonArea;						//下注区域
	LONG							lPlaceScore;						//当前下注
};

//更新积分
struct CMD_S_ChangeUserScore
{
	WORD							wChairID;							//椅子号码
	DOUBLE							lScore;								//玩家积分

	//庄家信息
	WORD							wCurrentBankerChairID;				//当前庄家
	BYTE							cbBankerTime;						//庄家局数
	DOUBLE							lCurrentBankerScore;				//庄家分数
};

//申请庄家
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;							//申请玩家
};

//取消申请
struct CMD_S_CancelBanker
{
	TCHAR							szCancelUser[32];					//取消玩家
};

//切换庄家
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;						//当庄玩家
	__int64							lBankerScore;						//庄家金币
};

//游戏状态
struct CMD_S_StatusFree
{
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间

	//玩家信息
	__int64							lUserMaxScore;							//玩家金币

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							cbBankerTime;						//庄家局数
	__int64							lBankerWinScore;					//庄家成绩
	__int64							lBankerScore;						//庄家分数
	bool							bEnableSysBanker;					//系统做庄

	//控制信息
	__int64							lApplyBankerCondition;				//申请条件
	__int64							lAreaLimitScore;					//区域限制
};

//游戏状态
struct CMD_S_StatusPlay
{
	//全局下注
	__int64							lAllJettonScore[AREA_COUNT+1];		//全体总注

	//玩家下注
	__int64							lUserJettonScore[AREA_COUNT+1];		//个人总注

	//玩家积分
	__int64							lUserMaxScore;						//最大下注							

	//控制信息
	__int64							lApplyBankerCondition;				//申请条件
	__int64							lAreaLimitScore;					//区域限制

	//扑克信息
	BYTE							cbTableCardArray[5][5];				//桌面扑克

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							cbBankerTime;						//庄家局数
	__int64							lBankerWinScore;					//庄家赢分
	__int64							lBankerScore;						//庄家分数
	bool							bEnableSysBanker;					//系统做庄

	//结束信息
	__int64							lEndBankerScore;					//庄家成绩
	__int64							lEndUserScore;						//玩家成绩
	__int64							lEndUserReturnScore;				//返回积分
	__int64							lEndRevenue;						//游戏税收

	//全局信息
	BYTE							cbTimeLeave;						//剩余时间
	BYTE							cbGameStatus;						//游戏状态
};

//游戏空闲
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//剩余时间
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//庄家位置
	__int64							lBankerScore;						//庄家金币
	__int64							lUserMaxScore;						//我的金币
	BYTE							cbTimeLeave;						//剩余时间	
	bool							bContiueCard;						//继续发牌
};

//用户下注
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//用户位置
	BYTE							cbJettonArea;						//筹码区域
	LONG							lJettonScore;						//加注数目
};

//游戏结束
struct CMD_S_GameEnd
{
	//下局信息
	BYTE							cbTimeLeave;						//剩余时间

	//扑克信息
	BYTE							cbTableCardArray[5][5];				//桌面扑克
	BYTE							cbLeftCardCount;					//扑克数目

	BYTE							bcFirstCard;
 
	//庄家信息
	__int64							lBankerScore;						//庄家成绩
	__int64							lBankerTotallScore;					//庄家成绩
	INT								nBankerTime;						//做庄次数

	//玩家成绩
	__int64							lUserScore;							//玩家成绩
	__int64							lUserReturnScore;					//返回积分

	//全局信息
	__int64							lRevenue;							//游戏税收
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_JETTON			1									//用户下注
#define SUB_C_APPLY_BANKER			2									//申请庄家
#define SUB_C_CANCEL_BANKER			3									//取消申请
#define SUB_C_CONTINUE_CARD			4									//继续发牌

//用户下注
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;						//筹码区域
	LONG							lJettonScore;						//加注数目
};

//////////////////////////////////////////////////////////////////////////

#endif
