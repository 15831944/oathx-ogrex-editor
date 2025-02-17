#include "StdAfx.h"
#include "Resource.h"
#include "SkinMenu.h"

//////////////////////////////////////////////////////////////////////////////////
//常量定义

//间隙变量
#define SPACE_LEFT					8									//空隙大小
#define SPACE_RIGHT					3									//空隙大小

//大小变量
#define MENU_BAR_CX					0									//标题高度
#define MENU_ITEM_CY				22									//子项高度
#define SEPARATOR_CY				3									//拆分高度

//区域偏移
#define FRAME_OUT_CX                8                                  //框架大小
#define FRAME_OUT_CY                8                                  //框架大小
#define FRAME_IN_CX                 3                                  //框架大小
#define FRAME_IN_CY                 3                                  //框架大小 

//////////////////////////////////////////////////////////////////////////////////

//菜单变量
CMenuImageArray						CSkinMenu::m_MenuItemImage;			//图形子项
CMenuStringArray					CSkinMenu::m_MenuItemString;		//字符子项
CMenuSeparatorArray					CSkinMenu::m_MenuItemSeparator;		//拆分子项

//内核变量
CSkinMenuAttribute					CSkinMenuKernel::m_SkinAttribute;	//菜单属性
HWND								CSkinMenuKernel::m_hWnd=NULL;		//窗口句柄
WNDPROC								CSkinMenuKernel::m_OldWndProc=NULL; //窗口过程 

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CSkinMenuAttribute::CSkinMenuAttribute()
{
	//字体颜色
	m_crNormalText=RGB(0,0,0);	
	m_crSelectText=RGB(0,0,0);	

	//边框颜色
	m_crMenuBar=RGB(0,0,0);	
	m_crSeparator=RGB(0,0,0);	
	m_crNormalBack=RGB(0,0,0);
	m_crSelectBack=RGB(0,0,0);	
	m_crSelectBorder=RGB(0,0,0);

	return;
}

//析构函数
CSkinMenuAttribute::~CSkinMenuAttribute() 
{
}

//配置资源
bool CSkinMenuAttribute::Initialization(CSkinRenderManager * pSkinRenderManager)
{
	//字体颜色
	m_crNormalText=pSkinRenderManager->RenderColor(0.99,0.29);
	m_crSelectText=pSkinRenderManager->RenderColor(1.95,0.75);

	//边框颜色
	m_crMenuBar=pSkinRenderManager->RenderColor(0.35,0.51);
	m_crSeparator=pSkinRenderManager->RenderColor(0.95,0.90);
	m_crNormalBack=pSkinRenderManager->RenderColor(0.00,1.00);
	m_crSelectBack=pSkinRenderManager->RenderColor(0.23,0.88);
	m_crSelectBorder=pSkinRenderManager->RenderColor(0.66,0.87);
	m_crOutFrame = RGB(234,203,60);
	m_crInFrame = RGB(165,113,0);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CSkinMenuKernel::CSkinMenuKernel()
{
	//标识变量
	m_bRemoveBorder = FALSE;
	m_Messagehook = NULL;
}

//析构函数
CSkinMenuKernel::~CSkinMenuKernel()
{
	//卸载钩子
	if(m_bRemoveBorder==TRUE)
	{
		UnhookWindowsHookEx(m_Messagehook);
	}
}

//标识设置
VOID CSkinMenuKernel::SetRemoveBorder(HINSTANCE hInstance, BOOL bRemove) 
{ 
	//设置变量
	m_bRemoveBorder=bRemove; 

	//设置钩子
	if (m_bRemoveBorder==TRUE)
	{
		DWORD dwThreadID = ::GetCurrentThreadId();
		m_Messagehook = SetWindowsHookEx(WH_CALLWNDPROC,CallWndProc,hInstance,dwThreadID);
	}

	return;
}

//窗口过程
LRESULT WINAPI CSkinMenuKernel::CallWndProc(INT code, WPARAM wParam, LPARAM lParam)
{
	//变量定义
	CWPSTRUCT* pStruct = (CWPSTRUCT*)lParam;	

	while (code == HC_ACTION)
	{
		// 筛选消息
		if ( pStruct->message != WM_CREATE)	break;

		//变量定义
		HWND hWnd = pStruct->hwnd;
		TCHAR sClassName[10];
		
		//获取类名
		int nCount = ::GetClassName(hWnd, sClassName, CountArray(sClassName));

		// 检查类名
		if ( nCount != 6 ||  _tcscmp(sClassName, TEXT("#32768")) != 0 ) break;  

		//变量定义
		WNDPROC lastWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);   
		if (lastWndProc != MenuWndProc)   
		{   
			// 替换窗口过程  
			SetWindowLong(hWnd, GWL_WNDPROC, (long)MenuWndProc); 

			//设置变量
			m_hWnd = hWnd;

			// 保存窗口过程   
			m_OldWndProc = lastWndProc;   
		}
		break;   
	}

	//调用钩子
	return CallNextHookEx((HHOOK)WH_CALLWNDPROC, code, wParam, lParam); 
}

