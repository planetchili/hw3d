#pragma once
#include <vector>
#include <memory>

class Camera;
class Graphics;
namespace Rgph
{
	class RenderGraph;
}

class CameraContainer
{
public:
	void SpawnWindow( Graphics& gfx );
	void Bind( Graphics& gfx );
	void AddCamera( std::shared_ptr<Camera> pCam );
	Camera* operator->();
	~CameraContainer();
	void LinkTechniques( Rgph::RenderGraph& rg );
	void Submit() const;
private:
	Camera& GetControlledCamera();

private:
	std::vector<std::shared_ptr<Camera>> cameras;
	int active = 0;
	int controlled = 0;
};