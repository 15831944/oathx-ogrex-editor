#include "StdAfx.h"
#include "Afxinet.h"
#include "DataBaseSink.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseSink::CDataBaseSink()
{
	//设置变量
	m_pGameUserDBInfo=NULL;
	m_pGameScoreDBInfo=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataBaseSink::~CDataBaseSink()
{
}

//接口查询
void * __cdecl CDataBaseSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//调度模块启动
bool __cdecl CDataBaseSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_GameScoreDBModule.GetInterface()==NULL)&&(m_GameScoreDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//变量定义
		BYTE * pcbAddr=NULL;
		TCHAR szDataBaseAddr[16]=TEXT("");

		//连接用户数据库
		pcbAddr=(BYTE *)&m_pGameUserDBInfo->dwDataBaseAddr;
		_snprintf(szDataBaseAddr,sizeof(szDataBaseAddr),TEXT("%d.%d.%d.%d"),pcbAddr[0],pcbAddr[1],pcbAddr[2],pcbAddr[3]);
		m_AccountsDBModule->SetConnectionInfo(szDataBaseAddr,m_pGameUserDBInfo->wDataBasePort,m_pGameUserDBInfo->szDataBaseName,
			m_pGameUserDBInfo->szDataBaseUser,m_pGameUserDBInfo->szDataBasePass);

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//连接游戏数据库
		pcbAddr=(BYTE *)&m_pGameScoreDBInfo->dwDataBaseAddr;
		_snprintf(szDataBaseAddr,sizeof(szDataBaseAddr),TEXT("%d.%d.%d.%d"),pcbAddr[0],pcbAddr[1],pcbAddr[2],pcbAddr[3]);
		m_GameScoreDBModule->SetConnectionInfo(szDataBaseAddr,m_pGameScoreDBInfo->wDataBasePort,m_pGameScoreDBInfo->szDataBaseName,
			m_pGameScoreDBInfo->szDataBaseUser,m_pGameScoreDBInfo->szDataBasePass);

		//发起连接
		m_GameScoreDBModule->OpenConnection();
		m_GameScoreDBAide.SetDataBase(m_GameScoreDBModule.GetInterface());
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//调度模块关闭
bool __cdecl CDataBaseSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	try
	{
		//关闭连接
		if (m_AccountsDBModule.GetInterface()) m_AccountsDBModule->CloseConnection();
		if (m_GameScoreDBModule.GetInterface()) m_GameScoreDBModule->CloseConnection();

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return false;
}

// 显示存储过程错误的信息 
void CDataBaseSink::ShowDbProcErrorInfo(LPCTSTR pszProc, IDataBaseException * pIException)
{
	if(pIException)
	{
		//错误信息
		CString str;
		str.Format("[%s] %s", pszProc, pIException->GetExceptionDescribe());
		CTraceService::TraceString(str,TraceLevel_Exception);
	}
}

//数据操作处理
bool __cdecl CDataBaseSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GR_LOGON_BY_USERID:		//I D 登录
		{
			return OnRequestLogon(dwContextID,pData,wDataSize);
		}
	case DBR_GR_WRITE_GAME_SCORE:		//写分操作
		{
			return OnRequestWriteUserScore(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LEAVE_GAME_SERVER:		//离开房间
		{
			return OnRequestLeaveGameServer(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LIMIT_ACCOUNTS:			//禁用帐号
		{
			return OnRequestLimitAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_GR_SET_USER_RIGHT:			//设置权限
		{
			return OnRequestSetUserRight(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LOAD_ANDROID:			//加载用户
		{
			return OnRequestLoadAndroid(dwContextID,pData,wDataSize);
		}
	case DBR_GR_SEND_GIFT:				//赠送鲜花
		{
			return OnRequestSendGift(dwContextID,pData,wDataSize);
		}
	case DBR_GR_BUY_PROPERTY:			//道具消息
		{
			return OnRequestProperty(dwContextID,pData,wDataSize);
		}
	case DBR_GR_WRITE_PROPERTY:			//写入道具
		{
			return OnWriteProperty(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LOAD_PROPERTY:			//加载道具
		{
			return OnLoadProperty(dwContextID,pData,wDataSize);
		}
	case DBR_GR_EXCHANGE_CHARM:			//魅力兑换
		{
			return OnExchangeCharm(dwContextID,pData,wDataSize);
		}
	case DBR_GR_CLEARSCORELOCKER:		//清理房间锁定的用户
		{
			return OnClearScoreLocker(dwContextID,pData,wDataSize);
		}
	case DBR_GR_TRANSFER_MONEY:		//转账
		{
			return OnRequestTransferMoney(dwContextID,pData,wDataSize);
		}
	case DBR_GR_BANK_TASK:			//银行操作
		{
			return OnRequestBankTask(dwContextID,pData,wDataSize);
		}
	case DBR_GR_QUERYUSERNAME:
		{
			return OnRequsetQueryUserName(dwContextID,pData,wDataSize);
		}
	case DBR_GR_UPDATEONLINECOUNT:  //统计房间的人数到数据库
		{
			return OnUpdateOnLineCount(dwContextID,pData,wDataSize);
		}
	case DBR_GR_FLASHUSERINFO:
		{
			return OnRequsetFlashUserInfo(dwContextID,pData,wDataSize);
		}
	}

	return false;
}

//登录请求处理
bool CDataBaseSink::OnRequestLogon(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_LogonByUserID));
	if (wDataSize!=sizeof(DBR_GR_LogonByUserID)) return false;

	//登陆处理
	try
	{
		//执行查询
		DBR_GR_LogonByUserID * pLogonByUserID=(DBR_GR_LogonByUserID *)pData;
		LONG lReturnValue=SPLogonByUserID(pLogonByUserID->dwUserID,pLogonByUserID->szPassWord,pLogonByUserID->dwClientIP,pLogonByUserID->szComputerID);

		//登录失败
		if (lReturnValue!=0L)
		{
			DBR_GR_LogonError LogonError;
			LogonError.lErrorCode=lReturnValue;
			m_GameScoreDBAide.GetValue_String(TEXT("ErrorDescribe"),LogonError.szErrorDescribe,CountArray(LogonError.szErrorDescribe));
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_LOGON_ERROR,dwContextID,&LogonError,sizeof(LogonError));
			m_GameScoreDBModule->CloseRecordset();
			return true;
		}

		//变量定义
		DBR_GR_LogonSuccess LogonSuccess;
		memset(&LogonSuccess,0,sizeof(LogonSuccess));

		//读取用户信息
		LogonSuccess.dwUserID=m_GameScoreDBAide.GetValue_DWORD(TEXT("UserID"));
		LogonSuccess.dwCustomFaceVer=m_GameScoreDBAide.GetValue_DWORD(TEXT("CustomFaceVer"));
		LogonSuccess.dwGameID=m_GameScoreDBAide.GetValue_DWORD(TEXT("GameID"));
		LogonSuccess.wFaceID=m_GameScoreDBAide.GetValue_WORD(TEXT("FaceID"));
		LogonSuccess.dwGroupID=m_GameScoreDBAide.GetValue_DWORD(TEXT("GroupID"));
		LogonSuccess.lExperience=m_GameScoreDBAide.GetValue_LONG(TEXT("Experience"));
		LogonSuccess.dwUserRight=m_GameScoreDBAide.GetValue_DWORD(TEXT("UserRight"));
		LogonSuccess.lLoveliness=m_GameScoreDBAide.GetValue_DWORD(TEXT("Loveliness"));
		LogonSuccess.dwMasterRight=m_GameScoreDBAide.GetValue_DWORD(TEXT("MasterRight"));
		m_GameScoreDBAide.GetValue_String(TEXT("Accounts"),LogonSuccess.szAccounts,CountArray(LogonSuccess.szAccounts));
		m_GameScoreDBAide.GetValue_String(TEXT("GroupName"),LogonSuccess.szGroupName,CountArray(LogonSuccess.szGroupName));
		m_GameScoreDBAide.GetValue_String(TEXT("UnderWrite"),LogonSuccess.szUnderWrite,CountArray(LogonSuccess.szUnderWrite));
		m_GameScoreDBAide.GetValue_String(TEXT("BankPassWord"),LogonSuccess.szBankPassWord,CountArray(LogonSuccess.szBankPassWord));

		//用户属性
		LogonSuccess.cbGender=m_GameScoreDBAide.GetValue_UINT(TEXT("Gender"));
		LogonSuccess.cbMemberOrder=m_GameScoreDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		LogonSuccess.cbMasterOrder=m_GameScoreDBAide.GetValue_BYTE(TEXT("MasterOrder"));

		//读取游戏信息
		LogonSuccess.lScore=m_GameScoreDBAide.GetValue_LONGLONG(TEXT("Score"));
		LogonSuccess.lInsureScore=m_GameScoreDBAide.GetValue_LONGLONG(TEXT("InsureScore"));
		//LogonSuccess.lGameGold=m_GameScoreDBAide.GetValue_LONGLONG(TEXT("GameGold"));
		LogonSuccess.lWinCount=m_GameScoreDBAide.GetValue_LONG(TEXT("WinCount"));
		LogonSuccess.lLostCount=m_GameScoreDBAide.GetValue_LONG(TEXT("LostCount"));
		LogonSuccess.lDrawCount=m_GameScoreDBAide.GetValue_LONG(TEXT("DrawCount"));
		LogonSuccess.lFleeCount=m_GameScoreDBAide.GetValue_LONG(TEXT("FleeCount"));

		//附加信息
		CopyMemory(LogonSuccess.szPassWord,pLogonByUserID->szPassWord,sizeof(LogonSuccess.szPassWord));

		//投递调度通知
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_LOGON_SUCCESS,dwContextID,&LogonSuccess,sizeof(LogonSuccess));

		//道具数目
		int nPropertyCount = 0;
		nPropertyCount=m_GameScoreDBAide.GetValue_INT(TEXT("PropCount"));

		//加载道具
		if ( 0 < nPropertyCount )
		{
			lReturnValue = SPLoadUserProperty(pLogonByUserID->dwUserID);

			if ( lReturnValue == 0 )
			{
				DBR_GR_UserProperty UserProperty;
				ZeroMemory(&UserProperty, sizeof(UserProperty));

				UserProperty.dwUserID = LogonSuccess.dwUserID;

				//读取记录
				m_GameScoreDBModule->MoveToFirst();
				while ( m_GameScoreDBModule->IsRecordsetEnd() == false )
				{
					UserProperty.nPropertyID=m_GameScoreDBAide.GetValue_INT(TEXT("CateID"));
					UserProperty.dwCurCount=m_GameScoreDBAide.GetValue_DWORD(TEXT("PropCount"));

					//投递调度通知
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_USER_PROPERTY,dwContextID, &UserProperty, sizeof(UserProperty));

					//移动记录
					m_GameScoreDBModule->MoveToNext();
				}

				//投递调度通知
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_LOAD_PROP_FINISHI,dwContextID, 0, 0);
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe(); 
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//操作失败
		DBR_GR_LogonError LogonError;
		LogonError.lErrorCode=-1;
		lstrcpyn(LogonError.szErrorDescribe,TEXT("由于数据库操作异常，请您稍后重试或选择另一游戏服务器！"),sizeof(LogonError.szErrorDescribe));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_LOGON_ERROR,dwContextID,&LogonError,sizeof(LogonError));
	}

	//关闭记录集)
	m_GameScoreDBModule->CloseRecordset();

	return true;
}

//写分请求
bool CDataBaseSink::OnRequestWriteUserScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_WriteUserScore));
	if (wDataSize!=sizeof(DBR_GR_WriteUserScore)) return false;
	//执行查询
	DBR_GR_WriteUserScore * pWriteUserScore=(DBR_GR_WriteUserScore *)pData;
	try
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pWriteUserScore->dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),pWriteUserScore->dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@lScore"),pWriteUserScore->ScoreModifyInfo.lScore);
		m_GameScoreDBAide.AddParameter(TEXT("@lRevenue"),pWriteUserScore->lRevenue);
		m_GameScoreDBAide.AddParameter(TEXT("@lWinCount"),pWriteUserScore->ScoreModifyInfo.lWinCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lLostCount"),pWriteUserScore->ScoreModifyInfo.lLostCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lDrawCount"),pWriteUserScore->ScoreModifyInfo.lDrawCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lFleeCount"),pWriteUserScore->ScoreModifyInfo.lFleeCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lExperience"),pWriteUserScore->ScoreModifyInfo.lExperience);
		m_GameScoreDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pWriteUserScore->dwPlayTimeCount);
		m_GameScoreDBAide.AddParameter(TEXT("@dwOnLineTimeCount"),pWriteUserScore->dwOnlineTimeCount);
		m_GameScoreDBAide.AddParameter(TEXT("@wKindID"),pWriteUserScore->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),pWriteUserScore->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		m_GameScoreDBAide.AddParameter(TEXT("@dwTableID"),pWriteUserScore->wTableID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwGameRound"),pWriteUserScore->dwGameRound);
		m_GameScoreDBAide.AddParameter(TEXT("@szQuitType"),pWriteUserScore->szQuitType);
		m_GameScoreDBAide.AddParameter(TEXT("@szJetton"),pWriteUserScore->szJetton);
		m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_WriteGameScore"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return true;
}

