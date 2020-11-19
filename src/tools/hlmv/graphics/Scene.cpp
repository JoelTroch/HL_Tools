#include <GL/glew.h>

#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/shared/CWorldTime.h"
#include "core/shared/Utility.h"

#include "engine/renderer/studiomodel/CStudioModelRenderer.h"
#include "engine/shared/renderer/studiomodel/IStudioModelRenderer.h"
#include "entity/CHLMVStudioModelEntity.h"

#include "game/entity/CBaseEntityList.h"
#include "game/entity/CEntityManager.h"

#include "graphics/GraphicsHelpers.h"
#include "graphics/GraphicsUtils.h"
#include "graphics/Scene.hpp"

#include "utility/Color.h"

namespace graphics
{
static const int CROSSHAIR_LINE_WIDTH = 3;
static const int CROSSHAIR_LINE_START = 5;
static const int CROSSHAIR_LINE_LENGTH = 10;
static const int CROSSHAIR_LINE_END = CROSSHAIR_LINE_START + CROSSHAIR_LINE_LENGTH;

static const int GUIDELINES_LINE_WIDTH = 1;
static const int GUIDELINES_LINE_LENGTH = 5;
static const int GUIDELINES_POINT_LINE_OFFSET = 2;
static const int GUIDELINES_OFFSET = GUIDELINES_LINE_LENGTH + (GUIDELINES_POINT_LINE_OFFSET * 2) + GUIDELINES_LINE_WIDTH;

static const int GUIDELINES_EDGE_WIDTH = 4;

Scene::Scene()
	: _studioModelRenderer(std::make_unique<studiomdl::CStudioModelRenderer>())
	, _worldTime(std::make_unique<CWorldTime>())
	//Use the default list class for now
	, _entityManager(std::make_unique<CEntityManager>(std::make_unique<CBaseEntityList>(), _worldTime.get()))
	, _entityContext(std::make_unique<EntityContext>(_worldTime.get(), _studioModelRenderer.get(), _entityManager->GetEntityList(), _entityManager.get()))
{
}

Scene::~Scene() = default;

void Scene::SetEntity(CHLMVStudioModelEntity* entity)
{
	_entity = entity;

	glm::vec3 min, max;
	_entity->ExtractBbox(min, max);

	//Clamp the values to a reasonable range
	for (int i = 0; i < 3; ++i)
	{
		//Use different limits for min and max so centering won't end up setting origin to 0 0 0
		min[i] = clamp(min[i], -2000.f, 2000.f);
		max[i] = clamp(max[i], -1000.f, 1000.f);
	}

	float dx = max[0] - min[0];
	float dy = max[1] - min[1];
	float dz = max[2] - min[2];

	float d = dx;

	if (dy > d)
		d = dy;
	if (dz > d)
		d = dz;

	glm::vec3 trans;
	glm::vec3 rot;

	trans[2] = 0;
	trans[0] = -(min[2] + dz / 2);
	trans[1] = d * 1.0f;
	rot[0] = -90.0f;
	rot[1] = 0.0f;
	rot[2] = -90.0f;

	_camera.SetOrigin(trans);
	_camera.SetViewDirection(rot);
}

void Scene::Initialize()
{
	if (!_studioModelRenderer->Initialize())
	{
		//TODO: handle error
	}

	if (nullptr != _entity)
	{
		//TODO: should be replaced with an on-demand resource uploading stage in Draw()
		_entity->GetModel()->CreateTextures();
	}
}

void Scene::Shutdown()
{
	_studioModelRenderer->Shutdown();
}

void Scene::Tick()
{
	const double flCurTime = GetCurrentTime();

	double flFrameTime = flCurTime - _worldTime->GetPreviousRealTime();

	_worldTime->SetRealTime(flCurTime);

	if (flFrameTime > 1.0)
		flFrameTime = 0.1;

	//TODO: implement frame limiter setting
	//TODO: investigate how to allow animation to work when framerate is very high
	if (flFrameTime < (1.0 / /*max_fps.GetFloat()*/60.0f))
	{
		return;
	}

	_worldTime->TimeChanged(flCurTime);

	_entityManager->RunFrame();
}

void Scene::Draw()
{
	glClearColor(_backgroundColor.r, _backgroundColor.g, _backgroundColor.b, 1.0f);

	if (ShouldMirrorOnGround())
	{
		glClearStencil(0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, _windowWidth, _windowHeight);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	_drawnPolygonsCount = 0;

	//TODO: implement
#if false
	if (ShouldShowTexture())
	{
		DrawTexture(m_pHLMV->GetState()->texture, m_pHLMV->GetState()->textureScale,
			m_pHLMV->GetState()->showUVMap, m_pHLMV->GetState()->overlayUVMap,
			m_pHLMV->GetState()->antiAliasUVLines, m_pHLMV->GetState()->pUVMesh);
	}
	else
#endif
	{
		DrawModel();
	}

	//TODO: implement
#if false
	const int centerX = _windowWidth / 2;
	const int centerY = _windowHeight / 2;

	if (m_pHLMV->GetState()->drawCrosshair)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(0.0f, (float)_windowWidth, (float)_windowHeight, 0.0f, 1.0f, -1.0f);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_CULL_FACE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_TEXTURE_2D);

		//TODO:
		const Color crosshairColor = Color(255, 0, 0);//; m_pHLMV->GetSettings()->GetCrosshairColor();

		glColor4f(crosshairColor.GetRed() / 255.0f, crosshairColor.GetGreen() / 255.0f, crosshairColor.GetBlue() / 255.0f, 1.0);

		glPointSize(CROSSHAIR_LINE_WIDTH);
		glLineWidth(CROSSHAIR_LINE_WIDTH);

		glBegin(GL_POINTS);

		glVertex2f(centerX - CROSSHAIR_LINE_WIDTH / 2, centerY + 1);

		glEnd();

		glBegin(GL_LINES);

		glVertex2f(centerX - CROSSHAIR_LINE_START, centerY);
		glVertex2f(centerX - CROSSHAIR_LINE_END, centerY);

		glVertex2f(centerX + CROSSHAIR_LINE_START, centerY);
		glVertex2f(centerX + CROSSHAIR_LINE_END, centerY);

		glVertex2f(centerX, centerY - CROSSHAIR_LINE_START);
		glVertex2f(centerX, centerY - CROSSHAIR_LINE_END);

		glVertex2f(centerX, centerY + CROSSHAIR_LINE_START);
		glVertex2f(centerX, centerY + CROSSHAIR_LINE_END);

		glEnd();

		glPointSize(1);
		glLineWidth(1);

		glPopMatrix();
	}

