#pragma once

#include <cstdint>
#include <minwindef.h>

namespace hbqj {
    struct Matrix4x4;

    // auto view_matrix_offset = process->GetOffsetAddr(manager_.GetSignature(SignatureType::ViewMatrix).value()->addr);
    // process->CalculateTargetOffsetCall(view_matrix_offset).value() = 4EA330
    static const unsigned long long get_view_matrix_func_offset = 0x4EA330;

    typedef int64_t (WINAPI *GetViewMatrixFunc)();

    GetViewMatrixFunc g_get_view_matrix_func;

    int64_t g_view_matrix_addr = 0;

    Matrix4x4 *g_view_matrix = nullptr;

    static int64_t GetViewMatrixHook() {
        int64_t result = g_get_view_matrix_func();

        g_view_matrix_addr = result + 0x1b4;
        g_view_matrix = reinterpret_cast<Matrix4x4 *>(result + 0x1b4);

        return result;
    }


#pragma pack(push, 1)

    struct Matrix4x4 {
        union {
            struct {
                float m11;
                float m12;
                float m13;
                float m14;
                float m21;
                float m22;
                float m23;
                float m24;
                float m31;
                float m32;
                float m33;
                float m34;
                float m41;
                float m42;
                float m43;
                float m44;
            };
            float matrix[16];
        };
    };

    struct Vector3 {
        float x;
        float y;
        float z;
    };

    // refer to
    // https://github.com/aers/FFXIVClientStructs/blob/89c97bc602b689ac1948cfc523dca6182360534b/FFXIVClientStructs/FFXIV/Client/Game/Camera.cs
    struct RenderCamera {
        uint8_t _pad0[0x10];
        Matrix4x4 view_matrix;                      // 0x10
        Matrix4x4 projection_matrix_2;              // 0x50
        Vector3 origin;                             // 0x90
        uint8_t _pad3[0x1A0 - 0x90 - sizeof(Vector3)];
        Matrix4x4 projection_matrix;                // 0x1A0
        uint8_t _pad4[0x1E8 - 0x1A0 - sizeof(Matrix4x4)];
        float fov;                                  // 0x1E8
        float aspect_ratio;                         // 0x1EC
        float near_plane;                           // 0x1F0
        float far_plane;                            // 0x1F4
        float ortho_height;                         // 0x1F8
        bool is_ortho;                              // 0x1FC
        uint8_t _pad5[2];                          // 0x1FD
        bool standard_z;                            // 0x1FF
        bool finite_far_plane;                      // 0x200
        uint8_t _pad6[0x290 - 0x201];
    };

    struct Camera {
        uint8_t _pad0[0x80];
        Vector3 lookAtVector;                       // 0x80
        uint8_t _pad1[0x90 - 0x80 - sizeof(Vector3)];
        Vector3 vector_1;                           // 0x90
        uint8_t _pad2[0xA0 - 0x90 - sizeof(Vector3)];
        Matrix4x4 viewMatrix;                       // 0xA0
        RenderCamera *render_camera;               // 0xE0
        uint8_t _pad4[0xF0 - 0xE0 - sizeof(RenderCamera *)];

        // void ScreenPointToRay(Ray* ray, int x, int y);
        // static Vector2* WorldToScreenPoint(Vector2* screenPoint, Vector3* worldPoint);
    };

    struct CameraBase {
        uint8_t _pad0[0x10];
        Camera scene_camera;         // 0x10
        // uint8_t _pad1[0x100 - 0x10 - sizeof(Camera)];
        uint32_t unk_UInt;                           // 0x100
        uint8_t _pad2[0x108 - 0x100 - sizeof(uint32_t)];
        uint32_t unk_Flags;                          // 0x108
        uint8_t _pad3[0x110 - 0x108 - sizeof(uint32_t)];
    };

#pragma pack(pop)

    static const unsigned long long g_get_active_camera_offset = 0x16488E0;
    CameraBase *active_camera = nullptr;

    typedef int64_t (WINAPI *GetActiveCameraFunc)();

    GetActiveCameraFunc g_get_active_camera_func = nullptr;

    static int64_t GetActiveCameraHook() {
        int64_t result = g_get_active_camera_func();

        active_camera = reinterpret_cast<CameraBase *>(result);

        return result;
    }
}