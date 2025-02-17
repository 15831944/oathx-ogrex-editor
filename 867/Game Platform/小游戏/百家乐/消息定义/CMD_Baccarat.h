#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
static void SuperRand()
{
	srand(::GetTickCount());

	LONG count=rand()%30;

	for ( int i=0; i<count; i++ )
	{
		rand();
	}
}
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }

#define LIMIT_VALUE(x, min, max) {if ((x)<(min)) (x)=(min); if ((x)>(max)) (x)=(max);}

#define SOCKET_PACKAGE				2046								//最大网络包
#define MAX_DB_JETTON_LEN			128									//押注的最大字节长度
//公共宏定义
#ifdef VIDEO_GAME
#define KIND_ID						502									//游戏 I D
#else
#define KIND_ID						100									//游戏 I D
#endif

#define GAME_PLAYER					100									//游戏人数
#define GAME_NAME					TEXT("百家乐")						//游戏名字
#define GAME_GENRE					(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型


#define BANK_CONDITION_MONEY		20000000

//玩家索引
#define ID_XIAN_JIA					1									//闲家索引
#define ID_PING_JIA					2									//平家索引
#define ID_ZHUANG_JIA				3									//庄家索引
#define ID_XIAN_TIAN_WANG			4									//闲天王
#define ID_ZHUANG_TIAN_WANG			5									//庄天王
#define ID_TONG_DIAN_PING			6									//同点平

//记录信息
struct tagServerGameRecord
{
	WORD							wWinner;							//胜利玩家
	__int64							lTieScore;							//买平总注
	__int64							lBankerScore;						//买庄总注
	__int64							lPlayerScore;						//买闲总注
	BYTE							cbPlayerCount;						//闲家点数
	BYTE							cbBankerCount;						//庄家点数
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_PLACE_JETTON			101									//用户下注
#define SUB_S_GAME_END				102									//游戏结束
#define SUB_S_APPLY_BANKER			103									//申请庄家
#define SUB_S_CHANGE_BANKER			104									//切换庄家
#define SUB_S_CHANGE_USER_SCORE		105									//更新积分
#define SUB_S_SEND_RECORD			106									//游戏记录
#define SUB_S_PLACE_JETTON_FAIL		107									//下注失败
#define SUB_S_GAME_SCORE			108									//发送积分
#define SUB_S_JETTON_CHANGE			109									//下注数改变
#define SUB_S_StartJetton			110									//开始下注

//失败结构
struct CMD_S_PlaceJettonFail
{
	__int64							lJettonArea;						//下注区域
	__int64							lPlaceScore;						//当前下注
	__int64							lMaxLimitScore;						//限制大小
	__int64							lFinishPlaceScore;					//已下注额
};

//更新积分
struct CMD_S_ChangeUserScore
{
	WORD							wChairID;							//椅子号码
	__int64							lScore;								//玩家积分

	//庄家信息
	WORD							wCurrentBankerChairID;				//当前庄家
	BYTE							cbBankerTime;						//庄家局数
	__int64							lCurrentBankerScore;				//庄家分数
};

//申请庄家
struct CMD_S_ApplyBanker
{
	CHAR							szAccount[ 32 ];					//申请玩家
	__int64							lScore;								//玩家金币
	bool							bApplyBanker;						//申请标识
};

//切换庄家
struct CMD_S_ChangeBanker
{
	WORD							wChairID;							//椅子号码
	BYTE							cbBankerTime;						//庄家局数
	__int64							lBankerScore;						//庄家分数
	__int64							lBankerTreasure;					//庄家金币
};

//游戏状态
struct CMD_S_StatusFree
{
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间
	__int64							lCellScore;							//最大单元下注
	//下注信息
	__int64							lTieScore;							//买平总注
	__int64							lBankerScore;						//买庄总注
	__int64							lPlayerScore;						//买闲总注
	__int64							lTieSamePointScore;					//同点平注
	__int64							lPlayerKingScore;					//闲天王注
	__int64							lBankerKingScore;					//庄天王注

	//我的下注
	__int64							lMeMaxScore;						//最大下注
	__int64							lMeTieScore;						//买平总注
	__int64							lMeBankerScore;						//买庄总注
	__int64							lMePlayerScore;						//买闲总注
	__int64							lMeTieKingScore;					//同点平注
	__int64							lMePlayerKingScore;					//闲天王注
	__int64							lMeBankerKingScore;					//庄天王注