// 菜单窗口过程
LRESULT WINAPI CSkinMenuKernel::MenuWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{ 
	//变量定义
	LRESULT lResult;
	switch (message)   
	{      
	case WM_CREATE:  //创建消息
		{   
			//修改样式
			lResult = CallWindowProc(m_OldWndProc, hWnd, message, wParam, lParam);   
			DWORD dwStyle = ::GetWindowLong(hWnd,   GWL_STYLE);   
			DWORD dwNewStyle = (dwStyle & ~WS_BORDER);
			::SetWindowLong(hWnd, GWL_STYLE, dwNewStyle);   
			DWORD dwExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
			DWORD dwNewExStyle = (dwExStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE));   
			::SetWindowLong(hWnd, GWL_EXSTYLE, dwNewExStyle);

			//刷新窗口
			InvalidateRect(hWnd,NULL,TRUE);

			return lResult; 
		}   
	case WM_WINDOWPOSCHANGING:   
		{   
			LPWINDOWPOS lpPos = (LPWINDOWPOS)lParam;  
			lpPos->cx -= 2*GetSystemMetrics(SM_CXBORDER)+4-2*(FRAME_OUT_CX+FRAME_IN_CX);   
			lpPos->cy -= 2*GetSystemMetrics(SM_CYBORDER)+4-2*(FRAME_OUT_CY+FRAME_IN_CY);   
			lResult = CallWindowProc(m_OldWndProc, hWnd, message, wParam, lParam); 
		
			return 0;
		}
	case WM_PRINT: 
		{
			return CallWindowProc( m_OldWndProc, hWnd, WM_PRINTCLIENT, wParam, lParam);
		}
	case WM_GETICON:
		{
			return 0;    
		}
	default:
		{
			return  CallWindowProc( m_OldWndProc, hWnd, message, wParam, lParam);   
		}
	}   
}  

