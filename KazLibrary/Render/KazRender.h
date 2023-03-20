#pragma once
#include"Sprite2DRender.h"
#include"Sprite3DRender.h"
#include"ObjModelRender.h"
#include"PolygonRender.h"
#include"FbxModelRender.h"
#include"CircleRender.h"
#include"LineRender.h"
#include"BoxPolygonRender.h"
#include"../Loader/FbxModelResourceMgr.h"
#include"../Loader/ObjResourceMgr.h"
#include"../Render/RenderData.h"
#include"Box2DRender.h"

typedef unique_ptr<Sprite2DRender> Sprite2DRenderPtr;
typedef unique_ptr<Sprite3DRender> Sprite3DRenderPtr;
typedef unique_ptr<ObjModelRender> ObjModelRenderPtr;
typedef unique_ptr<FbxModelRender> FbxModelRenderPtr;
typedef unique_ptr<BoxPolygonRender> BoxPolygonRenderPtr;
typedef unique_ptr<LineRender> LineRenderPtr;
typedef unique_ptr<PolygonRender> PolygonRenderPtr;
typedef unique_ptr<CircleRender> CircleRenderPtr;
