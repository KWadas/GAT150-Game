#pragma once
#include "RenderComponent.h"
#include "Renderer/Model.h"

namespace kiko
{
	class ModelRenderComponent : public RenderComponent
	{
	public:
		void Update(float dt) override;
		void Draw(class Renderer& renderer) override;

	public:
		res_t<Model> m_texture;
	};
}