//绘画函数
VOID CSkinMenuKernel::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//效验参数
	ASSERT(lpDrawItemStruct->CtlType==ODT_MENU);
	if (lpDrawItemStruct->CtlType!=ODT_MENU) return;

	//变量定义
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CSkinMenuItem * pSkinMenuItem=(CSkinMenuItem *)lpDrawItemStruct->itemData;

	//自绘框架
	if(m_bRemoveBorder==TRUE)
	{
		//区域偏移
		rcItem.OffsetRect(FRAME_OUT_CX+FRAME_IN_CX,FRAME_OUT_CY+FRAME_IN_CY);

		//变量定义
		CRect rcClient(0,0,0,0);		
		GetClientRect(m_hWnd,rcClient);

		//绘制外框
		pDC->FillSolidRect(0,0,rcClient.Width(),FRAME_OUT_CY,m_SkinAttribute.m_crOutFrame);
		pDC->FillSolidRect(rcClient.right-FRAME_OUT_CX,FRAME_OUT_CY,FRAME_OUT_CX,rcClient.Height()-FRAME_OUT_CY,m_SkinAttribute.m_crOutFrame);
		pDC->FillSolidRect(0,rcClient.Height()-FRAME_OUT_CY,rcClient.Width()-FRAME_OUT_CX,FRAME_OUT_CY,m_SkinAttribute.m_crOutFrame);
		pDC->FillSolidRect(0,FRAME_OUT_CY,FRAME_OUT_CX,rcClient.Height()-FRAME_OUT_CY,m_SkinAttribute.m_crOutFrame);

		//绘制内框
		pDC->FillSolidRect(FRAME_OUT_CX,FRAME_OUT_CY,rcClient.Width()-2*FRAME_OUT_CX,FRAME_IN_CY,m_SkinAttribute.m_crInFrame);
		pDC->FillSolidRect(rcClient.right-FRAME_OUT_CX-FRAME_IN_CX,FRAME_OUT_CY,FRAME_IN_CX,rcClient.Height()-2*FRAME_OUT_CY,m_SkinAttribute.m_crInFrame);
		pDC->FillSolidRect(FRAME_OUT_CX,rcClient.Height()-FRAME_OUT_CY-FRAME_IN_CY,rcClient.Width()-2*FRAME_OUT_CX,FRAME_IN_CY,m_SkinAttribute.m_crInFrame);
		pDC->FillSolidRect(FRAME_OUT_CX,FRAME_OUT_CY,FRAME_IN_CX,rcClient.Height()-2*FRAME_OUT_CY,m_SkinAttribute.m_crInFrame);
	}

	//状态变量
	bool bChecked=((lpDrawItemStruct->itemState&ODS_CHECKED)!=0);
	bool bSelected=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0);
	bool bGrayed=((lpDrawItemStruct->itemState&ODS_DISABLED)||(lpDrawItemStruct->itemState&ODS_GRAYED));

	//绘画背景
	COLORREF crMenuBar=m_SkinAttribute.m_crMenuBar;
	COLORREF crBorder=m_SkinAttribute.m_crSelectBorder;
	COLORREF crNormalBack=m_SkinAttribute.m_crNormalBack;
	pDC->FillSolidRect(rcItem.left,rcItem.top,MENU_BAR_CX,rcItem.Height(),m_SkinAttribute.m_crMenuBar);
	pDC->FillSolidRect(rcItem.left+MENU_BAR_CX,rcItem.top,rcItem.Width()-MENU_BAR_CX,rcItem.Height(),m_SkinAttribute.m_crNormalBack);	

	//焦点边框
	if ((bSelected==true)&&(bGrayed==false)&&(pSkinMenuItem!=NULL)&&(pSkinMenuItem->m_MenuItemType!=MenuItemType_Separator))
	{
		pDC->Draw3dRect(rcItem.left+1,rcItem.top+1,rcItem.Width()-2,rcItem.Height()-2,crBorder,crBorder);
		pDC->FillSolidRect(rcItem.left+2,rcItem.top+2,rcItem.Width()-4,rcItem.Height()-4,m_SkinAttribute.m_crSelectBack);
	}

	//选择标志
	if (bChecked==true)
	{
		//加载资源
		CBitImage ImageMenuFlags;
		ImageMenuFlags.LoadFromResource(GetModuleHandle(SKIN_CONTROL_DLL_NAME),IDB_SKIN_MENU_FLAGS);

		//渲染资源
		CSkinRenderManager * pSkinRenderManager=CSkinRenderManager::GetInstance();
		if (pSkinRenderManager!=NULL) pSkinRenderManager->RenderImage(ImageMenuFlags);

		//绘画界面
		ImageMenuFlags.BitBlt(pDC->m_hDC,rcItem.left+1,rcItem.top+1,ImageMenuFlags.GetWidth()/2,
			ImageMenuFlags.GetHeight(),((bSelected==true)&&(bGrayed==false))?ImageMenuFlags.GetWidth()/2:0,0);
	}

	//其他菜单
	if (pSkinMenuItem==NULL)
	{
		//获取信息
		MENUITEMINFO MenuItemInfo;
		MenuItemInfo.cbSize=sizeof(MenuItemInfo);
		MenuItemInfo.fMask=MIIM_FTYPE|MIIM_BITMAP;
		GetMenuItemInfo(lpDrawItemStruct->itemID,&MenuItemInfo);

		return;
	}

	//界面菜单
	switch (pSkinMenuItem->m_MenuItemType)
	{
	case MenuItemType_Image:		//图形菜单
		{
			//变量定义
			CSkinMenuImage * pSkinMenuImage=(CSkinMenuImage *)pSkinMenuItem;

			//创建 DC
			CDC ImageDC;
			ImageDC.CreateCompatibleDC(pDC);
			ImageDC.SelectObject(pSkinMenuImage->m_hBitmap);

			//获取信息
			BITMAP BitmapInfo;
			GetObject(pSkinMenuImage->m_hBitmap,sizeof(BitmapInfo),&BitmapInfo);

			//绘画位图
			INT nYPos=rcItem.top+3;
			INT nXPos=rcItem.left+MENU_BAR_CX+SPACE_LEFT;
			pDC->BitBlt(nXPos,nYPos,BitmapInfo.bmWidth,BitmapInfo.bmHeight,&ImageDC,0,0,SRCCOPY);

			//释放 DC
			ImageDC.DeleteDC();

			break;
		}
	case MenuItemType_String:		//字符菜单
		{
			//变量定义
			CSkinMenuString * pSkinMenuString=(CSkinMenuString *)pSkinMenuItem;

			//设置颜色
			pDC->SetBkMode(TRANSPARENT);
			pDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

			//设置颜色
			if (bGrayed==true) pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
			else if (bSelected==true) pDC->SetTextColor(m_SkinAttribute.m_crSelectText);
			else pDC->SetTextColor(m_SkinAttribute.m_crNormalText);

			//绘画字符
			CRect rcString;
			rcString.top=rcItem.top;
			rcString.bottom=rcItem.bottom;
			rcString.right=rcItem.right-SPACE_RIGHT;
			rcString.left=rcItem.left+MENU_BAR_CX+SPACE_LEFT;
			pDC->DrawText(pSkinMenuString->m_strString,pSkinMenuString->m_strString.GetLength(),&rcString,DT_SINGLELINE|DT_VCENTER|DT_NOCLIP);

			break;
		}
	case MenuItemType_Separator:	//拆分菜单
		{
			//绘画拆分
			pDC->FillSolidRect(rcItem.left+MENU_BAR_CX+SPACE_LEFT,rcItem.top+1,rcItem.Width()-MENU_BAR_CX-SPACE_LEFT-SPACE_RIGHT,
				rcItem.Height()-2,m_SkinAttribute.m_crSeparator);

			break;
		}
	}

	return;
}

