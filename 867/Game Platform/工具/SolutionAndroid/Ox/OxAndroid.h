#pragma once

#include "IAndroid.h"

namespace O2
{
	//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

#define MAX_COUNT					5
#define MIN_CHAIR_COUNT				1
#define MAX_WAIT_TIME				30

	//扑克类型
#define OX_VALUE0					0									//混合牌型
#define OX_THREE_SAME				12									//三条牌型
#define OX_FOUR_SAME				13									//四条牌型
#define OX_FOURKING					14									//天王牌型
#define OX_FIVEKING					15									//天王牌型

	struct STimerItem
	{
		DWORD						dwID;
		float						fElapsed;
		BOOL						bActive;

		STimerItem(DWORD id, float fTime, BOOL bInvalid)
			: dwID(id), fElapsed(fTime), bActive(bInvalid)
		{

		}
	};
	
	typedef std::map<DWORD, STimerItem*> TimerItemRegister;

	struct SDesk{
		WORD						wChairID[4];
		WORD						wTableID;
		BYTE						bPlayStatus;
		BYTE						bTableLock;

		SDesk()
			: wTableID(INVALID_TABLE)
		{
			for (int i=0; i<4; i++)
			{
				wChairID[i] = INVALID_CHAIR;
			}

			bPlayStatus	= FALSE;
			bTableLock	= FALSE;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// 牛牛机器人
	//////////////////////////////////////////////////////////////////////////
	class Ox : public IAndroid
	{
	public:
		// 构造函数
		Ox(DWORD dwUserID, double fOnlineTime);
		// 析构函数
		virtual ~Ox();
		
	public:
		// 周期性更新
		virtual bool			Update(float fElapsed);

	public:
		//获取类型
		virtual BYTE			GetCardType(BYTE cbCardData[], BYTE cbCardCount);
		//获取数值
		virtual BYTE			GetCardValue(BYTE cbCardData);
		//获取花色
		virtual BYTE			GetCardColor(BYTE cbCardData);
		//逻辑数值
		virtual BYTE			GetCardLogicValue(BYTE cbCardData);
		//获取工作时间
		virtual WORD			GetWorkTime();
		//获取桌子ID
		virtual bool			GetTableID(WORD& wTableID, WORD& wChairID);
	protected:
		/*
		* 房价服务信息
		*/
		virtual bool			OnSocketServerInfo(CMD_Command Command, void* pBuffer, 
			WORD wDataSize);
		/*
		* 房价服务信息
		*/
		virtual bool			OnGameStatus(CMD_Command Command, void* pBuffer, WORD wDataSize);

		/*
		* 重置机器人数据
		*/	
		virtual bool			OnReset();

		/*
		* 选择桌子信息
		*/	
		virtual bool			OnSwitchTable();

		/*
		*游戏状态
		*/
		virtual bool			OnGameSceneMessage(BYTE cbGameStation, void * pBuffer, 
			WORD wDataSize);
		
		/*
		*游戏消息
		*/
		virtual bool			OnGameMessage(WORD wSubCmdID, const void * pBuffer=NULL, 
			WORD wDataSize=0);
		/*
		*响应银行
		*/
		virtual bool			OnBanker();
		/*
		*游戏消息
		*/
		virtual bool			OnTimerEvent(DWORD dwID);
		/*
		*更新在线时间
		*/
		virtual bool			UpdateOnline(float fElapsed);
		virtual bool			OnChange(float fElapsed);
	protected:
		//用户叫庄
		virtual bool			OnSubCallBanker(const void * pBuffer, WORD wDataSize);
		//游戏开始
		virtual bool			OnSubGameStart(const void * pBuffer, WORD wDataSize);
		//用户加注
		virtual bool			OnSubAddScore(const void * pBuffer, WORD wDataSize);
		//发牌消息
		virtual bool			OnSubSendCard(const void * pBuffer, WORD wDataSize);
		//用户摊牌
		virtual bool			OnSubOpenCard(const void * pBuffer, WORD wDataSize);
		//用户强退
		virtual bool			OnSubPlayerExit(const void * pBuffer, WORD wDataSize);
		//游戏结束
		virtual bool			OnSubGameEnd(const void * pBuffer, WORD wDataSize);
		
	protected:
		//设置定时器
		virtual bool			SetTimer(DWORD dwID, double fElapsed);
		//删除定时器
		virtual bool			KillTimer(DWORD dwID);
		//更新定时器
		virtual void			UpdateTimer(float fElapsed);
	protected:
		WORD					m_wTableCount;
		WORD					m_wChairCount;
		TimerItemRegister		m_TimerItemActive;
		INT64					m_nTurnMaxScore;
		BYTE					m_byCard[MAX_COUNT];
		WORD					m_wCurBanker;
		BOOL					m_bOffline;
		SDesk*					m_pDesk;
		float					m_fWaitTime;
		int						m_nPlayCount;
		int						m_nMaxPlayCount;
	};

	class OxFactory : public IAndroidFactroy
	{
	public:
		OxFactory(DWORD dwTypeID)
			: IAndroidFactroy(dwTypeID)
		{

		}

		virtual ~OxFactory()
		{

		}

		virtual IAndroid*		Create(const DWORD dwUserID, double fOnlineTime)
		{
			return new Ox(dwUserID, fOnlineTime);
		}
	};
}
