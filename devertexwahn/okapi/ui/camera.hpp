/*
 *  Note: Contains derived and modified code from BlueFramework which is under Apache-2.0 license and
 *  copyrighted by Technical University of Munich, Chair of Computational Modeling and Simulation
 */

#pragma once

#include "core/namespace.hpp"
#include "core/reference_counted.hpp"
#include "math/constants.hpp"
#include "math/matrix.hpp"
#include "math/util.hpp"
#include "math/vector.hpp"

DE_VERTEXWAHN_BEGIN_NAMESPACE

struct CameraTransformation {
    static Vector3f view_direction_to_rotation(const Vector3f &dir);

    CameraTransformation();
    CameraTransformation(const Vector3f &translation, const Vector3f &rotation, const float offset);

    [[nodiscard]] const Vector3f &translation() const;
    Vector3f &translation();

    [[nodiscard]] const Vector3f &rotation() const;
    Vector3f &rotation();

    [[nodiscard]] const float &offset() const;
    float &offset();

    [[nodiscard]] const float &x() const;
    float &x();

    [[nodiscard]] const float &y() const;
    float &y();

    [[nodiscard]] const float &z() const;
    float &z();

    [[nodiscard]] const float &yaw() const;
    float &yaw();

    [[nodiscard]] const float &pitch() const;
    float &pitch();

    [[nodiscard]] const float &roll() const;
    float &roll();

    CameraTransformation operator+(const CameraTransformation &rhs) const;
    CameraTransformation operator-(const CameraTransformation &rhs) const;
    CameraTransformation operator*(const float rhs) const;
    CameraTransformation operator/(const float rhs) const;

    CameraTransformation &operator+=(const CameraTransformation &rhs);
    CameraTransformation &operator-=(const CameraTransformation &rhs);
    CameraTransformation &operator*=(const float rhs);
    CameraTransformation &operator/=(const float rhs);

    [[nodiscard]] float length() const;
    void normalize();

    [[nodiscard]] Matrix44f rotation_matrix() const;
    [[nodiscard]] Matrix44f transformation_matrix() const;
    [[nodiscard]] Matrix44f view_matrix() const;

    void set_view_direction(const Vector3f &dir);
    void fit_to_view(const Vector3f &min, const Vector3f &max, float field_of_view);
    void look_at(const Vector3f &target, const Vector3f &origin);

private:
    Vector3f translation_;
    Vector3f rotation_; // TODO(Vertexwahn): Switch to quaternion
    float offset_ = 0.f;
};

enum class eProjectionType { Perspective, Orthographic };

struct CameraFrustum {
public:
    CameraFrustum(float left,
                  float right,
                  float bottom,
                  float top,
                  float near,
                  float far,
                  eProjectionType projectionType,
                  float fieldOfView = pi_over_4f,
                  float scaling = 1);
    CameraFrustum(float width, float height, float near, float far, eProjectionType projectionType, float fieldOfView = pi_over_4, float scaling = 1);

    [[nodiscard]] float width() const;
    [[nodiscard]] float height() const;
    [[nodiscard]] float depth() const;

    void width(float w);
    void height(float h);
    void depth(float d);

    [[nodiscard]] float field_of_view() const;
    void field_of_view(float f);

    [[nodiscard]] Matrix44f projection_matrix() const;

private:
    float left_plane_, right_plane_;
    float bottom_plane_, top_plane_;
    float near_plane_, far_plane_;

    eProjectionType projection_type_;
    float field_of_view_;
    float scaling_;
};

class Camera {
public:
    Camera();

    void tick(const float delta);

    [[nodiscard]] const CameraTransformation &transformation() const;
    CameraTransformation &transformation();

    [[nodiscard]] const CameraTransformation &velocity() const;
    CameraTransformation &velocity();

    [[nodiscard]] const CameraFrustum &frustum() const;
    CameraFrustum &frustum();

    [[nodiscard]] Matrix44f view_projection_matrix() const;

    [[nodiscard]] Vector4f project(const Vector4f &worldSpace) const;
    [[nodiscard]] Vector4f deproject(const Vector3f &screenSpace) const;

    void pan_camera_world(const Vector2f &screen_space_delta);
    void pan_camera_world(const Vector2f &screen_space_delta, const Vector3f &world_space_reference);
    void pan_camera_screen(const Vector2f &screen_space_delta, const Vector3f &screen_space_reference);

    void move_camera_global(const Vector3f &worldSpaceDelta);
    void move_camera_local(const Vector3f &localDelta);

    void orbit_camera(float yaw);
    void pitch_camera(float pitch);
    void roll_camera(float roll);
    void rotate_camera(const Vector3f &rotation);

    void pull_camera_out(float delta);
    void pull_camera_in(float delta);

    void set_offset(float offset);

    void set_view_direction(const Vector3f &dir);
    void fit_to_view(const Vector3f &min, const Vector3f &max);
    void look_at(const Vector3f &target, const Vector3f &origin);

private:
    CameraTransformation transformation_, velocity_;
    CameraFrustum frustum_;
};

enum eViewDirection : short {
    Front = 0b00000001,
    Back = 0b00000010,
    Top = 0b00000100,
    Bottom = 0b00001000,
    Left = 0b00010000,
    Right = 0b00100000,
    TopLeftFront = Top | Front | Left,
    TopFront = Top | Front,
    TopFrontRight = Top | Front | Right,
    FrontLeft = Front | Left,
    FrontRight = Front | Right,
    FrontLeftBottom = Front | Left | Bottom,
    FrontBottom = Front | Bottom,
    FrontRightBottom = Front | Right | Bottom,
    TopRight = Top | Right,
    TopRightBack = Top | Right | Back,
    RightBack = Right | Back,
    RightBottom = Right | Bottom,
    RightBottomBack = Right | Back | Bottom,
    TopBack = Top | Back,
    TopLeftBack = Top | Back | Left,
    BackLeft = Back | Left,
    BackBottom = Back | Bottom,
    BackLeftBottom = Back | Bottom | Left,
    TopLeft = Top | Left,
    LeftBottom = Bottom | Left
};

class CameraController {
public:
    enum class eState { Orbiting, Free };

    enum class eKey { MoveForward, MoveBackward, MoveRight, MoveLeft, MoveUp, MoveDown, ViewForward, ViewBackward, ViewRight, ViewLeft, ViewUp, ViewDown };

public:
    CameraController();
    CameraController(ReferenceCounted<Camera> camera);

    void set_state(eState state);
    eState state();

    void tick(const float delta);

    void set_view_direction(const Vector3f &dir);
    void set_view_direction(const Vector3f &dir, const float duration);

    void fit_to_view(const Vector3f &min, const Vector3f &max, const float duration = 1.0f);
    void look_at(const Vector3f &target, const Vector3f &origin, const float duration = 1.0f);

    void handleKeyDown(eKey key);
    void handleKeyUp(eKey key);

    void handle_wheel(float delta);

    ReferenceCounted<Camera> camera();

    [[nodiscard]] const bool is_camera_moving() const;

    static Vector3f get_view_direction_vector(const eViewDirection &direction);

private:
    void start_interpolation(float duration, const CameraTransformation &target);

private:
    ReferenceCounted<Camera> camera_;

    eState state_;

    bool interpolating_;
    float duration_;
    CameraTransformation target_;

    Vector3f moveDirection_;
    float velocity_;
};

DE_VERTEXWAHN_END_NAMESPACE