//测量位置
VOID CSkinMenuKernel::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	//效验参数
	ASSERT(lpMeasureItemStruct->CtlType==ODT_MENU);
	if (lpMeasureItemStruct->CtlType!=ODT_MENU) return;

	//获取数据
	CSkinMenuItem * pSkinMenuItem=(CSkinMenuItem *)lpMeasureItemStruct->itemData;
	if (pSkinMenuItem==NULL)
	{
		//获取信息
		MENUITEMINFO MenuItemInfo;
		MenuItemInfo.cbSize=sizeof(MenuItemInfo);
		MenuItemInfo.fMask=MIIM_FTYPE|MIIM_STRING;
		GetMenuItemInfo(lpMeasureItemStruct->itemID,&MenuItemInfo);

		return;
	}

	//界面菜单
	switch (pSkinMenuItem->m_MenuItemType)
	{
	case MenuItemType_Image:		//图形菜单
		{
			//变量定义
			CSkinMenuImage * pSkinMenuImage=(CSkinMenuImage *)pSkinMenuItem;

			//获取信息
			BITMAP BitmapInfo;
			GetObject(pSkinMenuImage->m_hBitmap,sizeof(BitmapInfo),&BitmapInfo);

			//计算大小
			lpMeasureItemStruct->itemHeight=BitmapInfo.bmHeight+6;
			lpMeasureItemStruct->itemWidth=BitmapInfo.bmWidth+MENU_BAR_CX+SPACE_LEFT+SPACE_RIGHT;

			break;
		}
	case MenuItemType_String:		//字符菜单
		{
			//变量定义
			CSkinMenuString * pSkinMenuString=(CSkinMenuString *)pSkinMenuItem;

			//设置 DC
			HDC hDC=GetDC(NULL);
			SelectObject(hDC,CSkinResourceManager::GetInstance()->GetDefaultFont());

			//计算长度
			CSize SizeString;
			GetTextExtentPoint32(hDC,pSkinMenuString->m_strString,pSkinMenuString->m_strString.GetLength(),&SizeString);

			//计算位置
			lpMeasureItemStruct->itemHeight=MENU_ITEM_CY;
			lpMeasureItemStruct->itemWidth=SizeString.cx+MENU_BAR_CX+SPACE_LEFT+SPACE_RIGHT;

			//释放资源
			ReleaseDC(NULL,hDC);

			break;
		}
	case MenuItemType_Separator:	//拆分菜单
		{
			lpMeasureItemStruct->itemWidth=0;
			lpMeasureItemStruct->itemHeight=SEPARATOR_CY;

			break;
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CSkinMenu::CSkinMenu()
{	
}

//析构函数
CSkinMenu::~CSkinMenu()
{
	DestroyMenu();
}

//创建菜单
bool CSkinMenu::CreateMenu()
{
	return m_SkinMenuKernel.CreatePopupMenu()?true:false;
}

//销毁菜单
bool CSkinMenu::DestroyMenu()
{
	//销毁菜单
	m_SkinMenuKernel.DestroyMenu();

	//释放子项
	for (INT_PTR i=0;i<m_MenuItemActive.GetCount();i++)
	{
		FreeMenuItem(m_MenuItemActive[i]);
	}
	m_MenuItemActive.RemoveAll();

	return true;
}

//弹出菜单
bool CSkinMenu::TrackPopupMenu(CWnd * pWnd)
{
	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);

	//弹出菜单
	m_SkinMenuKernel.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,MousePoint.x,MousePoint.y,pWnd);

	return true;
}

