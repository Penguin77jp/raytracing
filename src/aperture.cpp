#include "aperture.h"

#include "ray.h"

#include <cmath>
#include <numbers>
#include <iostream>

namespace png {
	//-------------
	//-------------
	//ApertureDisk
	vec3 ApertureDisk::Sample(Random& _rand) {
		while (true) {
			png::vec3 p = png::vec3(_rand.next01() * 2.0 - 1.0, _rand.next01() * 2.0 - 1.0, 0);
			if (vec3::Magnitude(p) >= 1) continue;
			return p;
		}
	}

	//-------------
	//-------------
	//ApertureRenctangle
	vec3 ApertureRectangle::Sample(Random& _rand) {
		return vec3(2.0f * _rand.next01() - 1.0f, 2.0f * _rand.next01() - 1.0f, 0.0f);
	}

	//-------------
	//-------------
	//AperturePolygonBlueSplit
	int AperturePolygonBlueSplit::GetIndex(int index, int size) {
		if (index >= size) {
			index -= size;
		}
		return index;
	}

	vec3 AperturePolygonBlueSplit::Square2triangle(vec3 _input) {
		vec3 cal = _input;
		//std::cout << "Input :" << std::string(_input) << std::endl;
		if (_input.y > _input.x) {
			cal.x *= 0.5f;
			cal.y -= cal.x;
		} else {
			cal.y *= 0.5f;
			cal.x -= cal.y;
		}
		return cal;
	}

	AperturePolygonBlueSplit::AperturePolygonBlueSplit(unsigned int _polygon)
		: blue(Texture("blue.png"))
		, polygon(_polygon) {
		for (int i = 0; i < polygon; ++i) {
			double deg = 2.0 * std::numbers::pi * i / polygon;
			vec3 cal = vec3(std::cos(deg), std::sin(deg), 0);
			coner.push_back(cal);
		}
	}

	vec3 AperturePolygonBlueSplit::Sample(Random& _rand) {
		const double randX = _rand.next01();
		const double blueX = randX * blue.GetTextureSize();
		const unsigned int polygonIndex = (unsigned int)(randX * polygon);
		const double randY = _rand.next01();
		const double blueY = randY * blue.GetTextureSize() / polygon;
		const vec3 offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

		vec3& vec_a = coner[GetIndex(polygonIndex, coner.size())];
		vec3& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

		vec3 triVec = Square2triangle(vec3(blueX / polygon - (int)(blueX / polygon), offseted.y * polygon / blue.GetTextureSize(), 0));

		return vec_a * triVec.x + vec_b * triVec.y;
	}

