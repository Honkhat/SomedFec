#include "stdafx.h"
#include "CEFWebkitBrowser.h"

// WKEWebkitBrowser.cpp : ����Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "CEFWebkitBrowser.h"
#include <strsafe.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

#define 	E_GOFORWORD_TIMER  100
CEFWebkitBrowserWnd* CEFWebkitBrowserWnd::pCEFWebkitBrowserWnd = NULL;

CEFWebkitBrowserWnd::CEFWebkitBrowserWnd()
{
	pURLEditCtrl_ = NULL;
	pSearchEditCtrl_ = NULL;
	pWebStateCtrl_ = NULL;
	pWKEWebkitCtrl_ = NULL;
	pWebTabContainer_ = NULL;

	pCEFWebkitBrowserWnd = this;

}

CEFWebkitBrowserWnd::~CEFWebkitBrowserWnd()
{

}


CControlUI* CEFWebkitBrowserWnd::CreateControl(LPCTSTR pstrClassName)
{
	if (_tcsicmp(pstrClassName, _T("CEFWebkitBrowser")) == 0)
	{
		return  (pWKEWebkitCtrl_ = new CCEFWebkitUI(GetSafeHwnd()));
	}

	return NULL;
}

void CEFWebkitBrowserWnd::OnFinalMessage(HWND hWnd)
{


	__super::OnFinalMessage(hWnd);
}

LRESULT CEFWebkitBrowserWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostQuitMessage(0L);
	return __super::OnDestroy(uMsg, wParam, lParam, bHandled);
}

//��ȡWM_CLOSE��Ϣ
LRESULT CEFWebkitBrowserWnd::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{

	if ((pWKEWebkitCtrl_ != NULL) && (!pWKEWebkitCtrl_->IsClosed()))
	{
		//pWKEWebkitCtrl_->CloseAllPage();
		bHandled = TRUE;
		//  Cancel the close.

	}
	else
	{
		bHandled = TRUE;
	}

	return 0;
}

void CEFWebkitBrowserWnd::OnClick(TNotifyUI & msg)
{
	CDuiString strName = msg.pSender->GetName();

	if (strName.Find(_T("webtab")) != -1)
	{
		COptionUI* pOpt = (COptionUI*)msg.pSender;

		COptionTag* pTag = (COptionTag*)pOpt->GetTag();
		if (pTag)
		{
			RECT rc = pOpt->GetPos();
			if (msg.ptMouse.x > rc.right - 30 && msg.ptMouse.y < rc.top + 20)
			{
				pWKEWebkitCtrl_->DelPage(pTag->nID_);
			}
			else
			{
				pWKEWebkitCtrl_->ReFresh(pTag->nID_);
				pURLEditCtrl_->SetText(pWKEWebkitCtrl_->GetFinalURL(pTag->nID_).ToString().c_str());

			}

		}
	}
	else if (_T("ui_btn_close") == strName)
	{
		if ((pWKEWebkitCtrl_ != NULL) && (!pWKEWebkitCtrl_->IsClosed()))
		{
			pWKEWebkitCtrl_->CloseAllPage();
		}

	}

	__super::OnClick(msg);

}


void CEFWebkitBrowserWnd::InitWindow()
{
	pWebStateCtrl_ = dynamic_cast<CLabelUI*>(FindControl(_T("ui_lbl_status")));
	pURLEditCtrl_ = dynamic_cast<CRichEditUI*>(FindControl(_T("ui_et_urlinput")));
	pSearchEditCtrl_ = dynamic_cast<CEditUI*>(FindControl(_T("ui_et_search")));
	pWebTabContainer_ = dynamic_cast<CHorizontalLayoutUI*>(FindControl(_T("ui_hl_urltabs")));
	pGoBackCtrl_ = dynamic_cast<CButtonUI*>(FindControl(_T("ui_btn_goback")));
	pGoForwardCtrl_ = dynamic_cast<CButtonUI*>(FindControl(_T("ui_btn_goforward")));;

	if ((!pWebStateCtrl_) || (!pURLEditCtrl_) || (!pWebTabContainer_) || (!pGoBackCtrl_) || (!pGoForwardCtrl_) || (!pSearchEditCtrl_))
	{
		MessageBox(GetSafeHwnd(), _T("�ؼ���ʼ��ʧ�ܣ�������"), _T("Err"), MB_OK);
		PostQuitMessage(0);
	}

}


void CEFWebkitBrowserWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("ui_btn_goback"))
		{
			COptionUI* pCurrentOpt = NULL;
			COptionTag* pTag = NULL;

			if (pCurrentOpt = GetActiveOption())
			{
				if (pTag = (COptionTag*)pCurrentOpt->GetTag())
				{

					pWKEWebkitCtrl_->GoBack(pTag->nID_);
				}
			}
		}
		else if (msg.pSender->GetName() == _T("ui_btn_goforward"))
		{
			COptionUI* pCurrentOpt = NULL;
			COptionTag* pTag = NULL;

			if (pCurrentOpt = GetActiveOption())
			{
				if (pTag = (COptionTag*)pCurrentOpt->GetTag())
				{

					pWKEWebkitCtrl_->GoForward(pTag->nID_);
				}
			}
		}
		else if (msg.pSender->GetName() == _T("ui_btn_refresh"))
		{
			COptionUI* pCurrentOpt = NULL;
			COptionTag* pTag = NULL;

			if (pCurrentOpt = GetActiveOption())
			{
				if (pTag = (COptionTag*)pCurrentOpt->GetTag())
				{
					pWKEWebkitCtrl_->ReLoad(pTag->nID_);
				}
			}

		}
		else if (msg.pSender->GetName() == _T("ui_btn_home"))
		{
			pWKEWebkitCtrl_->NewPage("www.baidu.com");
		}
		else if (msg.pSender->GetName() == _T("ui_btn_newpage"))
		{
			if (pWKEWebkitCtrl_)
			{
				//	pWKEWebkitCtrl_->NewPage(_T("about:black"));
				pWKEWebkitCtrl_->NewPage(_T("about:blank"));
			}
		}


	}
	//��Ҫ�ر�richedit��want return����
	else if (msg.sType == DUI_MSGTYPE_RETURN)
	{
		CefString strURL;

		if (pURLEditCtrl_ == msg.pSender)
		{
			if (pURLEditCtrl_&&pWKEWebkitCtrl_)
			{


				if (pURLEditCtrl_->GetText().IsEmpty())
				{
					return;
				}
				else
				{
					strURL = pURLEditCtrl_->GetText().GetData();
				}

				COptionUI* pCurrentOpt = NULL;
				COptionTag* pTag = NULL;

				if (pCurrentOpt = GetActiveOption())
				{
					if (pTag = (COptionTag*)pCurrentOpt->GetTag())
					{
						pWKEWebkitCtrl_->LoadURL(pTag->nID_, strURL);
					}
				}

			}
		}
		else if (pSearchEditCtrl_ == msg.pSender)
		{
			if (!pSearchEditCtrl_->GetText().IsEmpty())
			{
				CDuiString sUrl;
				sUrl.Format(_T("https://www.baidu.com/s?ie=utf-8&wd=%s"), pSearchEditCtrl_->GetText().GetData());
				pWKEWebkitCtrl_->NewPage(sUrl.GetData());
			}

		}
	}
	else if (msg.sType == _T("windowinit"))
	{
		OnInitComplate();
	}
	else if (msg.sType == DUI_MSGTYPE_TIMER)
	{
		if (msg.pSender == pWKEWebkitCtrl_)
		{
			if (msg.wParam == E_GOFORWORD_TIMER)
			{
				SwitchUIState();
			}

		}

	}


	__super::Notify(msg);
}


LRESULT CEFWebkitBrowserWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//TCHAR szBuf[256];

	switch (uMsg)
	{

	case UM_CEF_POSTQUITMESSAGE:
		bHandled = TRUE;
		//Sleep(2000);
	//	CefQuitMessageLoop();
		PostQuitMessage(0L);
		break;
	case UM_CEF_WEBLOADPOPUP:
	{

		CefString* pStrComplateURL = (CefString*)lParam;
		if (pStrComplateURL != NULL)
		{
			pWKEWebkitCtrl_->NewPage(*pStrComplateURL);

			delete pStrComplateURL;
		}

	}
	break;
	case UM_CEF_WEBLOADEND:
	{
		OnWebLoadEnd(wParam, lParam);
	}
	break;

	case UM_CEF_WEBLOADSTART:
	{
		OnWebLoadStart(wParam, lParam);
	}
	break;
	case UM_CEF_WEBTITLECHANGE:
	{
		CefString* pStrComplateURL = (CefString*)lParam;
		if (pStrComplateURL != NULL)
		{
			OnTitleChanged(wParam, *pStrComplateURL);
			delete pStrComplateURL;
		}
	}
	break;
	case UM_CEF_AFTERCREATED:
	{
		OnAfterCreate(wParam);
	}
	break;
	case UM_CEF_BROWSERCLOSE:
	{
		OnBrowserClose(wParam);
	}
	break;
	default:
		break;
	}

	bHandled = FALSE;

	return 0;
}