	if (m_pHLMV->GetState()->drawGuidelines)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(0.0f, (float)_windowWidth, (float)_windowHeight, 0.0f, 1.0f, -1.0f);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_CULL_FACE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_TEXTURE_2D);

		const Color& crosshairColor = m_pHLMV->GetSettings()->GetCrosshairColor();

		glColor4f(crosshairColor.GetRed() / 255.0f, crosshairColor.GetGreen() / 255.0f, crosshairColor.GetBlue() / 255.0f, 1.0);

		glPointSize(GUIDELINES_LINE_WIDTH);
		glLineWidth(GUIDELINES_LINE_WIDTH);

		glBegin(GL_POINTS);

		for (int yPos = _windowHeight - GUIDELINES_LINE_LENGTH; yPos >= centerY + CROSSHAIR_LINE_END; yPos -= GUIDELINES_OFFSET)
		{
			glVertex2f(centerX - GUIDELINES_LINE_WIDTH, yPos);
		}

		glEnd();

		glBegin(GL_LINES);

		for (int yPos = _windowHeight - GUIDELINES_LINE_LENGTH - GUIDELINES_POINT_LINE_OFFSET - GUIDELINES_LINE_WIDTH;
			yPos >= centerY + CROSSHAIR_LINE_END + GUIDELINES_LINE_LENGTH;
			yPos -= GUIDELINES_OFFSET)
		{
			glVertex2f(centerX, yPos);
			glVertex2f(centerX, yPos - GUIDELINES_LINE_LENGTH);
		}

		glEnd();

		const float flWidth = _windowHeight * (16 / 9.0);

		glLineWidth(GUIDELINES_EDGE_WIDTH);

		glBegin(GL_LINES);

		glVertex2f((_windowWidth / 2) - (flWidth / 2), 0);
		glVertex2f((_windowWidth / 2) - (flWidth / 2), _windowHeight);

		glVertex2f((_windowWidth / 2) + (flWidth / 2), 0);
		glVertex2f((_windowWidth / 2) + (flWidth / 2), _windowHeight);

		glEnd();

		glPointSize(1);
		glLineWidth(1);

		glPopMatrix();
	}
