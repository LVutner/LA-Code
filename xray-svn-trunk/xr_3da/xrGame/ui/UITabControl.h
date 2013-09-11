#pragma once

#include "uiwindow.h"
#include "../script_export_space.h"
#include "UIOptionsItem.h"

class CUITabButton;

DEF_VECTOR (TABS_VECTOR, CUITabButton*)

class CUITabControl: public CUIWindow , public CUIOptionsItem 
{
	typedef				CUIWindow inherited;
public:
						CUITabControl				();
	virtual				~CUITabControl				();

	// options item
	virtual void		SetCurrentOptValue			();	// opt->current
	virtual void		SaveBackUpOptValue			();	// current->backup
	virtual void		SaveOptValue				();	// current->opt
	virtual void		UndoOptValue				();	// backup->current
	virtual bool		IsChangedOptValue			() const;	// backup!=current

	virtual bool		OnKeyboardAction			(int dik, EUIMessages keyboard_action);
	virtual void		OnTabChange					(const int iCur, const int iPrev);
	virtual void		OnStaticFocusReceive		(CUIWindow* pWnd);
	virtual void		OnStaticFocusLost			(CUIWindow* pWnd);

	// ���������� ������-�������� � ������ �������� ��������
	bool				AddItem						(LPCSTR pItemName, LPCSTR pTexName, Fvector2 pos, Fvector2 size);
	bool				AddItem						(CUITabButton *pButton);

	void				RemoveAll					();

	virtual void		SendMessage					(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void		Enable						(bool status);

			int			GetActiveIndex				()	const						{ return m_iPushedIndex; }
			int			GetPrevActiveIndex			()	const						{ return m_iPrevPushedIndex; }
			void		SetNewActiveTab				(const int iNewTab);	
			void		SetNewActiveTab_script			(int iNewTab)				{SetNewActiveTab(iNewTab);};
	const	u32			GetTabsCount				() const						{ return m_TabsArr.size(); }
	
	// ����� ������������� ������������� (���/����)
	IC bool				GetAcceleratorsMode			() const						{ return m_bAcceleratorsEnable; }
	void				SetAcceleratorsMode			(bool bEnable)					{ m_bAcceleratorsEnable = bEnable; }


	TABS_VECTOR *		GetButtonsVector			()								{ return &m_TabsArr; }
	CUITabButton*		GetButtonById				(const int id);
//	CUITabButton*		GetButtonById_script		(LPCSTR s)						{ return GetButtonById(s);}

	void		ResetTab					();
protected:
	// ������ ������ - �������������� ��������
	TABS_VECTOR			m_TabsArr;

	int					m_iPushedIndex;
	int					m_iPrevPushedIndex;

	// ���� ���������� ���������
	u32					m_cGlobalTextColor;
	u32					m_cGlobalButtonColor;

	// ���� ������� �� �������� ��������
	u32					m_cActiveTextColor;
	u32					m_cActiveButtonColor;

	bool				m_bAcceleratorsEnable;
	int					m_opt_backup_value;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUITabControl)
#undef script_type_list
#define script_type_list save_type_list(CUITabControl)