//弹出菜单
bool CSkinMenu::TrackPopupMenu(INT nXPos, INT nYPos, CWnd * pWnd)
{
	//弹出菜单
	m_SkinMenuKernel.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,nXPos,nYPos,pWnd,NULL)?true:false;

	return true;
}

//插入拆分
bool CSkinMenu::AppendSeparator()
{
	//获取子项
	CSkinMenuSeparator * pSkinMenuItem=(CSkinMenuSeparator *)AcitveMenuItem(MenuItemType_Separator);

	//插入菜单
	if (pSkinMenuItem!=NULL)
	{
		m_SkinMenuKernel.AppendMenu(MF_OWNERDRAW,0,(LPCTSTR)(pSkinMenuItem));
		return true;
	}

	return false;
}

//插入位图
bool CSkinMenu::AppendMenu(UINT nMenuID, HBITMAP hBitmap, UINT nFlags)
{
	//类型判断
	ASSERT((nFlags&MF_SEPARATOR)==0);
	if ((nFlags&MF_SEPARATOR)!=0) return false;

	//获取子项
	CSkinMenuImage * pSkinMenuItem=(CSkinMenuImage *)AcitveMenuItem(MenuItemType_Image);

	//插入菜单
	if (pSkinMenuItem!=NULL)
	{
		pSkinMenuItem->m_hBitmap=hBitmap;
		m_SkinMenuKernel.AppendMenu(MF_OWNERDRAW|nFlags,nMenuID,(LPCTSTR)(pSkinMenuItem));
		return true;
	}

	return false;
}

