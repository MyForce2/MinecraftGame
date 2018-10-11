#pragma once

#include "Vectors/Vec3.h"
#include "Mat4.h"

namespace Engine {
	namespace Math {

		struct Mat3 {

		private:
			static const unsigned int DATA_SIZE = 9U;
			static const unsigned int MATRIX_DIMENSIONS = 3;

		public:
			union {
				float data[DATA_SIZE];
				Vec3 columns[MATRIX_DIMENSIONS];
			};

			Mat3();
			Mat3(float mainDiagonal);
			Mat3(const Mat3& mat);
			Mat3(const Mat4& mat);
			~Mat3();

			// Operators
			Mat3 operator*(const Mat3& mat) const;
			Mat3 operator*(float value) const;
			Mat3 operator/(float value) const;
			Mat3 operator+(float value) const;
			Mat3 operator-(float value) const;
			Vec3 operator*(const Vec3& vec) const;

			void operator*=(const Mat3& mat);
			void operator*=(float value);
			void operator/=(float value);
			void operator+=(float value);
			void operator-=(float value);

			Vec3& operator[](size_t index);
			Vec3 operator[](size_t index) const;

			bool operator==(const Mat3& mat) const;
			bool operator!=(const Mat3& mat) const;

		private:

			// More operators

			friend Mat3 operator-(const Mat3& mat);
		};
	}
}