//离开房间
bool CDataBaseSink::OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LeaveGameServer));
		if (wDataSize!=sizeof(DBR_GR_LeaveGameServer)) return false;

		//执行查询
		DBR_GR_LeaveGameServer * pLeaveGameServer=(DBR_GR_LeaveGameServer *)pData;
		LONG lReturnValue=SPLeaveGameServer(pLeaveGameServer->dwUserID,pLeaveGameServer->dwPlayTimeCount,pLeaveGameServer->dwOnlineTimeCount,
			pLeaveGameServer->dwClientIP,pLeaveGameServer->lRevenue,pLeaveGameServer->lLoveliness,pLeaveGameServer->ScoreModifyInfo);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	return true;
}

//禁用帐户
bool CDataBaseSink::OnRequestLimitAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LimitAccounts));
		if (wDataSize!=sizeof(DBR_GR_LimitAccounts)) return false;

		//执行查询
		DBR_GR_LimitAccounts * pLimitAccounts=(DBR_GR_LimitAccounts *)pData;
		LONG lReturnValue=SPCongealAccounts(pLimitAccounts->dwUserID,pLimitAccounts->dwMasterUserID,pLimitAccounts->dwMasterClientIP);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	return true;
}

//设置权限
bool CDataBaseSink::OnRequestSetUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_SetUserRight));
	if (wDataSize!=sizeof(DBR_GR_SetUserRight)) return false;

	//执行查询
	DBR_GR_SetUserRight * pSetUserRight=(DBR_GR_SetUserRight *)pData;
	LONG lGameFailed = TRUE,lAccountsFailed = TRUE;

	try
	{
		//游戏权限
		if (pSetUserRight->cbGame==TRUE) 
		{
			lGameFailed = SPSetUserGameRight(pSetUserRight->dwUserID,pSetUserRight->dwUserRight,pSetUserRight->dwMasterUserID,pSetUserRight->dwMasterClientIP);
		}

		//帐号权限
		if (pSetUserRight->cbAccounts==TRUE) 
		{
			lAccountsFailed = SPSetUserAccountsRight(pSetUserRight->dwUserID,pSetUserRight->dwUserRight,pSetUserRight->dwMasterUserID,pSetUserRight->dwMasterClientIP);
		}

		//设置消息
		DBR_GR_UserRightResult UserRightResult;
		ZeroMemory(&UserRightResult,sizeof(UserRightResult));

		UserRightResult.dwUserRight = pSetUserRight->dwUserRight;
		UserRightResult.dwUserID = pSetUserRight->dwUserID;
		UserRightResult.dwMasterUserID = pSetUserRight->dwMasterUserID;
		UserRightResult.bGameSuccess=((lGameFailed==FALSE)?true:false);
		UserRightResult.bAccountsSuccess=((lAccountsFailed==FALSE)?true:false);

		//发送信息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_USER_RIGHT_RESULT,dwContextID,&UserRightResult,sizeof( UserRightResult ));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//设置消息
		DBR_GR_UserRightResult UserRightResult;
		ZeroMemory(&UserRightResult,sizeof(UserRightResult));

		UserRightResult.dwUserRight = pSetUserRight->dwUserRight;
		UserRightResult.dwUserID = pSetUserRight->dwUserID;
		UserRightResult.dwMasterUserID = pSetUserRight->dwMasterUserID;
		UserRightResult.bGameSuccess=((lGameFailed==FALSE)?true:false);
		UserRightResult.bAccountsSuccess=((lAccountsFailed==FALSE)?true:false);

		//发送信息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_USER_RIGHT_RESULT,dwContextID,&UserRightResult,sizeof( UserRightResult ));
	}

	return true;
}