void CEFWebkitBrowserWnd::OnInitComplate()
{
	if (pWKEWebkitCtrl_)
	{
		//pWKEWebkitCtrl_->NewPage(_T("about:blank"));
		char szPath[1024] = {0};
		::GetModuleFileName(NULL, szPath, 1024);
		::PathAppend(szPath, _T("..\\"));
		::PathAppend(szPath, _T("htmlexample/index.html"));
		CDuiString sDefUrl;
		sDefUrl = szPath;
		sDefUrl.Replace(_T("\\"), _T("/"));
		sDefUrl.Format(_T("file:///%s"), sDefUrl.GetData());
		pWKEWebkitCtrl_->NewPage(sDefUrl.GetData());
		//pWKEWebkitCtrl_->NewPage(_T("file:///htmlexample/index.html"));
	}

	GetPaintManager()->SetTimer(pWKEWebkitCtrl_, E_GOFORWORD_TIMER, 1000);

}

std::string UTF16ToString(const std::wstring& str)
{
	std::string retStr;
	if(str.length() > 0)
	{
		int nLen = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, NULL, NULL, NULL);

		char * pBuf = new char[nLen + 1];
		memset(pBuf, 0, nLen + 1);
		WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, pBuf, nLen, NULL, NULL);
		retStr = pBuf;
		delete []pBuf;
		pBuf = NULL;
	}	

	return retStr;
}

//����ı�
void CEFWebkitBrowserWnd::OnTitleChanged(int nID, const CefString str)
{
	int nCountWebtab = pWebTabContainer_->GetCount();
	for (int idx = 0; idx < nCountWebtab; idx++)
	{
		COptionUI* pOpt = (COptionUI*)pWebTabContainer_->GetItemAt(idx);
		COptionTag* pTag = (COptionTag*)pOpt->GetTag();

		if ((pTag != NULL) && (pTag->nID_ == nID))
		{
			std::wstring wsStr = str.ToWString();
			std::string asStr = UTF16ToString(wsStr);
			pOpt->SetText(asStr.c_str());
			pOpt->SetToolTip(asStr.c_str());			
		}
	}
}

void CEFWebkitBrowserWnd::OnAfterCreate(int nWebBrowserID)
{
	// ������ǩOPTION��ť
	CDuiString sAttr;
	COptionUI* pOpt = new COptionUI;
	sAttr.Format(_T("name=\"webtab\" width=\"100\" height=\"30\" normalimage=\"tabbar_normal.png\" hotimage=\"tabbar_hover.png\" \
			pushedimage=\"tabbar_pushed.png\" selectedimage=\"tabbar_pushed.png\" endellipsis=\"true\" group=\"ui_grp_webtab\""));
	pOpt->ApplyAttributeList(sAttr);

	int iCount = pWebTabContainer_->GetCount();
	pWebTabContainer_->AddAt(pOpt, iCount - 1);

	// �ѱ�ǩ��ť�������id��������
	COptionTag* pTag = new COptionTag(nWebBrowserID);

	pWKEWebkitCtrl_->ReFresh(pTag->nID_);

	pOpt->SetTag((UINT_PTR)pTag);
	pOpt->Selected(true);

}

