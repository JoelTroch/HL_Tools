#ifndef HLMV_UI_OPTIONS_CGENERALOPTIONS_H
#define HLMV_UI_OPTIONS_CGENERALOPTIONS_H

#include "../wxHLMV.h"

#include "cvar/CCVar.h"

class wxColourPickerCtrl;

namespace hlmv
{
class CHLMVSettings;

class CGeneralOptions final : public wxPanel, public cvar::ICVarHandler
{
public:
	CGeneralOptions( wxWindow* pParent, CHLMVSettings* const pSettings );
	~CGeneralOptions();

	void Save();

protected:
	wxDECLARE_EVENT_TABLE();

private:
	void Initialize();

	void HandleCVar( cvar::CCVar& cvar, const char* pszOldValue, float flOldValue ) override final;

	void SetDefaultColor( wxCommandEvent& event );

	void SetDefaultFPS( wxCommandEvent& event );

	void SetDefaultFloorLength( wxCommandEvent& event );

private:
	CHLMVSettings* const m_pSettings;

	wxCheckBox* m_pPowerOf2Textures;
	wxCheckBox* m_pUseTimerForFrame;
	wxCheckBox* m_pInvertHorizontalDragging;
	wxCheckBox* m_pInvertVerticalDragging;

	wxColourPickerCtrl* m_pGroundColor;
	wxColourPickerCtrl* m_pBackgroundColor;
	wxColourPickerCtrl* m_pCrosshairColor;
	wxColourPickerCtrl* m_pLightColor;
	wxColourPickerCtrl* m_pWireframeColor;

	wxSlider* m_pFPS;
	wxSlider* m_pFloorLength;

private:
	CGeneralOptions( const CGeneralOptions& ) = delete;
	CGeneralOptions& operator=( const CGeneralOptions& ) = delete;
};
}

#endif //HLMV_UI_OPTIONS_CGENERALOPTIONS_H