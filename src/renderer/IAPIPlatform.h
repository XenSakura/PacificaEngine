#pragma once

class IAPIPlatform
{
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;

	virtual void BeginFrame() = 0;

	virtual void Submit() = 0;

	virtual void EndFrame() = 0;

	virtual void resize() = 0;

	virtual void present() = 0;

	virtual bool reloadShaders() = 0;
	//// Use hot-reload the shaders
	//virtual bool reloadShaders(Resources& res, Scene& scene) = 0;
};