//加载用户
bool CDataBaseSink::OnRequestLoadAndroid(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		DBO_GR_AndroidUser AndroidUser;
		ZeroMemory(&AndroidUser,sizeof(AndroidUser));

		//用户帐户
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//执行查询
		AndroidUser.lResultCode=m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_LoadAndroidUser"),true);

		//读取信息
		for (WORD i=0;i<CountArray(AndroidUser.dwAndroidID);i++)
		{
			//结束判断
			if (m_GameScoreDBModule->IsRecordsetEnd()==true) break;

			//读取数据
			DWORD dwAndroidID=0L;
			dwAndroidID=m_GameScoreDBAide.GetValue_DWORD(TEXT("UserID"));
			AndroidUser.dwAndroidID[AndroidUser.wAndroidCount++]=dwAndroidID;

			//移动记录
			m_GameScoreDBModule->MoveToNext();
		}

		//发送信息
		WORD wHeadSize=sizeof(AndroidUser)-sizeof(AndroidUser.dwAndroidID);
		WORD wPacketSize=wHeadSize+AndroidUser.wAndroidCount*sizeof(AndroidUser.dwAndroidID[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_ANDROID_USER,dwContextID,&AndroidUser,wPacketSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//变量定义
		DBO_GR_AndroidUser AndroidUser;
		ZeroMemory(&AndroidUser,sizeof(AndroidUser));

		//构造变量
		AndroidUser.lResultCode=-1;
		AndroidUser.wAndroidCount=0L;

		//发送信息
		WORD wPacketSize=sizeof(AndroidUser)-sizeof(AndroidUser.dwAndroidID);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_ANDROID_USER,dwContextID,&AndroidUser,wPacketSize);
	}

	return false;
}

