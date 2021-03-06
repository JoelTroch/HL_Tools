#include <cfloat>

#include <wx/button.h>
#include <wx/spinctrl.h>
#include <wx/gbsizer.h>

#include "../CModelViewerApp.h"

#include "ui/common/CFOVCtrl.h"
#include "wx/utility/wxUtil.h"

#include "cvar/CVar.h"

#include "CModelDisplayPanel.h"

namespace hlmv
{
wxBEGIN_EVENT_TABLE( CModelDisplayPanel, CBaseControlPanel )
	EVT_CHOICE( wxID_MDLDISP_RENDERMODE, CModelDisplayPanel::RenderModeChanged )
	EVT_SLIDER( wxID_MDLDISP_OPACITY, CModelDisplayPanel::OpacityChanged )
	EVT_CHECKBOX( wxID_MDLDISP_CHECKBOX, CModelDisplayPanel::CheckBoxChanged )
	EVT_CHECKBOX( wxID_MDLDISP_MIRROR, CModelDisplayPanel::OnMirrorAxis )
	EVT_FOV_CHANGED( wxID_MDLDISP_FOVCHANGED, CModelDisplayPanel::OnFOVChanged )
	EVT_FOV_CHANGED( wxID_MDLDISP_FPFOVCHANGED, CModelDisplayPanel::OnFOVFPChanged )
wxEND_EVENT_TABLE()

//Client data for the mirror checkboxes
static const size_t MIRROR[ 3 ] = { 0, 1, 2 };

CModelDisplayPanel::CModelDisplayPanel( wxWindow* pParent, CModelViewerApp* const pHLMV )
	: CBaseControlPanel( pParent, "Model Display", pHLMV )
{
	//Helps catch errors if we miss one.
	memset( m_pCheckBoxes, 0, sizeof( m_pCheckBoxes ) );

	wxWindow* const pElemParent = GetElementParent();

	wxStaticText* pRenderMode = new wxStaticText( pElemParent, wxID_ANY, "Render mode:" );

	m_pOpacity = new wxStaticText( pElemParent, wxID_ANY, "Opacity: Undefined%" );

	m_pOpacitySlider = new wxSlider( pElemParent, wxID_MDLDISP_OPACITY, OPACITY_DEFAULT, OPACITY_MIN, OPACITY_MAX );

	m_pRenderMode = new wxChoice( pElemParent, wxID_MDLDISP_RENDERMODE );

	for( int iRenderMode = static_cast<int>( RenderMode::FIRST ); iRenderMode < static_cast<int>( RenderMode::COUNT ); ++iRenderMode )
	{
		m_pRenderMode->Append( RenderModeToString( static_cast<RenderMode>( iRenderMode ) ) );
	}

	m_pRenderMode->SetSelection( static_cast<int>( RenderMode::TEXTURE_SHADED ) );

	m_pCheckBoxes[ CheckBox::SHOW_HITBOXES ]		= new wxCheckBox( pElemParent, wxID_MDLDISP_CHECKBOX, "Show Hit Boxes" );
	m_pCheckBoxes[ CheckBox::SHOW_BONES ]			= new wxCheckBox( pElemParent, wxID_MDLDISP_CHECKBOX, "Show Bones" );
	m_pCheckBoxes[ CheckBox::SHOW_ATTACHMENTS ]		= new wxCheckBox( pElemParent, wxID_MDLDISP_CHECKBOX, "Show Attachments" );
	m_pCheckBoxes[ CheckBox::SHOW_EYE_POSITION ]	= new wxCheckBox( pElemParent, wxID_MDLDISP_CHECKBOX, "Show Eye Position" );
	m_pCheckBoxes[ CheckBox::BACKFACE_CULLING ]		= new wxCheckBox( pElemParent, wxID_MDLDISP_CHECKBOX, "Backface Culling" );

	m_pCheckBoxes[ CheckBox::SHOW_GROUND ]			= new wxCheckBox( pElemParent, wxID_MDLDISP_CHECKBOX, "Show Ground" );
	m_pCheckBoxes[ CheckBox::MIRROR_ON_GROUND ]		= new wxCheckBox( pElemParent, wxID_MDLDISP_CHECKBOX, "Mirror Model On Ground" );
	m_pCheckBoxes[ CheckBox::SHOW_BACKGROUND ]		= new wxCheckBox( pElemParent, wxID_MDLDISP_CHECKBOX, "Show Background" );
	m_pCheckBoxes[ CheckBox::WIREFRAME_OVERLAY ]	= new wxCheckBox( pElemParent, wxID_MDLDISP_CHECKBOX, "Wireframe Overlay" );
	m_pCheckBoxes[CheckBox::DRAW_SHADOWS]			= new wxCheckBox(pElemParent, wxID_MDLDISP_CHECKBOX, "Draw Shadows");
	m_pCheckBoxes[CheckBox::FIX_SHADOW_Z_FIGHTING]	= new wxCheckBox(pElemParent, wxID_MDLDISP_CHECKBOX, "Fix Shadow Z Fighting");
	m_pCheckBoxes[ CheckBox::SHOW_AXES ]			= new wxCheckBox( pElemParent, wxID_MDLDISP_CHECKBOX, "Show Axes" );
	m_pCheckBoxes[ CheckBox::SHOW_NORMALS ]			= new wxCheckBox( pElemParent, wxID_MDLDISP_CHECKBOX, "Show Normals" );

	m_pCheckBoxes[CheckBox::SHOW_CROSSHAIR] = new wxCheckBox(pElemParent, wxID_MDLDISP_CHECKBOX, "Show Crosshair");
	m_pCheckBoxes[CheckBox::SHOW_GUIDELINES] = new wxCheckBox(pElemParent, wxID_MDLDISP_CHECKBOX, "Show Guidelines");
	m_pCheckBoxes[CheckBox::SHOW_PLAYER_HITBOX] = new wxCheckBox(pElemParent, wxID_MDLDISP_CHECKBOX, "Show Player Hitbox");

	for( size_t uiIndex = CheckBox::FIRST; uiIndex < CheckBox::COUNT; ++uiIndex )
	{
		wxASSERT( m_pCheckBoxes[ uiIndex ] );

		m_pCheckBoxes[ uiIndex ]->SetClientData( reinterpret_cast<void*>( uiIndex ) );
	}

	m_pCheckBoxes[ CheckBox::BACKFACE_CULLING ]->SetValue( true );

	m_pMirror[ 0 ] = new wxCheckBox( pElemParent, wxID_MDLDISP_MIRROR, "Mirror on X axis" );
	m_pMirror[ 1 ] = new wxCheckBox( pElemParent, wxID_MDLDISP_MIRROR, "Mirror on Y axis" );
	m_pMirror[ 2 ] = new wxCheckBox( pElemParent, wxID_MDLDISP_MIRROR, "Mirror on Z axis" );

	m_pFOV = new ui::CFOVCtrl( this, wxID_MDLDISP_FOVCHANGED, CHLMVState::DEFAULT_FOV, "Field Of View" );

	m_pFPFOV = new ui::CFOVCtrl( this, wxID_MDLDISP_FPFOVCHANGED, CHLMVState::DEFAULT_FP_FOV, "First Person Field Of View" );

	for( size_t uiIndex = 0; uiIndex < 3; ++uiIndex )
	{
		m_pMirror[ uiIndex ]->SetClientData( const_cast<size_t*>( &MIRROR[ uiIndex ] ) );
	}

	m_pCenterOnOriginButton = new wxButton(pElemParent, wxID_ANY, "Center Model On World Origin");
	m_pCenterOnOriginButton->Bind(wxEVT_BUTTON, &CModelDisplayPanel::OnCenterOnOrigin, this);

	m_pAlignOnGroundButton = new wxButton(pElemParent, wxID_ANY, "Align On Ground");
	m_pAlignOnGroundButton->Bind(wxEVT_BUTTON, &CModelDisplayPanel::OnAlignOnGround, this);

	//Layout
	auto pSizer = new wxBoxSizer( wxHORIZONTAL );

	auto pFirstColSizer = new wxBoxSizer( wxVERTICAL );

	pFirstColSizer->Add( pRenderMode, wxSizerFlags().Expand() );
	pFirstColSizer->Add( m_pRenderMode, wxSizerFlags().Expand() );

	pFirstColSizer->Add( m_pOpacity, wxSizerFlags().Expand() );
	pFirstColSizer->Add( m_pOpacitySlider, wxSizerFlags().Expand() );

	pSizer->Add( pFirstColSizer, wxSizerFlags().Expand() );

	auto pCheckBoxSizer = wx::CreateCheckBoxSizer( m_pCheckBoxes, ARRAYSIZE( m_pCheckBoxes ), NUM_CHECKBOX_COLS, wxEXPAND );

	pSizer->Add( pCheckBoxSizer, wxSizerFlags().Expand().Border() );

	auto pMirrorSizer = new wxBoxSizer( wxVERTICAL );

	pMirrorSizer->Add( m_pMirror[ 0 ], wxSizerFlags().Expand() );
	pMirrorSizer->AddSpacer( 10 );
	pMirrorSizer->Add( m_pMirror[ 1 ], wxSizerFlags().Expand() );
	pMirrorSizer->AddSpacer( 10 );
	pMirrorSizer->Add( m_pMirror[ 2 ], wxSizerFlags().Expand() );

	pSizer->Add( pMirrorSizer, wxSizerFlags().Expand().Border() );

	auto pFOVSizer = new wxBoxSizer( wxVERTICAL );

	pFOVSizer->Add( m_pFOV, wxSizerFlags().Expand() );

	pFOVSizer->Add( m_pFPFOV, wxSizerFlags().Expand() );

	pSizer->Add( pFOVSizer, wxSizerFlags().Expand() );

	{
		auto buttonsSizer = new wxBoxSizer(wxVERTICAL);

		buttonsSizer->Add(m_pCenterOnOriginButton, wxSizerFlags().Expand());
		buttonsSizer->Add(m_pAlignOnGroundButton, wxSizerFlags().Expand());

		pSizer->Add(buttonsSizer);
	}

	GetMainSizer()->Add( pSizer );

	g_pCVar->InstallGlobalCVarHandler( this );
}

CModelDisplayPanel::~CModelDisplayPanel()
{
	g_pCVar->RemoveGlobalCVarHandler( this );
}

void CModelDisplayPanel::InitializeUI()
{
	SetRenderMode( RenderMode::TEXTURE_SHADED );
	SetOpacity( OPACITY_DEFAULT );

	auto pEntity = m_pHLMV->GetState()->GetEntity();

	for( size_t uiIndex = 0; uiIndex < 3; ++uiIndex )
	{
		m_pMirror[ uiIndex ]->SetValue( false );

		m_pMirror[ uiIndex ]->Enable( pEntity != nullptr );
	}

	m_pFOV->ChangeToDefault();
	m_pFPFOV->ChangeToDefault();

	m_pCenterOnOriginButton->Enable(pEntity != nullptr);
	m_pAlignOnGroundButton->Enable(pEntity != nullptr);
}

void CModelDisplayPanel::SetRenderMode( RenderMode renderMode )
{
	if( renderMode < RenderMode::FIRST )
		renderMode = RenderMode::FIRST;
	else if( renderMode > RenderMode::LAST )
		renderMode = RenderMode::LAST;

	m_pRenderMode->Select( static_cast<int>( renderMode ) );

	m_pHLMV->GetState()->renderMode = renderMode;
}

void CModelDisplayPanel::SetOpacity( int iValue, const bool bUpdateSlider )
{
	if( iValue < OPACITY_MIN )
		iValue = OPACITY_MIN;
	else if( iValue > OPACITY_MAX )
		iValue = OPACITY_MAX;

	m_pOpacity->SetLabelText( wxString::Format( "Opacity: %d%%", iValue ) );

	if( bUpdateSlider )
		m_pOpacitySlider->SetValue( iValue );

	auto pEntity = m_pHLMV->GetState()->GetEntity();

	if( !pEntity )
		return;

	pEntity->SetTransparency( iValue / static_cast<float>( OPACITY_MAX ) );
}

void CModelDisplayPanel::SetCheckBox( const CheckBox::Type checkBox, const bool bValue )
{
	InternalSetCheckBox( checkBox, bValue, false );
}

void CModelDisplayPanel::InternalSetCheckBox( const CheckBox::Type checkBox, const bool bValue, const bool bCameFromChangeEvent )
{
	if( checkBox < CheckBox::FIRST || checkBox > CheckBox::LAST )
		return;

	wxCheckBox* pCheckBox = m_pCheckBoxes[ checkBox ];

	//Don't do anything if it's identical. Helps prevent unnecessary calls.
	if( !bCameFromChangeEvent && pCheckBox->GetValue() == bValue )
		return;

	pCheckBox->SetValue( bValue );

	switch( checkBox )
	{
	case CheckBox::SHOW_HITBOXES:
		{
			g_pCVar->Command( wxString::Format( "r_showhitboxes %d", bValue ? 1 : 0 ).c_str() );
			break;
		}

	case CheckBox::SHOW_GROUND:
		{
			m_pHLMV->GetState()->showGround = bValue;

			if( !m_pHLMV->GetState()->showGround && m_pHLMV->GetState()->mirror )
			{
				SetCheckBox( CheckBox::MIRROR_ON_GROUND, false );
			}

			break;
		}

	case CheckBox::SHOW_BONES:
		{
			g_pCVar->Command( wxString::Format( "r_showbones %d", bValue ? 1 : 0 ).c_str() );

			break;
		}

	case CheckBox::MIRROR_ON_GROUND:
		{
			m_pHLMV->GetState()->mirror = bValue;

			if( m_pHLMV->GetState()->mirror && !m_pHLMV->GetState()->showGround )
			{
				SetCheckBox( CheckBox::SHOW_GROUND, true );
			}

			break;
		}

	case CheckBox::SHOW_ATTACHMENTS:
		{
			g_pCVar->Command( wxString::Format( "r_showattachments %d", bValue ? 1 : 0 ).c_str() );
			break;
		}

	case CheckBox::SHOW_BACKGROUND:
		{
			m_pHLMV->GetState()->showBackground = bValue;
			break;
		}

	case CheckBox::SHOW_EYE_POSITION:
		{
			g_pCVar->Command( wxString::Format( "r_showeyeposition %d", bValue ? 1 : 0 ).c_str() );
			break;
		}

	case CheckBox::WIREFRAME_OVERLAY:
		{
			m_pHLMV->GetState()->wireframeOverlay = bValue;
			break;
		}

	case CheckBox::DRAW_SHADOWS:
		{
			m_pHLMV->GetState()->drawShadows = bValue;
			break;
		}

	case CheckBox::FIX_SHADOW_Z_FIGHTING:
		{
			m_pHLMV->GetState()->fixShadowZFighting = bValue;
			break;
		}
	
	case CheckBox::BACKFACE_CULLING:
		{
			m_pHLMV->GetState()->backfaceCulling = bValue;
			break;
		}

	case CheckBox::SHOW_AXES:
		{
			m_pHLMV->GetState()->drawAxes = bValue;
			break;
		}

	case CheckBox::SHOW_NORMALS:
		{
			g_pCVar->SetCVarFloat( "r_showstudionormals", bValue ? 1 : 0 );
			break;
		}

	case CheckBox::SHOW_CROSSHAIR:
	{
		m_pHLMV->GetState()->drawCrosshair = bValue;
		break;
	}

	case CheckBox::SHOW_GUIDELINES:
	{
		m_pHLMV->GetState()->drawGuidelines = bValue;
		break;
	}

	case CheckBox::SHOW_PLAYER_HITBOX:
	{
		m_pHLMV->GetState()->drawPlayerHitbox = bValue;
		break;
	}

	default: break;
	}
}

void CModelDisplayPanel::RenderModeChanged( wxCommandEvent& event )
{
	const int iValue = m_pRenderMode->GetSelection();

	if( iValue == wxNOT_FOUND )
		return;

	SetRenderMode( static_cast<RenderMode>( iValue ) );
}

void CModelDisplayPanel::OpacityChanged( wxCommandEvent& event )
{
	SetOpacity( m_pOpacitySlider->GetValue(), false );
}

void CModelDisplayPanel::CheckBoxChanged( wxCommandEvent& event )
{
	wxCheckBox* const pCheckBox = static_cast<wxCheckBox*>( event.GetEventObject() );

	const CheckBox::Type checkbox = static_cast<CheckBox::Type>( reinterpret_cast<int>( pCheckBox->GetClientData() ) );

	if( checkbox < CheckBox::FIRST || checkbox > CheckBox::LAST )
		return;

	InternalSetCheckBox( checkbox, pCheckBox->GetValue(), true );
}

void CModelDisplayPanel::OnMirrorAxis( wxCommandEvent& event )
{
	auto pEntity = m_pHLMV->GetState()->GetEntity();

	if( !pEntity )
	{
		return;
	}

	const size_t uiIndex = *reinterpret_cast<const size_t*>( static_cast<wxCheckBox*>( event.GetEventObject() )->GetClientData() );

	pEntity->GetScale()[ uiIndex ] *= -1;
}

void CModelDisplayPanel::OnFOVChanged( wxCommandEvent& event )
{
	m_pHLMV->GetState()->flFOV = m_pFOV->GetValue();
}

void CModelDisplayPanel::OnFOVFPChanged( wxCommandEvent& event )
{
	m_pHLMV->GetState()->flFPFOV = m_pFPFOV->GetValue();
}

void CModelDisplayPanel::OnCenterOnOrigin(wxCommandEvent& event)
{
	if (auto entity = m_pHLMV->GetState()->GetEntity(); entity)
	{
		entity->SetOrigin({0, 0, 0});
	}
}

void CModelDisplayPanel::OnAlignOnGround(wxCommandEvent& event)
{
	if (auto entity = m_pHLMV->GetState()->GetEntity(); entity)
	{
		auto model = entity->GetModel();

		auto header = model->GetStudioHeader();

		//First try finding the idle sequence, since that typically represents a model "at rest"
		//Failing that, use the first sequence
		auto idleFinder = [&]() -> const mstudioseqdesc_t*
		{
			for (int i = 0; i < header->numseq; ++i)
			{
				const auto sequence = header->GetSequence(i);

				if (!strcmp(sequence->label, "idle"))
				{
					return sequence;
				}
			}

			return nullptr;
		};

		auto sequence = idleFinder();

		if (!sequence)
		{
			sequence = header->GetSequence(0);
		}

		entity->SetOrigin({0, 0, -sequence->bbmin.z});
	}
}

void CModelDisplayPanel::HandleCVar( cvar::CCVar& cvar, const char* pszOldValue, float flOldValue )
{
	if( strcmp( cvar.GetName(), "r_showbones" ) == 0 )
	{
		SetCheckBox( CheckBox::SHOW_BONES, cvar.GetBool() );
	}
	else if( strcmp( cvar.GetName(), "r_showattachments" ) == 0 )
	{
		SetCheckBox( CheckBox::SHOW_ATTACHMENTS, cvar.GetBool() );
	}
	else if( strcmp( cvar.GetName(), "r_showeyeposition" ) == 0 )
	{
		SetCheckBox( CheckBox::SHOW_EYE_POSITION, cvar.GetBool() );
	}
	else if( strcmp( cvar.GetName(), "r_showhitboxes" ) == 0 )
	{
		SetCheckBox( CheckBox::SHOW_HITBOXES, cvar.GetBool() );
	}
	else if( strcmp( cvar.GetName(), "r_showstudionormals" ) == 0 )
	{
		SetCheckBox( CheckBox::SHOW_NORMALS, cvar.GetBool() );
	}
}
}