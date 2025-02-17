#pragma once

#include "UserManager.h"

namespace O2
{
#define MAX_REQ_SITDOWNTIEM		5
#define MAX_REQ_SITDOWNCOUNT	8

	//////////////////////////////////////////////////////////////////////////
	// clas android
	//////////////////////////////////////////////////////////////////////////
	class IAndroid : public ITCPSocketSink
	{
	public:
		/*
		* 构造函数
		*/
		IAndroid(DWORD dwUserID, double fOnlineTime);

		/*
		* 析构函数
		*/
		virtual ~IAndroid(void);
		
	public:
		/*
		* 启动机器人
		*/
		virtual BOOL			Startup(const CString& szIPAdress, WORD wPort, const CString& szPwd);
		/*
		* 周期性更新机器人
		*/		
		virtual bool			Update(float fElapsed);
		/*
		* 关闭机器人
		*/		
		virtual void			Shutdown();

	public:
		/*
		* 获取用户ID
		*/
		virtual DWORD			GetUserID() const;

		/*
		* 获取游戏ID
		*/
		virtual DWORD			GetGameID() const;

		/*
		* 获取用户信息
		*/
		virtual SUser*			GetUserInfo() const;

		/*	
		* 设置用户在线时间
		*/
		virtual void			SetOnlineTime(double fOnlineTime);

		/*
		* 获取用户在线时间
		*/
		virtual double			GetOnlineTime() const;
		
		/*
		* 获取用户剩余时间
		*/
		virtual WORD			GetRemaindTime() const;
		/*
		* 设置用户当前状态
		*/
		virtual	void			SetStatus(WORD wStatus);
		/*
		* 获取用户当前状态
		*/		
		virtual WORD			GetStatus() const;
		/*
		* 是否是自己
		*/	
		virtual bool			isSelf(WORD wChairID);

		/*
		* 是否是自己
		*/	
		virtual bool			isSelf(DWORD wChairID);

		/*
		* 取钱
		*/	
		virtual bool			GetScoreFromBanker(INT64 nScore);

		/*
		* 存钱
		*/	
		virtual bool			SaveScoreToBanker(INT64 nScore);
		/*
		* 是否允许下线
		*/	
		virtual bool			CanOffline();

		/*
		* 网络协议发送
		*/		
		virtual bool			SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData=NULL, 
			WORD wDataSize=0);
	public:
		/*
		* 查询接口
		*/
		virtual void*	__cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

		/*
		* 释放对象
		*/
		virtual void	__cdecl Release();

		/*
		* 用户连接
		*/
		virtual bool	__cdecl	OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
		/*
		* 用户断开
		*/
		virtual bool	__cdecl	OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
		/*
		* 网络消息
		*/
		virtual bool	__cdecl	OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, 
			VOID* pBuffer, WORD wDataSize);

	protected:
		/*
		* 登录返回
		*/
		virtual bool			OnSocketMainLogon(WORD wSocketID, CMD_Command Command, 
			VOID* pBuffer, WORD wDataSize);

		/*
		*用户消息
		*/
		virtual bool			OnSocketMainUser(CMD_Command Command, void* pBuffer, 
			WORD wDataSize);

		/*
		*游戏消息
		*/
		virtual bool			OnSocketMainGameFrame(CMD_Command Command, void* pBuffer, 
			WORD wDataSize);
		/*
		*银行消息
		*/
		virtual bool			OnSocketToolBox(CMD_Command Command, void* pBuffer, 
			WORD wDataSize);
		
		/*
		*主通信
		*/
		virtual bool			OnMainSocket(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, 
			WORD wDataSize);
		
		/*
		*游戏消息
		*/
		virtual bool			OnSocket(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, 
			WORD wDataSize);
	protected:
		/*
		* 房价服务信息
		*/
		virtual bool			OnSocketServerInfo(CMD_Command Command, void* pBuffer, 
			WORD wDataSize);

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
		*响应存钱
		*/
		virtual bool			OnBanker();
	protected:
		/*
		* 获取客户端MAC
		*/
		virtual	WORD			GetMACAddress(BYTE cbMacBuffer[], WORD wBufferSize);
		/*
		* 获取客户端序列号
		*/		
		virtual	void			GetClientSerial(tagClientSerial& ClientSerial);
		/*
		* 输出LOG
		*/	
		virtual void			LogEvent(const CString& lpszText, enTraceLevel lv);
	protected:
		// 用户在线时间
		double					m_fOnlineTime;
		double					m_fElapsed;
		double					m_fSitReqTime;
		WORD					m_wSitReqCount;
		DWORD					m_dwUserID;
		CTCPSocketHelper		m_ClientSocket;
		CString					m_Password;
		UserManager*			m_pUserManager;
		BYTE					m_wStaus;
		BYTE					m_bGameStatus;	
	};

	//////////////////////////////////////////////////////////////////////////
	// 机器人创建工厂
	//////////////////////////////////////////////////////////////////////////
	class IAndroidFactroy
	{
	public:
		/*
		* 构造函数
		*/	
		IAndroidFactroy(DWORD dwTypeID)
			: m_dwTypeID(dwTypeID)
		{

		}
		
		/*
		* 析构函数
		*/	
		virtual ~IAndroidFactroy()
		{

		}

		/*
		* 创建机器人
		*/	
		virtual IAndroid*		Create(const DWORD dwUserID, double fOnlineTime) = 0;

		/*
		* 设置工厂类型
		*/	
		virtual void			SetTypeID(DWORD dwTypeID)
		{
			m_dwTypeID = dwTypeID;
		}

		/*
		* 获取工厂类型
		*/	
		virtual DWORD			GetTypeID() const
		{
			return m_dwTypeID;
		}
	protected:
		DWORD					m_dwTypeID;
	};
}