	void AperturePolygonBlueSplit::CheckBluePoint(unsigned int _size, int _pointNum) {
		Random _rand(0);
		constexpr unsigned int bpp = 3;
		std::vector<unsigned char> data(_size * _size * bpp, 255);
		for (int i = 0; i < _pointNum; ++i) {
			const double randX = _rand.next01();
			const double blueX = randX * blue.GetTextureSize();
			const unsigned int polygonIndex = (unsigned int)(randX * polygon);
			const double randY = _rand.next01();
			const double blueY = randY * blue.GetTextureSize() / polygon;
			const vec3 offseted = blue.Sample(blueX - ((double)(int)(blueX * polygon) / polygon), blueY);

			vec3& vec_a = coner[GetIndex(polygonIndex, coner.size())];
			vec3& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

			vec3 triVec = Square2triangle(vec3(blueX / polygon - (int)(blueX / polygon), offseted.y * polygon / blue.GetTextureSize(), 0));
			vec3 point = vec_a * triVec.x + vec_b * triVec.y;
			const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
			const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);
			data[indexX * bpp + indexY * _size * bpp] = 0;
			data[indexX * bpp + indexY * _size * bpp + 1] = 0;
			data[indexX * bpp + indexY * _size * bpp + 2] = 0;
		}
		Texture hoge(data, _size, _size);
		hoge.WriteImage("CheckBluePointSplit.png");
	}

	//-------------
	//-------------
	//AperturePolygonRejection
	int AperturePolygonRejection::GetIndex(int index, int size) {
		if (index >= size) {
			index -= size;
		}
		return index;
	}

	float AperturePolygonRejection::sign(vec3 p1, vec3 p2, vec3 p3) {
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	bool AperturePolygonRejection::PointInTriangle(vec3 pt, vec3 v1, vec3 v2, vec3 v3) {
		bool b1, b2, b3;

		b1 = sign(pt, v1, v2) < 0.0f;
		b2 = sign(pt, v2, v3) < 0.0f;
		b3 = sign(pt, v3, v1) < 0.0f;

		return ((b1 == b2) && (b2 == b3));
	}

	vec3 AperturePolygonRejection::Sample(Random& _rand) {
		constexpr int polygon = 8;
		std::vector<vec3> coner;
		for (int i = 0; i < polygon; ++i) {
			double deg = 2.0 * std::numbers::pi * i / polygon;
			vec3 cal = vec3(std::cos(deg), std::sin(deg), 0);
			coner.push_back(cal);
		}

		while (true) {
			double x = _rand.next01() * 2.0 - 1.0;
			double y = _rand.next01() * 2.0 - 1.0;
			for (int i = 0; i < polygon; ++i) {
				vec3 tmp = vec3(x, y, 0);
				if (PointInTriangle(
					tmp,
					vec3(),
					coner[GetIndex(i, polygon)],
					coner[GetIndex(i + 1, polygon)])) {
					return tmp;
				}
			}
		}

		std::cerr << "Out of Area" << std::endl;
		exit(1);
		return coner[0];
	}

	//-------------
	//-------------
	//AperturePolygonBlue
	int AperturePolygonBlue::GetIndex(int index, int size) {
		if (index >= size) {
			index -= size;
		}
		return index;
	}

	vec3 AperturePolygonBlue::Square2triangle(vec3 _input) {
		vec3 cal = _input;
		//std::cout << "Input :" << std::string(_input) << std::endl;
		if (_input.y > _input.x) {
			cal.x *= 0.5f;
			cal.y -= cal.x;
		} else {
			cal.y *= 0.5f;
			cal.x -= cal.y;
		}
		return cal;
	}

	AperturePolygonBlue::AperturePolygonBlue(unsigned int _polygon)
		: blue(Texture("blue.png"))
		, polygon(_polygon) {
		for (int i = 0; i < polygon; ++i) {
			double deg = 2.0 * std::numbers::pi * i / polygon;
			vec3 cal = vec3(std::cos(deg), std::sin(deg), 0);
			coner.push_back(cal);
		}
	}

	vec3 AperturePolygonBlue::Sample(Random& _rand) {
		double blue_x = _rand.next01();
		const unsigned int polygonIndex = (unsigned int)(blue_x * polygon);
		double x = blue_x * polygon - polygonIndex;
		double y = _rand.next01();
		double blue_y = y / polygon;
		const vec3 offseted = blue.Sample(blue_x, blue_y);

		vec3& vec_a = coner[GetIndex(polygonIndex, coner.size())];
		vec3& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

		vec3 triVec = Square2triangle(vec3(x, y, 0));

		return vec_a * triVec.x + vec_b * triVec.y;
	}

	void AperturePolygonBlue::CheckBluePoint(unsigned int _size, int _pointNum) {
		Random _rand(0);
		constexpr unsigned int bpp = 3;
		std::vector<unsigned char> data(_size * _size * bpp, 255);
		for (int i = 0; i < _pointNum; ++i) {
			const double randX = _rand.next01() / 3;
			const double blue_x = randX * blue.GetTextureSize();
			const unsigned int polygonIndex = (unsigned int)(randX * polygon);
			const double randY = _rand.next01();
			const double blue_y = randY * blue.GetTextureSize() / polygon;
			const vec3 offseted = blue.Sample(blue_x, blue_y);

			vec3& vec_a = coner[GetIndex(polygonIndex, coner.size())];
			vec3& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

			vec3 triVec = Square2triangle(vec3(blue_x / polygon - (int)(blue_x / polygon), offseted.y * polygon / blue.GetTextureSize(), 0));
			vec3 point = vec_a * triVec.x + vec_b * triVec.y;
			const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
			const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);
			data[indexX * bpp + indexY * _size * bpp] = 0;
			data[indexX * bpp + indexY * _size * bpp + 1] = 0;
			data[indexX * bpp + indexY * _size * bpp + 2] = 0;
		}
		Texture hoge(data, _size, _size);
		hoge.WriteImage("CheckBluePoint.png");
	}

	//-------------
	//-------------
	//AperturePolygonBlueProgress
	int AperturePolygonBlueProgress::GetIndex(int index, int size) {
		if (index >= size) {
			index -= size;
		}
		return index;
	}

	vec3 AperturePolygonBlueProgress::Square2triangle(vec3 _input, const float _progress) {
		vec3 output(0.5 * _input.x, 0.5 * _input.y, 0);
		const float offset = output.y - output.x;
		if (offset > 0) {
			output.y += offset;
		} else {
			output.x -= offset;
		}
		return output * _progress + _input * (1.0 - _progress);
	}

	AperturePolygonBlueProgress::AperturePolygonBlueProgress(unsigned int _polygon)
		: blue(Texture("blue.png"))
		, polygon(_polygon) {
		if (_polygon == 2) {
			coner.push_back(vec3(1, 0, 0));
			coner.push_back(vec3(0, 1, 0));
			polygon = 2;
		} else {
			for (int i = 0; i < polygon; ++i) {
				double deg = 2.0 * std::numbers::pi * i / polygon;
				vec3 cal = vec3(std::cos(deg), std::sin(deg), 0);
				coner.push_back(cal);
			}
		}
	}

	vec3 AperturePolygonBlueProgress::Sample(Random& _rand) {
		double blue_x = _rand.next01();
		const unsigned int polygonIndex = (unsigned int)(blue_x * polygon);
		double x = blue_x * polygon - polygonIndex;
		double y = _rand.next01();
		double blue_y = y / polygon;
		const vec3 offseted = blue.Sample(blue_x, blue_y);

		vec3& vec_a = coner[GetIndex(polygonIndex, coner.size())];
		vec3& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

		vec3 triVec = Square2triangle(vec3(x, y, 0), 0);

		return vec_a * triVec.x + vec_b * triVec.y;
	}

	void AperturePolygonBlueProgress::CheckBluePoint(const unsigned int _size, const int _pointNum, const float _progress) {
		Random _rand(0);
		constexpr unsigned int bpp = 3;
		std::vector<unsigned char> data(_size * _size * bpp, 255);
		for (int i = 0; i < _pointNum; ++i) {
			const double randX = _rand.next01();
			const double blue_x = randX * blue.GetTextureSize();
			const unsigned int polygonIndex = (unsigned int)(randX * polygon);
			const double randY = _rand.next01();
			const double blue_y = randY * blue.GetTextureSize() / polygon;
			const vec3 offseted = blue.Sample(blue_x, blue_y);

			vec3& vec_a = coner[GetIndex(polygonIndex, coner.size())];
			vec3& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

			vec3 triVec = Square2triangle(vec3(blue_x / polygon - (int)(blue_x / polygon), offseted.y * polygon / blue.GetTextureSize(), 0), _progress);
			vec3 point = vec_a * triVec.x + vec_b * triVec.y;
			const int indexX = (point.x * 0.5 + 0.5) * (_size - 1);
			const int indexY = (point.y * 0.5 + 0.5) * (_size - 1);
			data[indexX * bpp + indexY * _size * bpp] = 0;
			data[indexX * bpp + indexY * _size * bpp + 1] = 0;
			data[indexX * bpp + indexY * _size * bpp + 2] = 0;
		}
		Texture hoge(data, _size, _size);
		std::string fileName = std::string("CheckBluePoint") + std::to_string(polygon) + std::string(" , ") + std::to_string(_progress) + std::string(".png");
		hoge.WriteImage(fileName.c_str());
	}

	//-------------
	//-------------
	//ApertureBoxBlue
	int ApertureBoxBlue::GetIndex(int index, int size) {
		if (index >= size) {
			index -= size;
		}
		return index;
	}

	vec3 ApertureBoxBlue::Square2triangle(vec3 _input) {
		vec3 cal = _input;
		//std::cout << "Input :" << std::string(_input) << std::endl;
		if (_input.y > _input.x) {
			cal.x *= 0.5f;
			cal.y -= cal.x;
		} else {
			cal.y *= 0.5f;
			cal.x -= cal.y;
		}
		return cal;
	}

	ApertureBoxBlue::ApertureBoxBlue()
		: blue(Texture("blue.png")) {
	}

	vec3 ApertureBoxBlue::Sample(Random& _rand) {
		double blue_x = _rand.next01();
		const unsigned int polygonIndex = (unsigned int)(blue_x);
		double x = blue_x - polygonIndex;
		double y = _rand.next01();
		double blue_y = y;
		const vec3 offseted = blue.Sample(blue_x, blue_y);

		vec3 vec_a = vec3(1, 1, 0);
		vec3 vec_b = vec3(1, 0, 0);

		vec3 triVec = Square2triangle(vec3(x, y, 0));

		return vec_a * triVec.x + vec_b * triVec.y;
	}

	void ApertureBoxBlue::CheckBluePoint(unsigned int _size, int _pointNum, float _progress) {
		Random _rand(0);
		constexpr unsigned int bpp = 3;
		std::vector<unsigned char> data(_size * _size * bpp, 255);
		for (int i = 0; i < _pointNum; ++i) {
			const double randX = _rand.next01() / 3;
			const double blue_x = randX * blue.GetTextureSize();
			const unsigned int polygonIndex = (unsigned int)(randX);
			const double randY = _rand.next01();
			const double blue_y = randY * blue.GetTextureSize();
			const vec3 offseted = blue.Sample(blue_x, blue_y);

			vec3 vec_a = vec3(1, 1, 0);
			vec3 vec_b = vec3(1, 0, 0);

			vec3 triVec = Square2triangle(vec3(blue_x - (int)(blue_x), offseted.y / blue.GetTextureSize(), 0));

			vec3 point = vec_a * triVec.x + vec_b * triVec.y;
			const int indexX = point.x * (_size - 1);
			const int indexY = point.y * (_size - 1);
			data[indexX * bpp + indexY * _size * bpp] = 0;
			data[indexX * bpp + indexY * _size * bpp + 1] = 0;
			data[indexX * bpp + indexY * _size * bpp + 2] = 0;
		}
		Texture hoge(data, _size, _size);
		hoge.WriteImage("CheckBluePoint.png");
	}

	//-------------
	//-------------
	//AperturePolygonSquare
	int AperturePolygonSquare::GetIndex(int index, int size) {
		if (index >= size) {
			index -= size;
		}
		return index;
	}

	AperturePolygonSquare::AperturePolygonSquare(unsigned int _polygon)
		: blue(Texture("blue.png"))
		, polygon(_polygon) {
		for (int i = 0; i < polygon; ++i) {
			double deg = 2.0 * std::numbers::pi * i / polygon;
			vec3 cal = vec3(std::cos(deg), std::sin(deg), 0);
			coner.push_back(cal);
		}
	}

	vec3 AperturePolygonSquare::Sample(Random& _rand) {
		double blue_x = _rand.next01();
		const unsigned int polygonIndex = (unsigned int)(blue_x * polygon);
		double x = blue_x * polygon - polygonIndex;
		double y = _rand.next01();
		double blue_y = y / polygonIndex;
		const vec3 offset = blue.Sample(blue_x, blue_y);

		vec3& vec_a = coner[GetIndex(polygonIndex, coner.size())];
		vec3& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

		//vec3 P = vec3(std::sqrt(s1) * (s2 * V2 + (1 - s2) * V3);

		//return vec_a * triVec.x + vec_b * triVec.y;
		return vec3();
	}

	//-------------
	//-------------
	//AperturePolygonBlueTest
	int AperturePolygonBlueTest::GetIndex(int index, int size) {
		if (index >= size) {
			index -= size;
		}
		return index;
	}

	vec3 AperturePolygonBlueTest::Square2triangle(vec3 _input) {
		vec3 cal = _input;
		if (_input.y > _input.x) {
			cal.x *= 0.5f;
			cal.y -= cal.x;
		} else {
			cal.y *= 0.5f;
			cal.x -= cal.y;
		}
		return cal;
	}

	AperturePolygonBlueTest::AperturePolygonBlueTest(unsigned int _polygon)
		: blue(Texture("blue.png"))
		//: blue(Texture("LDR_RG01_23.png"))
		, polygon(_polygon) {
		for (int i = 0; i < polygon; ++i) {
			double deg = 2.0 * std::numbers::pi * i / polygon;
			vec3 cal = vec3(std::cos(deg), std::sin(deg), 0);
			coner.push_back(cal);
		}
	}

	vec3 AperturePolygonBlueTest::Sample(Random& _rand) {
		double blue_x = _rand.next01();
		const unsigned int polygonIndex = (unsigned int)(blue_x * polygon);
		double x = blue_x * polygon - polygonIndex;
		double y = _rand.next01();
		double blue_y = y / polygon;
		const vec3 offseted = blue.Sample(blue_x, blue_y);

		vec3& vec_a = coner[GetIndex(polygonIndex, coner.size())];
		vec3& vec_b = coner[GetIndex(polygonIndex + 1, coner.size())];

		vec3 triVec = Square2triangle(vec3(x, y, 0));

		return vec_a * triVec.x + vec_b * triVec.y;
	}

	void AperturePolygonBlueTest::CheckBluePoint(int _texSize, int _pointNum) {
		Texture tmp_blue = Texture("blue.png");
		const unsigned int width = _texSize * tmp_blue.width;
		constexpr unsigned int bpp = 3;
		std::vector<unsigned char>data(bpp * width * width, 255);
		Random rand(0);
		for (int y = 0; y < width; ++y) {
			for (int x = 0; x < width; ++x) {
				vec3 hoge = blue.Sample((double)x / _texSize, (double)y / _texSize);
				const float blue_x01 = hoge.x - (int)(hoge.x);
				const float blue_y01 = hoge.y - (int)(hoge.y);
				const vec3 point = vec3(
					(double)x / _texSize / tmp_blue.width, (double)y / _texSize / tmp_blue.height, 0
				);
				const vec3 pointInCell = vec3(
					(double)x / _texSize - (int)((double)x / _texSize)
					, (double)y / _texSize - (int)((double)y / _texSize)
					, 0);
				const vec3 pointNextInCell = vec3(
					(double)(x + 1) / _texSize - (int)((double)(x + 1) / _texSize)
					, (double)(y + 1) / _texSize - (int)((double)(y + 1) / _texSize)
					, 0);
				if (pointInCell.x <= blue_x01 && blue_x01 < pointNextInCell.x) {
					if (pointInCell.y <= blue_y01 && blue_y01 < pointNextInCell.y) {
						data[x * bpp + y * width * bpp] = 0;
						data[x * bpp + y * width * bpp + 1] = 0;
						data[x * bpp + y * width * bpp + 2] = 0;
					}
				}
			}
		}
		Texture hoge(data, _texSize * blue.GetTextureSize(), _texSize * blue.GetTextureSize());
		hoge.WriteImage("CheckBluePoint.png");
	}
}