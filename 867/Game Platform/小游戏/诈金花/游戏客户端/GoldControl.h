#ifndef GOLD_CONTROL_HEAD_FILE
#define GOLD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//金币控制
class CGoldControl : public CWnd
{
	//变量定义
protected:
	__int64								m_lAllGold;						//总币数目
	__int64								m_lGoldCount[3];				//金币数目
	__int64								m_lMaxGold;						//最大金币
	__int64								m_lGoldCell[7];					//单元金币
	__int64								m_lMinGold;						//最少金币
	CPoint								m_BasicPoint;					//基础位置

	//位置变量
protected:
	int									m_nWidth;						//控件宽度
	int									m_nCellCount;					//单元数目
	int									m_AppendWidth;					//附加宽度

	//资源变量
protected:
	CSkinImage							m_ImageBack;					//背景资源
	CSkinImage							m_ImageMoney;					//金币资源
	CSkinImage							m_ImageNumber;					//数字资源

	//函数定义
public:
	//构造函数
	CGoldControl();
	//析构函数
	virtual ~CGoldControl();

	//功能函数
public:
	//获取金币
	__int64 GetGold(){return m_lAllGold;};
	//设置筹码
	void SetGoldCount(__int64 lCellSocre);
	//设置金币
	void SetGold(__int64 lGold);
	//设置限注
	void SetMaxGold(__int64 lMaxGold);
	//限制单元
	void SetMinGold(__int64 lMinGold);
	//设置位置
	void SetBasicPoint(int nXPos, int nYPos);

	//内部函数
private:
	//调整控件
	void RectifyControl();

	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();
	//左键按下消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//设置光标
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

#endif