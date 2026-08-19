#pragma once

#include "helper.h"


struct ConguessInput
{
    static constexpr kl::Sphere EARTH_SPHERE{ {}, 1.0f };
    static constexpr float TESTED_CAM_DIST = 2.25f;
    static constexpr float MIN_CAMERA_DISTANCE = 1.05f;
    static constexpr float ANGLE_MULTI = 0.05f;
    static constexpr float VERTICAL_ANGLE_LIMIT = 85.0f;
    static constexpr float SCROLL_MULTI = 0.05f;

    Conguess& conguess;

    float3 earth_rotation{ 0.0f, 180.0f, 0.0f };
    float3 camera_rotation{ 45.0f, 0.0f, 0.0f };
    float camera_distance = 3.0f;
    bool rotate_camera_y = false;

    float2 original_rotations;
    int2 original_mouse_pos;
    int original_mouse_country = -1;
    bool original_is_correct = false;

    std::optional<int> mouse_country_index;

    ConguessInput( Conguess& conguess );

    void update();

private:
    float calc_deg_per_px() const;
};