#endif
}

void Scene::ApplyCameraToScene()
{
	//TODO: reimplement cameras
	auto pCamera = &_camera;// m_pHLMV->GetState()->GetCurrentCamera();

	const auto& vecOrigin = pCamera->GetOrigin();
	const auto vecAngles = pCamera->GetViewDirection();

	const glm::mat4x4 identity = Mat4x4ModelView();

	auto mat = Mat4x4ModelView();

	mat *= glm::translate(-vecOrigin);

	mat *= glm::rotate(glm::radians(vecAngles[2]), glm::vec3{1, 0, 0});

	mat *= glm::rotate(glm::radians(vecAngles[0]), glm::vec3{0, 1, 0});

	mat *= glm::rotate(glm::radians(vecAngles[1]), glm::vec3{0, 0, 1});

	glLoadMatrixf(glm::value_ptr(mat));
}

void Scene::SetupRenderMode(RenderMode renderMode)
{
	//TODO: implement
#if false
	if (renderMode == RenderMode::INVALID)
		renderMode = m_pHLMV->GetState()->renderMode;

	graphics::helpers::SetupRenderMode(renderMode, m_pHLMV->GetState()->backfaceCulling);
#endif

	graphics::helpers::SetupRenderMode(RenderMode::TEXTURE_SHADED, true);
}

void Scene::DrawModel()
{
	//
	// draw background
	//

	//TODO: implement
#if false
	if (m_pHLMV->GetState()->showBackground && m_BackgroundTexture != GL_INVALID_TEXTURE_ID && !m_pHLMV->GetState()->showTexture)
	{
		graphics::DrawBackground(m_BackgroundTexture);
	}
#endif

	//TODO: implement
	graphics::SetProjection(/*m_pHLMV->GetState()->GetCurrentFOV()*/65.f, _windowWidth, _windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	ApplyCameraToScene();

	//TODO: implement
#if false
	if (m_pHLMV->GetState()->drawAxes)
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);

		const float flLength = 50.0f;

		glLineWidth(1.0f);

		glBegin(GL_LINES);

		glColor3f(1.0f, 0, 0);

		glVertex3f(0, 0, 0);
		glVertex3f(flLength, 0, 0);

		glColor3f(0, 1, 0);

		glVertex3f(0, 0, 0);
		glVertex3f(0, flLength, 0);

		glColor3f(0, 0, 1.0f);

		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, flLength);

		glEnd();
	}
