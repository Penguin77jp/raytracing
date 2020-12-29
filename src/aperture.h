#pragma once

#include "random.h"
#include "bluenoise.h"

namespace png {
	class Aperture {
	public:
		vec3 virtual Sample(Random& _rand) = 0;
	};

	class ApertureDisk : public Aperture {
	public:
		vec3 Sample(Random& _rand) override;
	};

	class ApertureRectangle : public Aperture {
	public:
		vec3 Sample(Random& _rand) override;
	};

	class AperturePolygonBlueSplit : public Aperture {
	private:
		BlueNoiseSampler blue;
		std::vector<vec3> coner;
		unsigned int polygon;
		int GetIndex(int index, int size);
		vec3 Square2triangle(vec3 _input);
	public:
		AperturePolygonBlueSplit(unsigned int _polygon);
		vec3 Sample(Random& _rand) override;
		void CheckBluePoint(unsigned int _size, int _pointNum);
	};

	class AperturePolygonRejection : public Aperture {
	private:
		int GetIndex(int index, int size);

		float sign(vec3 p1, vec3 p2, vec3 p3);

		bool PointInTriangle(vec3 pt, vec3 v1, vec3 v2, vec3 v3);

	public:
		vec3 Sample(Random& _rand) override;
	};

	class AperturePolygonBlue : public Aperture {
	private:
		BlueNoiseSampler blue;
		std::vector<vec3> coner;
		unsigned int polygon;
		int GetIndex(int index, int size);
		vec3 Square2triangle(vec3 _input);
	public:
		AperturePolygonBlue(unsigned int _polygon);
		vec3 Sample(Random& _rand) override;
		void CheckBluePoint(unsigned int _size, int _pointNum);
	};

	class AperturePolygonBlueProgress : public Aperture {
	private:
		BlueNoiseSampler blue;
		std::vector<vec3> coner;
		unsigned int polygon;
		int GetIndex(int index, int size);
		vec3 Square2triangle(vec3 _input, const float _progress);
	public:
		AperturePolygonBlueProgress(unsigned int _polygon);
		vec3 Sample(Random& _rand) override;
		void CheckBluePoint(const unsigned int _size, const int _pointNum, const float _progress);
	};

	class ApertureBoxBlue : public Aperture {
	private:
		BlueNoiseSampler blue;
		int GetIndex(int index, int size);
		vec3 Square2triangle(vec3 _input);
	public:
		ApertureBoxBlue();
		vec3 Sample(Random& _rand) override;
		void CheckBluePoint(unsigned int _size, int _pointNum, float _progress);
	};

	class AperturePolygonSquare : public Aperture {
	private:
		BlueNoiseSampler blue;
		std::vector<vec3> coner;
		unsigned int polygon;
		int GetIndex(int index, int size);
	public:
		AperturePolygonSquare(unsigned int _polygon);
		vec3 Sample(Random& _rand) override;
	};

	class AperturePolygonBlueTest : public Aperture {
	private:
		BlueNoiseSampler blue;
		std::vector<vec3> coner;
		unsigned int polygon;
		int GetIndex(int index, int size);
		vec3 Square2triangle(vec3 _input);
	public:
		AperturePolygonBlueTest(unsigned int _polygon);
		vec3 Sample(Random& _rand) override;
		void CheckBluePoint(int _texSize, int _pointNum);
	};
}
