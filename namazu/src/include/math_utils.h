#pragma once

#include <array>
#include <algorithm>
#include <cmath>

namespace hbqj {
    static constexpr double Pi = 3.14159265358979323846;
    static constexpr double RadToDeg = 360 / (Pi * 2);
    static constexpr double DegToRad = (Pi * 2) / 360;

    // Convert quaternion to euler angles (in degrees)
    std::array<float, 3> QuaternionToEulerAngles(const Quaternion &quaternion) {
        Quaternion q{};
        // remap quaternion components to match target coordinate system
        q.x = quaternion.w;
        q.y = quaternion.z;
        q.z = quaternion.x;
        q.w = quaternion.y;

        auto x = static_cast<float>(asin(2. * (q.x * q.z - q.w * q.y)));
        auto y = static_cast<float>(atan2(2. * q.x * q.w + 2. * q.y * q.z, 1 - 2. * (q.z * q.z + q.w * q.w)));
        auto z = static_cast<float>(atan2(2. * q.x * q.y + 2. * q.z * q.w, 1 - 2. * (q.y * q.y + q.z * q.z)));

        x *= RadToDeg;
        y *= RadToDeg;
        z *= RadToDeg;

        return {x, y, z};
    }

    void Frustum(float left, float right, float bottom, float top, float znear, float zfar, float *m16) {
        float temp, temp2, temp3, temp4;
        temp = 2.0f * znear;
        temp2 = right - left;
        temp3 = top - bottom;
        temp4 = zfar - znear;
        m16[0] = temp / temp2;
        m16[1] = 0.0;
        m16[2] = 0.0;
        m16[3] = 0.0;
        m16[4] = 0.0;
        m16[5] = temp / temp3;
        m16[6] = 0.0;
        m16[7] = 0.0;
        m16[8] = (right + left) / temp2;
        m16[9] = (top + bottom) / temp3;
        m16[10] = (-zfar - znear) / temp4;
        m16[11] = -1.0f;
        m16[12] = 0.0;
        m16[13] = 0.0;
        m16[14] = (-temp * zfar) / temp4;
        m16[15] = 0.0;
    }

    void Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar, float *m16) {
        float ymax, xmax;
        ymax = znear * tanf(fovyInDegrees * 3.141592f / 180.0f);
        xmax = ymax * aspectRatio;
        Frustum(-xmax, xmax, -ymax, ymax, znear, zfar, m16);
    }

    void Cross(const float *a, const float *b, float *r) {
        r[0] = a[1] * b[2] - a[2] * b[1];
        r[1] = a[2] * b[0] - a[0] * b[2];
        r[2] = a[0] * b[1] - a[1] * b[0];
    }

    float Dot(const float *a, const float *b) {
        return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    }

    void Normalize(const float *a, float *r) {
        float il = 1.f / (sqrtf(Dot(a, a)) + FLT_EPSILON);
        r[0] = a[0] * il;
        r[1] = a[1] * il;
        r[2] = a[2] * il;
    }

    void LookAt(const float *eye, const float *at, const float *up, float *m16) {
        float X[3], Y[3], Z[3], tmp[3];

        tmp[0] = eye[0] - at[0];
        tmp[1] = eye[1] - at[1];
        tmp[2] = eye[2] - at[2];
        Normalize(tmp, Z);
        Normalize(up, Y);

        Cross(Y, Z, tmp);
        Normalize(tmp, X);

        Cross(Z, X, tmp);
        Normalize(tmp, Y);

        m16[0] = X[0];
        m16[1] = Y[0];
        m16[2] = Z[0];
        m16[3] = 0.0f;
        m16[4] = X[1];
        m16[5] = Y[1];
        m16[6] = Z[1];
        m16[7] = 0.0f;
        m16[8] = X[2];
        m16[9] = Y[2];
        m16[10] = Z[2];
        m16[11] = 0.0f;
        m16[12] = -Dot(X, eye);
        m16[13] = -Dot(Y, eye);
        m16[14] = -Dot(Z, eye);
        m16[15] = 1.0f;
    }
}