#endif

	//TODO: reimplement cameras
	auto camera = &_camera;//m_pHLMV->GetState()->GetCurrentCamera()

	const auto vecAngles = camera->GetViewDirection();

	auto mat = Mat4x4ModelView();

	mat *= glm::translate(-camera->GetOrigin());

	mat *= glm::rotate(glm::radians(vecAngles[2]), glm::vec3{1, 0, 0});

	mat *= glm::rotate(glm::radians(vecAngles[0]), glm::vec3{0, 1, 0});

	mat *= glm::rotate(glm::radians(vecAngles[1]), glm::vec3{0, 0, 1});

	const auto vecAbsOrigin = glm::inverse(mat)[3];

	_studioModelRenderer->SetViewerOrigin(glm::vec3(vecAbsOrigin));

	//Originally this was calculated as:
	//vecViewerRight[ 0 ] = vecViewerRight[ 1 ] = vecOrigin[ 2 ];
	//But that vector was incorrect. It mostly affects chrome because of its reflective nature.

	//Grab the angles that the player would have in-game. Since model viewer rotates the world, rather than moving the camera, this has to be adjusted.
	glm::vec3 angViewerDir = -camera->GetViewDirection();

	angViewerDir = angViewerDir + 180.0f;

	glm::vec3 vecViewerRight;

	//We're using the up vector here since the in-game look can only be matched if chrome is rotated.
	AngleVectors(angViewerDir, nullptr, nullptr, &vecViewerRight);

	//Invert it so it points down instead of up. This allows chrome to match the in-game look.
	_studioModelRenderer->SetViewerRight(-vecViewerRight);

	const unsigned int uiOldPolys = _studioModelRenderer->GetDrawnPolygonsCount();

	if (nullptr != _entity)
	{
		// setup stencil buffer and draw mirror
		//TODO: implement
#if false
		if (m_pHLMV->GetState()->mirror)
		{
			graphics::helpers::DrawMirroredModel(*_studioModelRenderer, _entity,
				m_pHLMV->GetState()->renderMode,
				m_pHLMV->GetState()->wireframeOverlay,
				m_pHLMV->GetSettings()->GetFloorLength(),
				m_pHLMV->GetState()->backfaceCulling);
		}
#endif
	}

	SetupRenderMode();

	if (nullptr != _entity)
	{
		const glm::vec3& vecScale = _entity->GetScale();

		//Determine if an odd number of scale values are negative. The cull face has to be changed if so.
		const float flScale = vecScale.x * vecScale.y * vecScale.z;

		glCullFace(flScale > 0 ? GL_FRONT : GL_BACK);

		renderer::DrawFlags_t flags = renderer::DrawFlag::NONE;

		//Draw wireframe overlay
		//TODO: implement
#if false
		if (m_pHLMV->GetState()->wireframeOverlay)
		{
			flags |= renderer::DrawFlag::WIREFRAME_OVERLAY;
		}

		if (m_pHLMV->GetState()->UsingWeaponOrigin())
		{
			flags |= renderer::DrawFlag::IS_VIEW_MODEL;
		}

		if (m_pHLMV->GetState()->drawShadows)
		{
			flags |= renderer::DrawFlag::DRAW_SHADOWS;
		}

		if (m_pHLMV->GetState()->fixShadowZFighting)
		{
			flags |= renderer::DrawFlag::FIX_SHADOW_Z_FIGHTING;
		}
#endif

		_entity->Draw(flags);
	}

	//
	// draw ground
	//

	//TODO: implement
#if false
	if (m_pHLMV->GetState()->showGround)
	{
		graphics::helpers::DrawFloor(m_pHLMV->GetSettings()->GetFloorLength(), m_GroundTexture, m_pHLMV->GetSettings()->GetGroundColor(), m_pHLMV->GetState()->mirror);
	}
#endif

	//TODO: remove
	graphics::helpers::DrawFloor(100, 0, Color(1, 0, 0), false);

	_drawnPolygonsCount = _studioModelRenderer->GetDrawnPolygonsCount() - uiOldPolys;

	//TODO: implement
#if false
	if (m_pHLMV->GetState()->drawPlayerHitbox)
	{
		//Draw a transparent green box to display the player hitbox
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(0.0f, 1.0f, 0.0f, 0.5f);

		const glm::vec3 bbmin{-16, -16, 0};
		const glm::vec3 bbmax{16, 16, 72};

		glm::vec3 v[8];

		v[0][0] = bbmin[0];
		v[0][1] = bbmax[1];
		v[0][2] = bbmin[2];

		v[1][0] = bbmin[0];
		v[1][1] = bbmin[1];
		v[1][2] = bbmin[2];

		v[2][0] = bbmax[0];
		v[2][1] = bbmax[1];
		v[2][2] = bbmin[2];

		v[3][0] = bbmax[0];
		v[3][1] = bbmin[1];
		v[3][2] = bbmin[2];

		v[4][0] = bbmax[0];
		v[4][1] = bbmax[1];
		v[4][2] = bbmax[2];

		v[5][0] = bbmax[0];
		v[5][1] = bbmin[1];
		v[5][2] = bbmax[2];

		v[6][0] = bbmin[0];
		v[6][1] = bbmax[1];
		v[6][2] = bbmax[2];

		v[7][0] = bbmin[0];
		v[7][1] = bbmin[1];
		v[7][2] = bbmax[2];

		graphics::DrawBox(v);

		//Draw dark green edges
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor4f(0.0f, 0.5f, 0.0f, 0.5f);

		graphics::DrawBox(v);
	}
#endif

	glPopMatrix();
}
}