//赠送鲜花
bool CDataBaseSink::OnRequestSendGift(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_SendGift));
	if (wDataSize!=sizeof(DBR_GR_SendGift)) return false;

	//参数转换
	DBR_GR_SendGift * pSendGift=(DBR_GR_SendGift *)pData;

	try
	{
		//记录赠送
		LONG lRet = SPRecordGiftGrant(pSendGift->dwSendUserID, pSendGift->dwRcvUserID, pSendGift->wGiftID, pSendGift->dwClientIP,
			pSendGift->lSendUserCharm, pSendGift->lRcvUserCharm, pSendGift->lFlowerPrice, pSendGift->wFlowerCount);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	return true;
}

//道具消息
bool CDataBaseSink::OnRequestProperty(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数转换
	DBR_GR_Property * pProperty=(DBR_GR_Property *)pData;

	try
	{
		//记录赠送
		SPBuyProperty(pProperty->dwUserID, pProperty->dwTargetUserID, pProperty->nPropertyID, pProperty->dwCurCount, 
			pProperty->dwOnceCount, pProperty->dwPachurseCount, pProperty->lPrice, pProperty->dwClientIP);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	return true;
}

//写入道具
bool CDataBaseSink::OnWriteProperty(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_WriteProperty));
	if (wDataSize!=sizeof(DBR_GR_WriteProperty)) return false;

	//参数转换
	DBR_GR_WriteProperty * pWriteProperty=(DBR_GR_WriteProperty *)pData;

	try
	{
		//写入道具
		SPWriteProperty(pWriteProperty->dwUserID, pWriteProperty->nPropertyID, pWriteProperty->lUseableTime, pWriteProperty->dwClientIP);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	return true;
}

//加载道具
bool CDataBaseSink::OnLoadProperty(DWORD dwContextID, VOID * pData, WORD wDataSize)
{	
	try
	{
		LONG lReturn = -1;

		//加载道具
		lReturn = SPLoadPropertyAttribute();
		if ( lReturn > 0 )
		{
			DBR_GR_PropertyAttribute PropertyAttribute;
			ZeroMemory(&PropertyAttribute, sizeof(PropertyAttribute));

			//读取记录
			m_GameScoreDBModule->MoveToFirst();
			while ( m_GameScoreDBModule->IsRecordsetEnd() == false )
			{
				PropertyAttribute.wPropertyID=m_GameScoreDBAide.GetValue_WORD(TEXT("CateID"));
				PropertyAttribute.dwPropCount1=m_GameScoreDBAide.GetValue_DWORD(TEXT("PropCount1"));
				PropertyAttribute.dwPropCount2=m_GameScoreDBAide.GetValue_DWORD(TEXT("PropCount2"));
				PropertyAttribute.dwPropCount3=m_GameScoreDBAide.GetValue_DWORD(TEXT("PropCount3"));
				PropertyAttribute.dwPropCount4=m_GameScoreDBAide.GetValue_DWORD(TEXT("PropCount4"));
				PropertyAttribute.dwPropCount5=m_GameScoreDBAide.GetValue_DWORD(TEXT("PropCount5"));
				PropertyAttribute.dwPropCount6=m_GameScoreDBAide.GetValue_DWORD(TEXT("PropCount6"));
				PropertyAttribute.dwPropCount7=m_GameScoreDBAide.GetValue_DWORD(TEXT("PropCount7"));
				PropertyAttribute.dwPropCount8=m_GameScoreDBAide.GetValue_DWORD(TEXT("PropCount8"));
				PropertyAttribute.dwPropCount9=m_GameScoreDBAide.GetValue_DWORD(TEXT("PropCount9"));
				PropertyAttribute.dwPropCount10=m_GameScoreDBAide.GetValue_DWORD(TEXT("PropCount10"));

				PropertyAttribute.lPrice1=m_GameScoreDBAide.GetValue_LONG(TEXT("Price1"));
				PropertyAttribute.lPrice2=m_GameScoreDBAide.GetValue_LONG(TEXT("Price2"));
				PropertyAttribute.lPrice3=m_GameScoreDBAide.GetValue_LONG(TEXT("Price3"));
				PropertyAttribute.lPrice4=m_GameScoreDBAide.GetValue_LONG(TEXT("Price4"));
				PropertyAttribute.lPrice5=m_GameScoreDBAide.GetValue_LONG(TEXT("Price5"));
				PropertyAttribute.lPrice6=m_GameScoreDBAide.GetValue_LONG(TEXT("Price6"));
				PropertyAttribute.lPrice7=m_GameScoreDBAide.GetValue_LONG(TEXT("Price7"));
				PropertyAttribute.lPrice8=m_GameScoreDBAide.GetValue_LONG(TEXT("Price8"));
				PropertyAttribute.lPrice9=m_GameScoreDBAide.GetValue_LONG(TEXT("Price9"));
				PropertyAttribute.lPrice10=m_GameScoreDBAide.GetValue_LONG(TEXT("Price10"));

				PropertyAttribute.cbDiscount=m_GameScoreDBAide.GetValue_BYTE(TEXT("Discount"));

				//投递调度通知
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_PROPERTY_ATTRIBUTE,dwContextID,&PropertyAttribute, sizeof(PropertyAttribute));

				//移动记录
				m_GameScoreDBModule->MoveToNext();
			}
		}

		//加载鲜花
		lReturn = SPLoadFlowerAttribute();
		if ( lReturn > 0 )
		{
			//变量定义
			DBR_GR_FlowerAttribute FlowerAttribute;
			ZeroMemory(&FlowerAttribute, sizeof(FlowerAttribute));

			//读取记录
			m_GameScoreDBModule->MoveToFirst();
			while ( m_GameScoreDBModule->IsRecordsetEnd() == false )
			{
				FlowerAttribute.nFlowerID=m_GameScoreDBAide.GetValue_INT(TEXT("ID"));
				FlowerAttribute.lSendUserCharm=m_GameScoreDBAide.GetValue_LONG(TEXT("SendUserCharm"));
				FlowerAttribute.lRcvUserCharm=m_GameScoreDBAide.GetValue_LONG(TEXT("RcvUserCharm"));
				FlowerAttribute.lPrice=m_GameScoreDBAide.GetValue_LONG(TEXT("Price"));
				FlowerAttribute.cbDiscount=m_GameScoreDBAide.GetValue_BYTE(TEXT("Discount"));

				//投递调度通知
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_FLOWER_ATTRIBUTE,dwContextID,&FlowerAttribute, sizeof(FlowerAttribute));

				//移动记录
				m_GameScoreDBModule->MoveToNext();
			}
		}

	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	return true;
}