//插入字符
bool CSkinMenu::AppendMenu(UINT nMenuID, LPCTSTR pszString, UINT nFlags)
{
	//类型判断
	ASSERT((nFlags&MF_SEPARATOR)==0);
	if ((nFlags&MF_SEPARATOR)!=0) return false;

	//获取子项
	CSkinMenuString * pSkinMenuItem=(CSkinMenuString *)AcitveMenuItem(MenuItemType_String);

	//插入菜单
	if (pSkinMenuItem!=NULL)
	{
		pSkinMenuItem->m_strString=pszString;
		m_SkinMenuKernel.AppendMenu(MF_OWNERDRAW|nFlags,nMenuID,(LPCTSTR)(pSkinMenuItem));
		return true;
	}

	return false;
}

//插入拆分
bool CSkinMenu::InsertSeparator(UINT nPosition)
{
	//获取子项
	CSkinMenuSeparator * pSkinMenuItem=(CSkinMenuSeparator *)AcitveMenuItem(MenuItemType_Separator);

	//插入菜单
	if (pSkinMenuItem!=NULL)
	{
		m_SkinMenuKernel.InsertMenu(nPosition,MF_OWNERDRAW,0,(LPCTSTR)(pSkinMenuItem));
		return true;
	}

	return false;
}

//插入位图
bool CSkinMenu::InsertMenu(UINT nMenuID, HBITMAP hBitmap, UINT nPosition, UINT nFlags)
{
	//类型判断
	ASSERT((nFlags&MF_SEPARATOR)==0);
	if ((nFlags&MF_SEPARATOR)!=0) return false;

	//获取子项
	CSkinMenuImage * pSkinMenuItem=(CSkinMenuImage *)AcitveMenuItem(MenuItemType_Image);

	//插入菜单
	if (pSkinMenuItem!=NULL)
	{
		pSkinMenuItem->m_hBitmap=hBitmap;
		m_SkinMenuKernel.InsertMenu(nPosition,MF_OWNERDRAW|nFlags,nMenuID,(LPCTSTR)(pSkinMenuItem));
		return true;
	}

	return false;
}

//插入字符
bool CSkinMenu::InsertMenu(UINT nMenuID, LPCTSTR pszString, UINT nPosition, UINT nFlags)
{
	//类型判断
	ASSERT((nFlags&MF_SEPARATOR)==0);
	if ((nFlags&MF_SEPARATOR)!=0) return false;

	//获取子项
	CSkinMenuString * pSkinMenuItem=(CSkinMenuString *)AcitveMenuItem(MenuItemType_String);

	//插入菜单
	if (pSkinMenuItem!=NULL)
	{
		pSkinMenuItem->m_strString=pszString;
		m_SkinMenuKernel.InsertMenu(nPosition,MF_OWNERDRAW|nFlags,nMenuID,(LPCTSTR)(pSkinMenuItem));
		return true;
	}

	return false;
}

//删除菜单
bool CSkinMenu::RemoveMenu(UINT nPosition, UINT nFlags)
{
	return m_SkinMenuKernel.RemoveMenu(nPosition,nFlags)?true:false;
}

//修改菜单
bool CSkinMenu::ModifyMenu(UINT nMenuID, HBITMAP hBitmap, UINT nPosition, UINT nFlags)
{
	return 0;
}

//修改菜单
bool CSkinMenu::ModifyMenu(UINT nMenuID, LPCTSTR pszString, UINT nPosition, UINT nFlags)
{
	return 0;
}

//设置默认
bool CSkinMenu::SetDefaultItem(UINT uItem, BOOL fByPos)
{
	return m_SkinMenuKernel.SetDefaultItem(uItem,fByPos)?true:false;
}