void CEFWebkitBrowserWnd::OnBrowserClose(int nBrowserID)
{
	TCHAR strDebugMsg[128];

	int nCountWebtab = pWebTabContainer_->GetCount();

	for (int idx = 0; idx < nCountWebtab; idx++)
	{

		COptionUI* pOpt = (COptionUI*)pWebTabContainer_->GetItemAt(idx);


		COptionTag* pTag = (COptionTag*)pOpt->GetTag();


		if ((pTag != NULL) && (pTag->nID_ == nBrowserID))
		{

			int nSize = pWebTabContainer_->GetCount();

			int nIdx = pWebTabContainer_->GetItemIndex(pOpt);

			pWebTabContainer_->Remove(pOpt);

			if (nIdx != -1)
			{

				if (nIdx == (nSize - 2))
				{
					--nIdx;
				}

				COptionUI* pShilfOption = dynamic_cast<COptionUI*>(pWebTabContainer_->GetItemAt(nIdx));
				COptionTag* pShilfTag = (COptionTag*)pShilfOption->GetTag();


				if ((pShilfOption) && (pShilfTag != NULL))
				{
					pWKEWebkitCtrl_->ReFresh(pShilfTag->nID_);
					pURLEditCtrl_->SetText(pWKEWebkitCtrl_->GetFinalURL(pShilfTag->nID_).ToString().c_str());
					pShilfOption->Selected(true);

				}

			}

			break;
		}
	}

	StringCbPrintf(strDebugMsg, sizeof(strDebugMsg), _T("delete id=%d\n"), nBrowserID);
	OutputDebugString(strDebugMsg);

}

void CEFWebkitBrowserWnd::OnWebLoadEnd(WPARAM wParam, LPARAM lParam)
{

	TCHAR szBuf[256];
	int nBrowserID = wParam;

	if (pURLEditCtrl_)
	{

		COptionUI* pCurrentOpt = NULL;
		COptionTag* pTag = NULL;

		if (pCurrentOpt = GetActiveOption())
		{
			if (pTag = (COptionTag*)pCurrentOpt->GetTag())
			{
				if (pTag->nID_ == nBrowserID)
				{
					pURLEditCtrl_->SetText(pWKEWebkitCtrl_->GetFinalURL(nBrowserID).ToString().c_str());
					SwitchUIState();
				}
			}
		}
	}

	memset(szBuf, '\0', sizeof(szBuf));
	CefString* pStrComplateURL = (CefString*)lParam;
	if (pStrComplateURL != NULL)
	{
		StringCbPrintf(szBuf, sizeof(szBuf), _T("�������:%s"), pStrComplateURL->c_str());
		delete pStrComplateURL;
	}

	pWebStateCtrl_->SetText(szBuf);
}

void CEFWebkitBrowserWnd::OnWebLoadStart(WPARAM wParam, LPARAM lParam)
{
	TCHAR szBuf[256];
	memset(szBuf, '\0', sizeof(szBuf));
	CefString* pStrComplateURL = (CefString*)lParam;
	if (pStrComplateURL != NULL)
	{
		StringCbPrintf(szBuf, sizeof(szBuf), _T("���ڼ���:%s"), pStrComplateURL->c_str());
		delete pStrComplateURL;
	}

	pWebStateCtrl_->SetText(szBuf);

}

COptionUI * CEFWebkitBrowserWnd::GetActiveOption() const
{
	COptionUI* pCurrentOpt = NULL;
	int nCountWebtab = pWebTabContainer_->GetCount();

	for (int idx = 0; idx < nCountWebtab; idx++)
	{
		COptionUI* pOpt = (COptionUI*)pWebTabContainer_->GetItemAt(idx);

		if (pOpt->IsSelected())
		{
			pCurrentOpt = pOpt;
		}

	}

	return pCurrentOpt;
}

void CEFWebkitBrowserWnd::SwitchUIState()
{
	COptionUI* pCurrentOpt = NULL;
	COptionTag* pTag = NULL;

	if (pCurrentOpt = GetActiveOption())
	{
		if (pTag = (COptionTag*)pCurrentOpt->GetTag())
		{
			if (pWKEWebkitCtrl_->CanGoBack(pTag->nID_) != FALSE)
			{
				pGoBackCtrl_->SetEnabled(true);
			}
			else
			{
				pGoBackCtrl_->SetEnabled(false);
			}


			if (pWKEWebkitCtrl_->CanGoForward(pTag->nID_) != FALSE)
			{

				pGoForwardCtrl_->SetEnabled(true);
			}
			else
			{
				pGoForwardCtrl_->SetEnabled(false);
			}
		}
	}

}
