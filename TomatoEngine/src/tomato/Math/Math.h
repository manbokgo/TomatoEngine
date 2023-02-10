#pragma once

namespace tomato
{
	class Math
	{
	public:
		const float EPSILON = 1.17549435E-38f;

		static bool DecomposeTransform(const Matrix& transform, Vec3& outTranslation, Vec3& outRotation, Vec3& outScale);
		
		[[nodiscard]] static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
		[[nodiscard]] static Vec2 SmoothDamp(const Vec2& current, const Vec2& target, Vec2& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);
		[[nodiscard]] static Vec3 SmoothDamp(const Vec3& current, const Vec3& target, Vec3& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

		[[nodiscard]] static float InverseLerp(float a, float b, float value)
		{
			float den = b - a;
			if (den == 0.0f)
				return 0.0f;
			return (value - a) / den;
		}

		[[nodiscard]] static float InverseLerpClamped(float a, float b, float value)
		{
			float den = b - a;
			if (den == 0.0f)
				return 0.0f;
			return std::clamp((value - a) / den, 0.0f, 1.0f);
		}
	};
}