//魅力兑换
bool CDataBaseSink::OnExchangeCharm(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_ExchangeLoveliness));
	if (wDataSize!=sizeof(DBR_GR_ExchangeLoveliness)) return false;

	//参数转换
	DBR_GR_ExchangeLoveliness * pExchangeLoveliness=(DBR_GR_ExchangeLoveliness *)pData;

	LONG lReturnValue = -1;
	try
	{
		//魅力兑换
		lReturnValue = SPExchangeCharm(pExchangeLoveliness->dwUserID, pExchangeLoveliness->lLoveliness, 
			pExchangeLoveliness->lGoldValue, pExchangeLoveliness->dwClientIP);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	////交换结果
	//DBR_GR_ExchangeCharmResult ExchangeCharmResult;
	//ZeroMemory(&ExchangeCharmResult, sizeof(ExchangeCharmResult));
	//ExchangeCharmResult.cbResultCode = BYTE(lReturnValue);
	//ExchangeCharmResult.dwUserID = pExchangeLoveliness->dwUserID;
	//ExchangeCharmResult.lLoveliness = pExchangeLoveliness->lLoveliness;
	//ExchangeCharmResult.lGoldValue = pExchangeLoveliness->lGoldValue;

	////投递调度通知
	//m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_EXCHANGE_RESULT,dwContextID, &ExchangeCharmResult, sizeof(ExchangeCharmResult));

	return true;
}

