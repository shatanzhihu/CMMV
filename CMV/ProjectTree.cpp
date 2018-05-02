// ProjectView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProjectTree.h"
#include "MainFrm.h"

using namespace std;

// CProjectView

//IMPLEMENT_DYNCREATE(CProjectTreeView, CTreeView)

CProjectTree::CProjectTree()
{

}

CProjectTree::~CProjectTree()
{
}


//https://social.msdn.microsoft.com/Forums/vstudio/en-US/7a012ba0-dad8-4644-810f-68f2ff94572f/vs-2015-c-compile-error-extra-text-after-expected-end-of-number?forum=vcgeneral

BEGIN_MESSAGE_MAP(CProjectTree, CTreeCtrl)
	ON_NOTIFY_REFLECT_EX1(TVN_BEGINLABELEDIT, &CProjectTree::OnTvnBeginlabeledit)
	ON_NOTIFY_REFLECT_EX1(TVN_ENDLABELEDIT, &CProjectTree::OnTvnEndlabeledit)
	ON_NOTIFY_REFLECT_EX1(NM_DBLCLK, &CProjectTree::OnNMDblclk)
	ON_NOTIFY_REFLECT_EX1(NM_CLICK, &CProjectTree::OnNMClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree ��Ϣ��������


HTREEITEM CProjectTree::findProjectItemByPrID(int prID)         // ͨ����ĿID������Ŀ�ڵ�
{
	
	HTREEITEM hroot = this->GetRootItem();

	HTREEITEM hChildItem = GetChildItem(hroot);

	while (hChildItem != NULL) {

		if (this->GetItemData(hChildItem) == prID) {
			return hChildItem;
		}

		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
	
	return HTREEITEM();
}

// ɾ����Ŀ�µ����в���Ԫ��
void CProjectTree::DelAllByPrID(int prID)
{
	HTREEITEM hroot = this->GetRootItem();
	HTREEITEM hChildItem = GetChildItem(hroot);
//	int prID = UCI::OPTIONS[L"��Ŀ���"];

	// ����ÿһ����Ŀ
	while (hChildItem != NULL) {

		//if (this->GetItemData(hChildItem) == prID) {

			// ��ʼɾ������ڵ��������ڵ�
			HTREEITEM it = GetChildItem(hChildItem);

			while (it != NULL) {
				HTREEITEM hNextChild = GetNextSiblingItem(it);
				DeleteItem(it);
				it = hNextChild;
			}
		//}
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
}

// ����һ������Ԫ�� �� ��Ŀ�� �ϵ�β��
BOOL CProjectTree::AddOneElementTail(UCI::OptionsMap * o)
{
	
	HTREEITEM hroot = this->GetRootItem();
	HTREEITEM hChildItem = GetChildItem(hroot);

	int prID = UCI::OPTIONS[L"��Ŀ���"];

	// ����ÿһ����Ŀ
	while (hChildItem != NULL) {

		if (this->GetItemData(hChildItem) == prID) {

			CString name = (*o)[L"Ԫ������"];
			int type = (*o)[L"Ԫ������"];  

			POSITION pos = UCI::ELE.GetOptionsPosByEleName(name);

			UINT mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

			this->InsertItem(mask, name,
				ELEMTEN_PIC_INDEX_NOSEL[type], ELEMTEN_PIC_INDEX_SEL[type],
				0, 0, (LPARAM)pos, hChildItem, NULL);

			this->Expand(hChildItem, TVE_EXPAND);
			return 0;
		}
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
	
	return 1;
}

UCI::OptionsMap* CProjectTree::AddOneElement(UCI::OptionsMap * o, HTREEITEM hCurrentSel)
{
	HTREEITEM hroot = this->GetRootItem();
	HTREEITEM hProjectItem = GetChildItem(hroot);

	int prID = UCI::OPTIONS[L"��Ŀ���"];
	
	// ����ÿһ����Ŀ
	while (hProjectItem != NULL) {
		if (this->GetItemData(hProjectItem) == prID) {
			CString name = (*o)[L"Ԫ������"];
			int type = (*o)[L"Ԫ������"];
			UINT mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

			POSITION npos;

			if (hCurrentSel == NULL) {   // ��β������				
				
			}
			else {

				POSITION selpos = (POSITION)this->GetItemData(hCurrentSel);

				// ��Ҫ�ж�һ���ǲ���ѡ���ǰ������
				bool isFirst3 = false;
				int i = 0;
				POSITION pos = UCI::ELE.EleList.GetHeadPosition();
				while (pos != NULL && i<3) {
					if (pos == selpos) {
						isFirst3 = true;
						break;
					}
					UCI::OptionsMap *ot = &UCI::ELE.EleList.GetNext(pos);
					i++;
				}

				if (isFirst3) {
					hCurrentSel = NULL;
				}
				else {
					UCI::ELE.EleList.InsertAfter(selpos, *o);
					UCI::ELE.EleList.RemoveTail();
					
				}
			}

			npos = UCI::ELE.GetOptionsPosByEleName(name);
			o = &UCI::ELE.EleList.GetAt(npos);

			this->InsertItem(mask, name,
				ELEMTEN_PIC_INDEX_NOSEL[type], ELEMTEN_PIC_INDEX_SEL[type],
				0, 0, (LPARAM)npos, hProjectItem, hCurrentSel);   // β������

			this->Expand(hProjectItem, TVE_EXPAND);
			return o;
		}

		hProjectItem = GetNextItem(hProjectItem, TVGN_NEXT);
	}
	return NULL;
}

// 
BOOL CProjectTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}



void CProjectTree::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: �ڴ����ӿؼ�֪ͨ�����������

	this->GetEditControl()->LimitText(50);

	*pResult = 0;
}


void CProjectTree::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: �ڴ����ӿؼ�֪ͨ�����������

	HTREEITEM it = GetSelectedItem();
	HTREEITEM parent = this->GetParentItem(it);

	if (parent == nullptr) {   // ����Ǹ�Ŀ¼
		return;
	}

	TCHAR s[50];
	this->GetEditControl()->GetWindowText(s, 50);

	if (this->GetParentItem(parent) == nullptr) {    // �������Ŀ

		// ������Ŀ����
		int proID = (int)this->GetItemData(it);
		UCI::sDB.MyProjectRename(s, proID);
		this->SetItemText(it, s);
	}
	else {
		// ����һ��Ԫ�ؽڵ㣬 ������Ҫ��һ���ǲ������ظ�������
		UCI::OptionsMap* oc = UCI::ELE.GetOptionsMapByEleName(s);
		if (oc == nullptr) {
			this->SetItemText(it, s);
			POSITION pos = (POSITION)this->GetItemData(it);
			UCI::OptionsMap *ot = &UCI::ELE.EleList.GetNext(pos);
			//CString old = (*ot)[L"Ԫ������"];
			(*ot)[L"Ԫ������"] = s;
		}
		else {
			UCI::MyLog(L"Ԫ��������������������Ԫ��");
		}
	}

	

	*pResult = 0;
}

// �û�˫����Ԫ��
void CProjectTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// �����ǰ״̬���� READY, �ͷ���
	if(!UCI::pDis->TestCurrentAppState(APP_LOGGING_OK)) return;
	
	HTREEITEM hSel = this->GetSelectedItem();
	HTREEITEM hParent;

	CString name = this->GetItemText(hSel);

	if (name == L"������Ŀ") { // root �ڵ�
		return;
	}

	hParent = this->GetParentItem(hSel);
	if (hParent == nullptr)return;
	int curPrID = UCI::OPTIONS[L"��Ŀ���"];

	//===========================================================================================
	// ��Ŀ�ڵ�
	if (this->GetItemText(hParent) == L"������Ŀ") {  
		int prID = (int)this->GetItemData(hSel);      

		if (prID == curPrID) {  // ˫���˵�ǰ����Ŀ������ʾȫ�ֵı���

			UCI::pMain->SetPettyCurSel(PROPLIST_GLOBAL);
			UCI::pMain->SetPropertyCurOptionsMap(&UCI::OPTIONS);
			UCI::pMain->ProPertyUpdate();
			return;
		}
		return;
	}
	//===========================================================================================
	// Ԫ�ؽڵ�
	int prID = (int)this->GetItemData(hParent);                        // �õ����ڵ����Ŀ���	
	if (prID == curPrID) {	

		// �����Ա�����ʾ��ǰԪ��
		UCI::pMain->SetPettyCurSel(PROPLIST_OTHER);
		UCI::OptionsMap* ocur = UCI::ELE.GetOptionsMapByEleName(name);
		UCI::pMain->SetPropertyCurOptionsMap(ocur);
		UCI::pMain->ProPertyUpdate();

		UCI::pDis->EditOneElement(ocur);
	}
	

	*pResult = 0;
}


// �������
void CProjectTree::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������




	*pResult = 0;
}