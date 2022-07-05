#include "Input.h"
#include "Game.h"


static float _ignore = 0;
static bool lastIntersect = false;
static kl::float3 lastDirection = {};
static const kl::sphere sphere = { { 0.0f }, 1.0f };

kl::ray GetMouseRay() {
	const kl::float2 frameSize = Game::window.size();
	const kl::float2 ndc = {
		float(Game::window.mouse.position.x),
		float(frameSize.y - Game::window.mouse.position.y)
	};
	return { Game::camera, ndc / frameSize * 2.0f - 1.0f };
}

void SaveLastValues() {
	lastIntersect = GetMouseRay().intersect(sphere, lastDirection, _ignore);
	lastDirection = lastDirection.normalize();
}

void Input::Initialize() {
	Game::window.mouse.lmb.press = [&]() {
		SaveLastValues();
	};
	Game::window.mouse.lmb.down = [&]() {
		kl::float3 currentDirection;
		if (lastIntersect && GetMouseRay().intersect(sphere, currentDirection, _ignore)) {
			currentDirection = currentDirection.normalize();

			const kl::float2 deltaAngles = {
				kl::float2(lastDirection.z, lastDirection.y).angle({ currentDirection.z, currentDirection.y }, true),
				kl::float2(lastDirection.x, lastDirection.z).angle({ currentDirection.x, currentDirection.z }, true)
			};

			Game::sphereRotation.y -= deltaAngles.y;

			const float cameraAngleX = kl::to::degrees(std::asin(Game::camera.forward().y));
			const float newCameraAngleX = kl::math::minmax(cameraAngleX - deltaAngles.x, -85.0f, 85.0f);
			const float newCameraAngleXRads = kl::to::radians(newCameraAngleX);
			Game::camera.forward({ 0.0f, std::sin(newCameraAngleXRads), std::cos(newCameraAngleXRads) });
			Game::camera.position = Game::camera.forward() * -2.0f;

			SaveLastValues();
		}
	};

	Game::window.keys.f11.press = [&]() {
		static bool previousState = false;
		previousState = !previousState;
		Game::window.fullscreen(previousState);
	};
}

void Input::Update() {
	static int lastScroll = 0;
	const int scrollDelta = lastScroll - Game::window.mouse.scroll;
	Game::camera.fov = kl::math::minmax(Game::camera.fov + scrollDelta * 5.0f, 5.0f, 90.0f);
	lastScroll = Game::window.mouse.scroll;

	kl::float3 mouseSphereIntersect;
	GetMouseRay().intersect(sphere, mouseSphereIntersect, _ignore);

	const kl::float3 mouseSphereIntersectNoY = kl::float3(mouseSphereIntersect.x, 0.0f, mouseSphereIntersect.z);
	mouseGeoLocation.x = mouseSphereIntersect.angle(mouseSphereIntersectNoY);
	mouseGeoLocation.x *= (mouseSphereIntersect.y < 0.0f) ? -1.0f : 1.0f;

	const kl::float3 greenwich = (kl::mat4::rotation(Game::sphereRotation) * kl::float4(1.0f, 0.0f, 0.0f, 1.0f)).xyz;
	mouseGeoLocation.y = kl::float2(greenwich.x, greenwich.z).angle(
		kl::float2(mouseSphereIntersectNoY.x, mouseSphereIntersectNoY.z), true);
}