	//庄家信息
	WORD							wCurrentBankerChairID;				//当前庄家
	BYTE							cbBankerTime;						//庄家局数
	__int64							lCurrentBankerScore;				//庄家分数
	__int64							lApplyBankerCondition;				//申请条件
	__int64							lBankerTreasure;					//庄家金币
};

//游戏状态
struct CMD_S_StatusPlay
{
	BYTE                            cbTimeLeave;						//剩余时间
	__int64							lCellScore;							//最大单元下注
	//下注信息
	__int64							lTieScore;							//买平总注
	__int64							lBankerScore;						//买庄总注
	__int64							lPlayerScore;						//买闲总注
	__int64							lTieSamePointScore;					//同点平注
	__int64							lPlayerKingScore;					//闲天王注
	__int64							lBankerKingScore;					//庄天王注

	//我的下注
	__int64							lMeMaxScore;						//最大下注
	__int64							lMeTieScore;						//买平总注
	__int64							lMeBankerScore;						//买庄总注
	__int64							lMePlayerScore;						//买闲总注
	__int64							lMeTieKingScore;					//同点平注
	__int64							lMePlayerKingScore;					//闲天王注
	__int64							lMeBankerKingScore;					//庄天王注

	//扑克信息
 	BYTE							cbCardCount[2];						//扑克数目
	BYTE							cbTableCardArray[2][3];				//桌面扑克

	//庄家信息
	WORD							wCurrentBankerChairID;				//当前庄家
	BYTE							cbBankerTime;						//庄家局数
	__int64							lCurrentBankerScore;				//庄家分数
	__int64							lApplyBankerCondition;				//申请条件
	__int64							lBankerTreasure;					//庄家金币
};

//游戏开始
struct CMD_S_GameStart
{
	BYTE							cbTimeLeave;
	BYTE							cbCardCount[2];						//扑克数目
    BYTE							cbTableCardArray[2][3];				//桌面扑克
	__int64							lApplyBankerCondition;				//申请条件
	//庄家信息
	WORD							wBankerChairID;						//庄家号码
	__int64							lBankerScore;						//庄家积分
	BYTE							cbBankerTime;						//做庄次数
};

//用户下注
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//用户位置
	BYTE							cbJettonArea;						//筹码区域
	__int64							lJettonScore;						//加注数目
};

//游戏结束
struct CMD_S_GameEnd
{
	//下局信息
	__int64							lMeMaxScore;						//最大下注
	__int64							lCellScore;							//最大下注
	BYTE							cbTimeLeave;						//剩余时间

	//成绩记录
	BYTE							cbWinner;							//胜利玩家
	BYTE							cbKingWinner;						//天王胜利
	__int64							lBankerTreasure;					//庄家金币

	__int64							lBankerTotalScore;					//庄家成绩
	__int64							lBankerScore;						//庄家成绩
	INT								nBankerTime;						//做庄次数
};

//游戏得分
struct CMD_S_GameScore
{
	//成绩记录
	BYTE							cbWinner;							//胜利玩家
	BYTE							cbKingWinner;						//天王胜利
	__int64							lMeGameScore;						//我的成绩
	__int64							lMeReturnScore;						//返还注额
	__int64							lBankerScore;						//庄家成绩

	//下注信息
	__int64							lDrawTieScore;						//买平总注
	__int64							lDrawBankerScore;					//买庄总注
	__int64							lDrawPlayerScore;					//买闲总注
	__int64							lDrawTieSamPointScore;				//同点平注
	__int64							lDrawPlayerKingScore;				//闲天王注
	__int64							lDrawBankerKingScore;				//庄天王注

	//我的下注
	__int64							lMeTieScore;						//买平总注
	__int64							lMeBankerScore;						//买庄总注
	__int64							lMePlayerScore;						//买闲总注
	__int64							lMeTieKingScore;					//同点平注
	__int64							lMePlayerKingScore;					//闲天王注
	__int64							lMeBankerKingScore;					//庄天王注
};

//开始下注
struct CMD_S_JettonStart
{
	BYTE							cbTimeLeave;						//剩余时间
};

//押注玩家强退信息改变
struct CMD_S_JettonChange
{
	__int64							lIieScore;								//扣除平门
	__int64							lBankerScore;							//扣除庄门
	__int64							lPlayerScore;							//扣除闲门
	__int64							lTieSamePointScore;						//扣除同点门
	__int64							lPlayerKingScore;						//扣除闲天王
	__int64							lBankerkingScore;						//扣除庄天王

	__int64							lZhuangSocre;							//
	__int64							lKexiaSocre;							//已经下注的总值	
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_JETTON			1									//用户下注
#define SUB_C_APPLY_BANKER			2									//申请庄家

//用户下注
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;						//筹码区域
	__int64							lJettonScore;						//加注数目
};

//申请庄家
struct CMD_C_ApplyBanker
{
	bool							bApplyBanker;						//申请标识
};

//////////////////////////////////////////////////////////////////////////

#endif