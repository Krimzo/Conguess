#include "Input.h"
#include "Render.h"
#include "Game.h"


const kl::sphere sphere = { { 0.0f }, 1.0f };

kl::float2 GetMouseNDC() {
	const kl::float2 frameSize = Render::window.size();
	const kl::float2 ndc = {
		float(Render::window.mouse.position.x),
		float(frameSize.y - Render::window.mouse.position.y)
	};
	return ndc / frameSize * 2.0f - 1.0f;
}

kl::ray GetMouseRay() {
	return { Render::camera, GetMouseNDC() };
}

bool lastIntersect = false;
kl::float3 lastDirection = {};
float _ignore = 0;

void SaveLastValues() {
	lastIntersect = GetMouseRay().intersect(sphere, lastDirection, _ignore);
	lastDirection = (lastDirection - sphere.center).normalize();
}

void Input::LMBPress() {
	SaveLastValues();
}

void Input::LMBDown() {
	kl::float3 currentDirection;
	if (lastIntersect && GetMouseRay().intersect(sphere, currentDirection, _ignore)) {
		currentDirection = (currentDirection - sphere.center).normalize();

		const kl::float2 deltaAngles = {
			kl::float2(lastDirection.z, lastDirection.y).angle({ currentDirection.z, currentDirection.y }, true),
			kl::float2(lastDirection.x, lastDirection.z).angle({ currentDirection.x, currentDirection.z }, true)
		};

		Game::sphereRotation.y -= deltaAngles.y;

		const float cameraAngleX = kl::to::degrees(std::asin(Render::camera.forward().y));
		const float newCameraAngleX = kl::math::minmax(cameraAngleX - deltaAngles.x, -85.0f, 85.0f);
		const float newCameraAngleXRads = kl::to::radians(newCameraAngleX);
		Render::camera.forward({ 0.0f, std::sin(newCameraAngleXRads), std::cos(newCameraAngleXRads) });
		Render::camera.position = Render::camera.forward() * -2.0f;

		SaveLastValues();
	}
}

int lastScroll = 0;
float scrollSensitivity = 5.0f;

void Input::Scroll() {
	const int scrollDelta = lastScroll - Render::window.mouse.scroll;
	Render::camera.fov = kl::math::minmax(Render::camera.fov + scrollDelta * scrollSensitivity, 5.0f, 90.0f);
	lastScroll = Render::window.mouse.scroll;
}
