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
	void AddCamera( std::unique_ptr<Camera> pCam );
	Camera& GetCamera();
	~CameraContainer();
	void LinkTechniques( Rgph::RenderGraph& rg );
	void Submit() const;
private:
	std::vector<std::unique_ptr<Camera>> cameras;
	int selected = 0;
};