//I D 存储过程
LONG CDataBaseSink::SPLogonByUserID(DWORD dwUserID, LPCTSTR pszPassword, DWORD dwClientIP, LPCTSTR pszComputerID)
{
	LONG lReturnValue=-1;
	try
	{
		//效验参数
		ASSERT(dwUserID!=0L);
		ASSERT(pszPassword!=NULL);

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@strPassword"),pszPassword);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		m_GameScoreDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);
		m_GameScoreDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyUserID"),true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}


//离开存储过程
LONG CDataBaseSink::SPLeaveGameServer(DWORD dwUserID, DWORD dwPlayTimeCount,
									  DWORD dwOnLineTimeCount, DWORD dwClientIP,
									  __int64 lRevenue, LONG lLoveliness,
									  tagUserScore & UserScore)
{
	LONG lReturnValue=-1;
	try
	{

		//效验参数
		ASSERT(dwUserID!=0L);

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@lScore"),UserScore.lScore);
		m_GameScoreDBAide.AddParameter(TEXT("@lGameGold"),UserScore.lScore);
		m_GameScoreDBAide.AddParameter(TEXT("@lInsureScore"),UserScore.lInsureScore);
		m_GameScoreDBAide.AddParameter(TEXT("@lLoveliness"),lLoveliness);
		m_GameScoreDBAide.AddParameter(TEXT("@lRevenue"),lRevenue);
		m_GameScoreDBAide.AddParameter(TEXT("@lWinCount"),UserScore.lWinCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lLostCount"),UserScore.lLostCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lDrawCount"),UserScore.lDrawCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lFleeCount"),UserScore.lFleeCount);
		m_GameScoreDBAide.AddParameter(TEXT("@lExperience"),UserScore.lExperience);
		m_GameScoreDBAide.AddParameter(TEXT("@dwPlayTimeCount"),dwPlayTimeCount);
		m_GameScoreDBAide.AddParameter(TEXT("@dwOnLineTimeCount"),dwOnLineTimeCount);
		m_GameScoreDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_LeaveGameServer"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}

//禁号存储过程
LONG CDataBaseSink::SPCongealAccounts(DWORD dwUserID, DWORD dwMasterUserID, DWORD dwClientIP)
{
	LONG lReturnValue=-1;
	try
	{
		//效验参数
		ASSERT(dwUserID!=0L);

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMasterUserID"),dwMasterUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		lReturnValue= m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_CongealAccounts"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}

//权限存储过程
LONG CDataBaseSink::SPSetUserGameRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP)
{
	LONG lReturnValue=-1;
	try
	{
		//效验参数
		ASSERT(dwUserID!=0L);

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserRight"),dwUserRight);
		m_GameScoreDBAide.AddParameter(TEXT("@dwMasterUserID"),dwMasterUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_SetUserRight"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}

//权限存储过程
LONG CDataBaseSink::SPSetUserAccountsRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP)
{
	LONG lReturnValue=-1;
	try
	{

		//效验参数
		ASSERT(dwUserID!=0L);

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwUserRight"),dwUserRight);
		m_AccountsDBAide.AddParameter(TEXT("@dwMasterUserID"),dwMasterUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		lReturnValue= m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_SetUserRight"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}

//记录赠送
LONG CDataBaseSink::SPRecordGiftGrant(DWORD dwSendUserID, DWORD dwRcvUserID, WORD wGiftID, DWORD dwClientIP, 
									  DWORD dwSendUserLoveliness, DWORD dwRcvUserLoveliness, LONG lValue, int nFlowerCount)
{
	LONG lReturnValue=-1;
	try
	{
		ASSERT( dwSendUserID != 0 && dwRcvUserID != 0 );

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwSendUserID"),dwSendUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwRcvUserID"),dwRcvUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwSendUserLoveliness"),dwSendUserLoveliness);
		m_GameScoreDBAide.AddParameter(TEXT("@dwRcvUserLoveliness"),dwRcvUserLoveliness);
		m_GameScoreDBAide.AddParameter(TEXT("@dwFlowerCount"),(DWORD)nFlowerCount);
		m_GameScoreDBAide.AddParameter(TEXT("@strFlowerName"),g_FlowerDescribe[wGiftID].szName);
		m_GameScoreDBAide.AddParameter(TEXT("@lFlowerPay"),lValue);
		m_GameScoreDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_RecordFlowerGrant"),true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}

//购买道具
LONG CDataBaseSink::SPBuyProperty(DWORD dwUserID, DWORD dwTargetUserID, int nPropertyID, DWORD dwCurCount, DWORD dwOnceCount, 
								  DWORD dwPachurCount, LONG lPrice, DWORD dwClientIP)
{
	LONG lReturnValue=-1;
	try
	{
		ASSERT( dwUserID != 0 );

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwRcvUserID"),dwTargetUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),(DWORD)m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwCateID"),(DWORD)nPropertyID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwCurCount"),dwCurCount);	
		m_GameScoreDBAide.AddParameter(TEXT("@dwOnceCount"),dwOnceCount);	
		m_GameScoreDBAide.AddParameter(TEXT("@dwPachurseCount"),dwPachurCount);
		m_GameScoreDBAide.AddParameter(TEXT("@bwSpendScore"),lPrice);	
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_PurchaseProp"),true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}

//写入道具
LONG CDataBaseSink::SPWriteProperty(DWORD dwUserID, int nPropertyID, DWORD lUseableTime, DWORD dwClientIP)
{
	LONG lReturnValue=-1;
	try
	{
		ASSERT( dwUserID != 0 );

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

		//喇叭判断
		if ( nPropertyID == PROP_BUGLE )
			m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),0);
		else
			m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),m_pGameServiceAttrib->wKindID);

		m_GameScoreDBAide.AddParameter(TEXT("@dwCateID"),nPropertyID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwPropCount"),lUseableTime);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_WriteUserProp"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}

//加载道具
LONG CDataBaseSink::SPLoadPropertyAttribute()
{
	LONG lReturnValue=-1;
	try
	{
		//执行存储过程
		m_GameScoreDBAide.ResetParameter();

		lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_LoadGameShopCate"),true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}

//加载鲜花
LONG CDataBaseSink::SPLoadFlowerAttribute()
{
	LONG lReturnValue=-1;
	try
	{
		//执行存储过程
		m_GameScoreDBAide.ResetParameter();

		lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_LoadFlowerCate"),true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}

//加载道具
LONG CDataBaseSink::SPLoadUserProperty(DWORD dwUserID)
{
	LONG lReturnValue=-1;
	try
	{
		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),m_pGameServiceAttrib->wKindID);

		lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_LoadUserProp"),true);

	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}
//魅力兑换
LONG CDataBaseSink::SPExchangeCharm(DWORD dwUserID, LONG lLoveliness, LONG lGoldValue, DWORD dwClientIP)
{
	LONG lReturnValue=-1;
	try
	{
		ASSERT( dwUserID != 0 );

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@lLoveliness"),lLoveliness);
		m_GameScoreDBAide.AddParameter(TEXT("@lInsureScore"),lGoldValue);
		m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
	    lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_ExchangeLoveliness"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}

//提取存储过程
LONG CDataBaseSink::SPBankDrawoutGold(DWORD dwUserID, DWORD lSwapScore, DWORD dwClientIP)
{
	LONG lReturnValue=-1;
	try
	{
		ASSERT( dwUserID != 0 );

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@lSwapScore"),lSwapScore);
		m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_DrawoutGameGold"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}


//存金存储过程
LONG CDataBaseSink::SPBankStorageGold(DWORD dwUserID, DWORD lSwapScore, DWORD dwClientIP)
{
	LONG lReturnValue=-1;
	try
	{
		ASSERT( dwUserID != 0 );

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&dwClientIP;
		_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//执行存储过程
		m_GameScoreDBAide.ResetParameter();
		m_GameScoreDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
		m_GameScoreDBAide.AddParameter(TEXT("@lSwapScore"),lSwapScore);
		m_GameScoreDBAide.AddParameter(TEXT("@dwKindID"),m_pGameServiceAttrib->wKindID);
		m_GameScoreDBAide.AddParameter(TEXT("@dwServerID"),m_pGameServiceOption->wServerID);
		m_GameScoreDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		lReturnValue= m_GameScoreDBAide.ExecuteProcess(TEXT("GSP_GR_SavingGameGold"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}
	return lReturnValue;
}


//清理房间锁定的用户
bool CDataBaseSink::OnClearScoreLocker(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==0);
	if (wDataSize!=0)
		return false;
	LPCTSTR lpszProc = _T("GSP_GR_ClearScoreLocker");
	TRY_BEGIN()
	//执行存储过程
	m_GameScoreDBAide.ResetParameter();
	m_GameScoreDBAide.AddParameter(TEXT("@dwServerID"),m_pGameServiceOption->wServerID);
	m_GameScoreDBAide.ExecuteProcess(lpszProc,false);
	return true;
	CATCH_ADO_SHOW(lpszProc);
	return true;
}

//转账
bool CDataBaseSink::OnRequestTransferMoney(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	ASSERT(wDataSize==sizeof(DBR_GR_TransferMoney));
	if (wDataSize!=sizeof(DBR_GR_TransferMoney))
		return false;
	//执行查询
	DBR_GR_TransferMoney * pTransferMoney=(DBR_GR_TransferMoney *)pData;
	LPCTSTR lpszProc = _T("GSP_GR_TransferMoney");
	TRY_BEGIN()
	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&pTransferMoney->dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行查询
	m_GameScoreDBModule->ClearParameters();
	m_GameScoreDBModule->AddParameter(TEXT("RETURN_VALUE"),adInteger,adParamReturnValue,sizeof(long),_variant_t((long)0));
	m_GameScoreDBModule->AddParameter(TEXT("@dwUserID_Out"),adInteger,adParamInput,sizeof(long),_variant_t((long)pTransferMoney->dwUserID));
	m_GameScoreDBModule->AddParameter(TEXT("@dwGameID_In"),adInteger,adParamInput,sizeof(long),_variant_t((long)pTransferMoney->dwGameID_IN));
	m_GameScoreDBModule->AddParameter(TEXT("@Account_Out"),adVarChar, adParamInput,lstrlen(pTransferMoney->szAccount_Out),_variant_t(pTransferMoney->szAccount_Out));
	m_GameScoreDBModule->AddParameter(TEXT("@Account_In"),adVarChar,adParamInput, lstrlen(pTransferMoney->szAccount_In),_variant_t(pTransferMoney->szAccount_In));
	m_GameScoreDBModule->AddParameter(TEXT("@MoneyNumber"),adBigInt,adParamInput,sizeof(__int64),_variant_t(pTransferMoney->sfMoneyNumber));
	m_GameScoreDBModule->AddParameter(TEXT("@strClientIP"),adVarChar,adParamInput,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameScoreDBModule->ExecuteProcess(lpszProc,true);
	//结果判断
	LONG lReturnCode=m_GameScoreDBModule->GetReturnValue();
	pTransferMoney->lErrorCode=lReturnCode;
	if ( lReturnCode!=0L )
	{
		TCHAR szErrorDescribe[256]=TEXT("");
		m_GameScoreDBModule->GetFieldValue(TEXT("ErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe));
		CTraceService::TraceString(szErrorDescribe,TraceLevel_Exception);
		lstrcpyn(pTransferMoney->szErrorDescribe, szErrorDescribe, CountArray(pTransferMoney->szErrorDescribe));
	}
	else
	{
		m_GameScoreDBModule->GetFieldValue(TEXT("MoneyNumber"), pTransferMoney->sfMoneyNumber);
		m_GameScoreDBModule->GetFieldValue(TEXT("Score_Out"), pTransferMoney->sfLeftMoney);
		m_GameScoreDBModule->GetFieldValue(TEXT("Tax"), pTransferMoney->sfTax);
		m_GameScoreDBModule->GetFieldValue(TEXT("UserID_IN"), pTransferMoney->lUserID_IN);
	}

	//发送信息
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_TRANSFER_MONEY,dwContextID,
		pTransferMoney,sizeof( DBR_GR_TransferMoney ));
	return true;
	CATCH_ADO_SHOW(lpszProc);
	return true;
}

//银行操作
bool CDataBaseSink::OnRequestBankTask(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_BankTask));
	if (wDataSize!=sizeof(DBR_GR_BankTask)) return false;
	//执行查询
	DBR_GR_BankTask * pBankTask=(DBR_GR_BankTask *)pData;
	LPCTSTR lpszProc = _T("GSP_GR_Bank");
	TRY_BEGIN()
	//执行存储过程
	m_GameScoreDBModule->ClearParameters();
	m_GameScoreDBModule->AddParameter(TEXT("RETURN_VALUE"),adInteger,adParamReturnValue,sizeof(long),_variant_t((long)0));
	m_GameScoreDBModule->AddParameter(TEXT("@dwUserID"),adInteger,adParamInput,sizeof(long),_variant_t((long)pBankTask->dwUserID));
	m_GameScoreDBModule->AddParameter(TEXT("@Password"), adChar, adParamInput,PASS_LEN, _variant_t(pBankTask->szPassword));
	m_GameScoreDBModule->AddParameter(TEXT("@BankTask"),adInteger,adParamInput,sizeof(long),_variant_t((long)pBankTask->lBankTask));
	m_GameScoreDBModule->AddParameter(TEXT("@MoneyNumber"),adBigInt,adParamInput,sizeof(__int64),_variant_t(pBankTask->lMoneyNumber));
	m_GameScoreDBModule->ExecuteProcess(lpszProc,true);
	LONG lReturn=m_GameScoreDBModule->GetReturnValue();
	pBankTask->lErrorCode=lReturn;
	if ( lReturn==0 )
	{
		m_GameScoreDBModule->GetFieldValue(TEXT("MoneyNumber"), pBankTask->lMoneyNumber);
		m_GameScoreDBModule->GetFieldValue(TEXT("NewScore"), pBankTask->lNewScore);
		m_GameScoreDBModule->GetFieldValue(TEXT("MoneyInBank"), pBankTask->lMoneyInBank);
	}
	else
	{
		m_GameScoreDBModule->GetFieldValue(TEXT("ErrorDescribe"), pBankTask->szErrorDescribe, sizeof(pBankTask->szErrorDescribe) );
	}
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_BANK_TASK,
		dwContextID , pBankTask, sizeof(DBR_GR_BankTask));
	return true;
	CATCH_ADO_SHOW(lpszProc);
	return true;
}

//查询用户名
bool CDataBaseSink::OnRequsetQueryUserName(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_Query_UserName));
	if (wDataSize!=sizeof(DBR_GR_Query_UserName)) return false;
	//执行查询
	DBR_GR_Query_UserName * QuserName=(DBR_GR_Query_UserName *)pData;
	LPCTSTR lpszProc = _T("GSP_GP_QueryUserName");
	TRY_BEGIN()
	//执行存储过程
	m_AccountsDBModule->ClearParameters();
	m_AccountsDBModule->AddParameter(TEXT("RETURN_VALUE"),adInteger,adParamReturnValue,sizeof(long),_variant_t((long)0));
	m_AccountsDBModule->AddParameter(TEXT("@dwGameID"),adInteger,adParamInput,sizeof(long),_variant_t((long)QuserName->lGameID));
	m_AccountsDBModule->ExecuteProcess(lpszProc,true);
	LONG lReturn=m_AccountsDBModule->GetReturnValue();
	QuserName->lErrorCode=lReturn;
	if ( lReturn==0 )
	{
		m_AccountsDBModule->GetFieldValue(TEXT("UserName"), QuserName->szUserName,sizeof(QuserName->szUserName));
	}
	else
	{
		m_AccountsDBModule->GetFieldValue(TEXT("@ErrorDescribe"), QuserName->szErrorDescribe, sizeof(QuserName->szErrorDescribe) );
	}
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_QUERYUSERNAME,
		dwContextID , QuserName, sizeof(DBR_GR_Query_UserName));
	return true;
	CATCH_ADO_SHOW(lpszProc);
	return true;
}

//更新房间人数
bool CDataBaseSink::OnUpdateOnLineCount(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	ASSERT(wDataSize == sizeof(DBR_GR_UpdateOnLineCount));
	if (wDataSize!=sizeof(DBR_GR_UpdateOnLineCount)) 
		return false;
	DBR_GR_UpdateOnLineCount* Count = (DBR_GR_UpdateOnLineCount*)pData;
	LPCTSTR lpszProc = _T("GSP_GP_UpdateOnLineCount");
	TRY_BEGIN()
	m_AccountsDBModule->ClearParameters();
	m_AccountsDBModule->AddParameter(TEXT("@GameID"),adInteger,adParamInput,sizeof(int),_variant_t((int)Count->lGameID));
	m_AccountsDBModule->AddParameter(TEXT("@RoomID"),adInteger,adParamInput,sizeof(int),_variant_t((long)Count->lRoomID));
	m_AccountsDBModule->AddParameter(TEXT("@Count"),adInteger,adParamInput,sizeof(int),_variant_t((long)Count->lCount));
	m_AccountsDBModule->ExecuteProcess(lpszProc,true);
	return true;
	CATCH_ADO_SHOW(lpszProc);
	return true;
}

//处理刷新用户信息
bool CDataBaseSink::OnRequsetFlashUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	ASSERT(wDataSize == sizeof(DBR_GR_FlashUserInfo));
	if (wDataSize!=sizeof(DBR_GR_FlashUserInfo)) 
		return false;
	DBR_GR_FlashUserInfo* info = (DBR_GR_FlashUserInfo*)pData;
	LPCTSTR lpszProc = _T("GSP_GP_FlashUserInfo");
	TRY_BEGIN()
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),info->lUserID);
	m_AccountsDBAide.ExecuteProcess(lpszProc,true);
	DBR_GR_FlashUserInfo_Ret infoRet;
	ZeroMemory(&infoRet,sizeof(infoRet));
	//读取用户信息
	infoRet.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
	infoRet.dwCustomFaceVer=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomFaceVer"));
	infoRet.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
	infoRet.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
	infoRet.dwGroupID=m_AccountsDBAide.GetValue_DWORD(TEXT("GroupID"));
	infoRet.lExperience=m_AccountsDBAide.GetValue_LONG(TEXT("Experience"));
	infoRet.dwUserRight=m_AccountsDBAide.GetValue_DWORD(TEXT("UserRight"));
	infoRet.lLoveliness=m_AccountsDBAide.GetValue_DWORD(TEXT("Loveliness"));
	infoRet.dwMasterRight=m_AccountsDBAide.GetValue_DWORD(TEXT("MasterRight"));
	infoRet.cbGender=m_AccountsDBAide.GetValue_UINT(TEXT("Gender"));
	infoRet.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
	infoRet.cbMasterOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MasterOrder"));
	m_AccountsDBAide.GetValue_String(TEXT("Accounts"),infoRet.szAccounts,CountArray(infoRet.szAccounts));
	m_AccountsDBAide.GetValue_String(TEXT("GroupName"),infoRet.szGroupName,CountArray(infoRet.szGroupName));
	m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),infoRet.szUnderWrite,CountArray(infoRet.szUnderWrite));
	//读取游戏信息
	infoRet.lScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
	infoRet.lInsureScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));
	//LogonSuccess.lGameGold=m_AccountsDBModule.GetValue_LONGLONG(TEXT("GameGold"));
	infoRet.lWinCount=m_AccountsDBAide.GetValue_LONG(TEXT("WinCount"));
	infoRet.lLostCount=m_AccountsDBAide.GetValue_LONG(TEXT("LostCount"));
	infoRet.lDrawCount=m_AccountsDBAide.GetValue_LONG(TEXT("DrawCount"));
	infoRet.lFleeCount=m_AccountsDBAide.GetValue_LONG(TEXT("FleeCount"));

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_FLASHUSERINFO,
		dwContextID , &infoRet, sizeof(infoRet));
	return true;
	CATCH_ADO_SHOW(lpszProc);
	return true;
}

//////////////////////////////////////////////////////////////////////////
