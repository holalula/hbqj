#pragma once

namespace Rotation {
	static float PI = 3.14159265358979323846;
	static float Rad2Deg = (float)(360 / (PI * 2));
	static float Deg2Rad = (float)(PI * 2) / 360;

	typedef struct POS {
		float x, y, z;
	}POS;

	typedef struct QUA {
		float x, y, z, w;
	}QUA;

	std::vector<float> Qua2Deg(QUA q2) {
		QUA q;
		q.x = q2.w;
		q.y = q2.z;
		q.z = q2.x;
		q.w = q2.y;
		float Y = (float)atan2(2. * q.x * q.w + 2. * q.y * q.z, 1 - 2. * (q.z * q.z + q.w * q.w));
		float X = (float)asin(2. * (q.x * q.z - q.w * q.y));
		float Z = (float)atan2(2. * q.x * q.y + 2. * q.z * q.w, 1 - 2. * (q.y * q.y + q.z * q.z));

		X *= Rad2Deg;
		Y *= Rad2Deg;
		Z *= Rad2Deg;
		return std::vector<float>{X, Y, Z};
	}
	QUA Euler2Qua(std::vector<float> euler) {
		float xOver2 = euler[0] * Deg2Rad * 0.5f;
		float yOver2 = euler[1] * Deg2Rad * 0.5f;
		float zOver2 = euler[2] * Deg2Rad * 0.5f;

		float sinXOver2 = (float)sin(xOver2);
		float cosXOver2 = (float)cos(xOver2);
		float sinYOver2 = (float)sin(yOver2);
		float cosYOver2 = (float)cos(yOver2);
		float sinZOver2 = (float)sin(zOver2);
		float cosZOver2 = (float)cos(zOver2);

		QUA result;
		result.x = cosYOver2 * sinXOver2 * cosZOver2 + sinYOver2 * cosXOver2 * sinZOver2;
		result.y = sinYOver2 * cosXOver2 * cosZOver2 - cosYOver2 * sinXOver2 * sinZOver2;
		result.z = cosYOver2 * cosXOver2 * sinZOver2 - sinYOver2 * sinXOver2 * cosZOver2;
		result.w = cosYOver2 * cosXOver2 * cosZOver2 + sinYOver2 * sinXOver2 * sinZOver2;
		return result;
	}
}