//选择控制
UINT CSkinMenu::CheckMenuItem(UINT nMenuID, UINT nCheck)
{
	return m_SkinMenuKernel.CheckMenuItem(nMenuID,nCheck)?true:false;
}

//启用控制
UINT CSkinMenu::EnableMenuItem(UINT nMenuID, UINT nEnable)
{
	return m_SkinMenuKernel.EnableMenuItem(nMenuID,nEnable)?true:false;
}

//边框控制
VOID CSkinMenu::RemoveMenuBorder(HINSTANCE hInstance, BOOL bRemove)
{
	//设置标识
	m_SkinMenuKernel.SetRemoveBorder(hInstance,bRemove);

	return;
}

//释放子项
VOID CSkinMenu::FreeMenuItem(CSkinMenuItem * pSkinMenuItem)
{
	//效验参数
	ASSERT(pSkinMenuItem!=NULL);
	if (pSkinMenuItem==NULL) return;

	//清理变量
	switch (pSkinMenuItem->m_MenuItemType)
	{
	case MenuItemType_Image:		//图形类型
		{
			//变量定义
			CSkinMenuImage * pSkinMenuImage=(CSkinMenuImage *)pSkinMenuItem;

			//设置变量
			pSkinMenuImage->m_hBitmap=NULL;
			m_MenuItemImage.Add(pSkinMenuImage);

			break;
		}
	case MenuItemType_String:		//字符类型
		{
			//变量定义
			CSkinMenuString * pSkinMenuString=(CSkinMenuString *)pSkinMenuItem;

			//设置变量
			pSkinMenuString->m_strString.Empty();
			m_MenuItemString.Add(pSkinMenuString);

			break;
		}
	case MenuItemType_Separator:	//拆分类型
		{
			//变量定义
			CSkinMenuSeparator * pSkinMenuSeparator=(CSkinMenuSeparator *)pSkinMenuItem;

			//设置变量
			m_MenuItemSeparator.Add(pSkinMenuSeparator);

			break;
		}
	}

	return;
}

//获取子项
CSkinMenuItem * CSkinMenu::AcitveMenuItem(enMenuItemType MenuItemType)
{
	//变量定义
	CSkinMenuItem * pSkinMenuItem=NULL;

	//创建子项
	switch (MenuItemType)
	{
	case MenuItemType_Image:		//图形类型
		{
			if (m_MenuItemImage.GetCount()>0)
			{
				INT_PTR nItemCount=m_MenuItemImage.GetCount();
				pSkinMenuItem=m_MenuItemImage[nItemCount-1];
				m_MenuItemImage.RemoveAt(nItemCount-1);
			}
			else
			{
				pSkinMenuItem=new CSkinMenuImage;
				if (pSkinMenuItem==NULL) return NULL;
			}

			break;
		}
	case MenuItemType_String:		//字符类型
		{
			if (m_MenuItemString.GetCount()>0)
			{
				INT_PTR nItemCount=m_MenuItemString.GetCount();
				pSkinMenuItem=m_MenuItemString[nItemCount-1];
				m_MenuItemString.RemoveAt(nItemCount-1);
			}
			else
			{
				pSkinMenuItem=new CSkinMenuString;
				if (pSkinMenuItem==NULL) return NULL;
			}

			break;
		}
	case MenuItemType_Separator:	//拆分类型
		{
			if (m_MenuItemSeparator.GetCount()>0)
			{
				INT_PTR nItemCount=m_MenuItemSeparator.GetCount();
				pSkinMenuItem=m_MenuItemSeparator[nItemCount-1];
				m_MenuItemSeparator.RemoveAt(nItemCount-1);
			}
			else
			{
				pSkinMenuItem=new CSkinMenuSeparator;
				if (pSkinMenuItem==NULL) return NULL;
			}

			break;
		}
	default:
		{
			ASSERT(FALSE);
			return NULL;
		}
	}

	//加入队列
	m_MenuItemActive.Add(pSkinMenuItem);

	return pSkinMenuItem;
}

//////////////////////////////////////////////////////////////////////////////////
