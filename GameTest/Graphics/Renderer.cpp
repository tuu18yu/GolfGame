#include "stdafx.h"

#include "Renderer.h"

#include <algorithm>
#include <cmath>
#include "../App/app.h"
#include "../Math/Vec2.h"

void Renderer::Draw(Camera camera, const Mesh &mesh, bool wireFrameOn) const
{
	std::vector<Triangle> rasterTriangles;


	for (auto& triangle : mesh.triangles)
	{
		Triangle triProjected;
		triProjected = triangle;

		// Local Space -> World Space: Apply rotation, translation, scale

		// Convention is to apply rotation in order of roll -> yaw -> pitch
		// As different order results different matrix
		if (mesh.fRoll != 0.0f)
		{
			Mat4 rotateZ;
			rotateZ.ZRotate(mesh.fRoll);
			triProjected *= rotateZ;
			triProjected.transformNormal(rotateZ);
		}

		if (mesh.fYaw != 0.0f)
		{
			Mat4 rotateY;
			rotateY.YRotate(mesh.fYaw);
			triProjected *= rotateY;
			triProjected.transformNormal(rotateY);
		}

		if (mesh.fPitch != 0.0f)
		{
			Mat4 rotateX;
			rotateX.XRotate(mesh.fPitch);
			triProjected *= rotateX;
			triProjected.transformNormal(rotateX);
		}


		// Scale and Translate
		Mat4 matScale, matMove;
		// Assume always uniform transformation, then normal is not affected
		matScale.Scale(mesh.scale);
		triProjected *= matScale;
		// Normal is not affected
		matMove.Translate(mesh.pos);
		triProjected *= matMove;

		//Get Normal
		// Vec3 normal = normalExist ? triProjected.normals[0] : triProjected.GetNormal();
		Vec3 normal = triProjected.GetNormal();
		
		Vec3 target = camera.forward;
		// Backface culling
		if (normal.dot_product((target - camera.pos)) < 0.0f)
		{
			// View Transformation: World Space -> View Space
			Mat4 matView;
			matView.PointAt(camera.pos, target, { 0.0f, 1.0f, 0.0f });
			triProjected *= matView;

			//Projection Transformation: View Space -> Clip Space
			Mat4 projection;
			projection.Projection(m_fovAngle, float(WINDOW_WIDTH), float(WINDOW_HEIGHT), m_near, m_far); // w = 1024, h = 768
			triProjected *= projection;

			// Check if triangle is within the NDC
			bool isClipped = false;


			for (auto& p : triProjected.points)
			{
				// clipped on bottom and top
				if (p.x > 1.0f || p.x < -1.0f)
				{
					isClipped = true;
					break;
				}

				// clipped on left and right
				if (p.y > 1.0f || p.y < -1.0f)
				{
					isClipped = true;
					break;
				}

				// clipped on near and far
				if (p.z > 1.0f)
				{
					isClipped = true;
					break;
				}
			}


			if (!isClipped)
			{
				// Viewport Transformation: NDC -> Screen Coordinate
				// Scale and trasform the points between -1 to 1 to fit actual screen
				Mat4 viewport;
				viewport.Viewport(0.0f, 0.0f, float(WINDOW_WIDTH), float(WINDOW_HEIGHT), m_near, m_far); // w = 1024, h = 768
				triProjected *= viewport;

				// Color triangles
				Vec3 lightColor(1.f, 1.f, 1.f);

				// multiply ambient strength of 0.1;
				float ambientStrength = 0.1f;
				Vec3 ambient = lightColor * ambientStrength;

				// Diffuse lighting
				Vec3 lightDir = camera.pos - target;
				lightDir = lightDir.normalize();

				float diff = (((normal.dot_product(lightDir)) > (0.0f)) ? (normal.dot_product(lightDir)) : (0.0f));
				Vec3 diffuse = lightColor * diff;
				triProjected.fillColor = mesh.color * (diffuse + ambient);
				triProjected.lineColor = mesh.color * (diffuse + ambient);

				rasterTriangles.push_back(triProjected);
			}

		}

	}

	// Sort triangle in order of z, z is ordered based on average z value
	// This allows triangles to be drawn in order
	std::sort(rasterTriangles.begin(), rasterTriangles.end(), [](const Triangle& t1, const Triangle& t2)
		{
			float z1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.f;
			float z2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.f;
			return z1 > z2;
		});

	// Draw ordered triangles
	for (auto& triangle : rasterTriangles)
	{
		if (wireFrameOn) { triangle.DrawOutline(); }
		else { Fill(triangle); }
	}
}

void Renderer::Draw(Camera camera, const CBoundingBox& box) const
{
	// Make 8 points for cube
	Vec3 points[8] = { {box.min_corner().x, box.min_corner().y, box.min_corner().z},
			{box.max_corner().x, box.min_corner().y, box.min_corner().z},
			{box.min_corner().x, box.max_corner().y, box.min_corner().z},
			{box.max_corner().x, box.max_corner().y, box.min_corner().z},
			{box.min_corner().x, box.min_corner().y, box.max_corner().z},
			{box.max_corner().x, box.min_corner().y, box.max_corner().z},
			{box.min_corner().x, box.max_corner().y, box.max_corner().z},
			{box.max_corner().x, box.max_corner().y, box.max_corner().z} };

	std::vector<Square> faces = {
		// Bottom face (y = min)
		Square(points[0], points[1], points[3], points[2]),
		// Top face (y = max)
		Square(points[4], points[5], points[7], points[6]),
		// Front face (z = min)
		Square(points[0], points[1], points[5], points[4]),
		// Back face (z = max)
		Square(points[2], points[3], points[7], points[6]),
		// Left face (x = min)
		Square(points[0], points[2], points[6], points[4]),
		// Right face (x = max)
		Square(points[1], points[3], points[7], points[5])
	};

	std::vector<Square> transformedSquare;

	for (auto& square : faces)
	{
		Square squProjected;
		squProjected = square;

		//Get Normal
		// Vec3 normal = normalExist ? triProjected.normals[0] : triProjected.GetNormal();
		Vec3 normal = squProjected.GetNormal();
		Vec3 target = camera.forward;

		// Backface culling
		//if (normal.dot_product((target - camera.pos)) < 0.0f)
		{

			// View Transformation: World Space -> View Space
			Mat4 matView;
			matView.PointAt(camera.pos, target, { 0.0f, 1.0f, 0.0f });
			squProjected *= matView;

			//Projection Transformation: View Space -> Clip Space
			Mat4 projection;
			projection.Projection(m_fovAngle, float(WINDOW_WIDTH), float(WINDOW_HEIGHT), m_near, m_far); // w = 1024, h = 768
			squProjected *= projection;

			// Check if triangle is within the NDC
			bool isClipped = false;


			for (auto& p : squProjected.points)
			{
				// clipped on bottom
				if (p.x > 1.0f || p.x < -1.0f)
				{
					isClipped = true;
					break;
				}


				if (p.y > 1.0f || p.y < -1.0f)
				{
					isClipped = true;
					break;
				}

				if (p.z > 1.0f)
				{
					isClipped = true;
					break;
				}
			}


			if (!isClipped)
			{
				// Viewport Transformation: NDC -> Screen Coordinate
				// Scale and trasform the points between -1 to 1 to fit actual screen
				Mat4 viewport;
				viewport.Viewport(0.0f, 0.0f, float(WINDOW_WIDTH), float(WINDOW_HEIGHT), m_near, m_far); // w = 1024, h = 768
				squProjected *= viewport;

				squProjected.DrawOutline();
			}
		}

	}
}


void Renderer::Draw(Camera camera, const AABB &box) const
{
	// Make 8 points for cube
	Vec3 points[8] = { {box.min_corner.x, box.min_corner.y, box.min_corner.z},
			{box.max_corner.x, box.min_corner.y, box.min_corner.z},
			{box.min_corner.x, box.max_corner.y, box.min_corner.z},
			{box.max_corner.x, box.max_corner.y, box.min_corner.z},
			{box.min_corner.x, box.min_corner.y, box.max_corner.z},
			{box.max_corner.x, box.min_corner.y, box.max_corner.z},
			{box.min_corner.x, box.max_corner.y, box.max_corner.z},
			{box.max_corner.x, box.max_corner.y, box.max_corner.z} };

	std::vector<Square> faces = {
		// Bottom face (y = min)
		Square(points[0], points[1], points[3], points[2]),
		// Top face (y = max)
		Square(points[4], points[5], points[7], points[6]),
		// Front face (z = min)
		Square(points[0], points[1], points[5], points[4]),
		// Back face (z = max)
		Square(points[2], points[3], points[7], points[6]),
		// Left face (x = min)
		Square(points[0], points[2], points[6], points[4]),
		// Right face (x = max)
		Square(points[1], points[3], points[7], points[5])
	};

	std::vector<Square> transformedSquare;

	for (auto& square : faces)
	{
		Square squProjected;
		squProjected = square;

		//Get Normal
		// Vec3 normal = normalExist ? triProjected.normals[0] : triProjected.GetNormal();
		Vec3 normal = squProjected.GetNormal();
		Vec3 target = camera.forward;

		// Backface culling
		//if (normal.dot_product((target - camera.pos)) < 0.0f)
		{

			// View Transformation: World Space -> View Space
			Mat4 matView;
			matView.PointAt(camera.pos, target, { 0.0f, 1.0f, 0.0f });
			squProjected *= matView;

			//Projection Transformation: View Space -> Clip Space
			Mat4 projection;
			projection.Projection(m_fovAngle, float(WINDOW_WIDTH), float(WINDOW_HEIGHT), m_near, m_far); // w = 1024, h = 768
			squProjected *= projection;

			// Check if triangle is within the NDC
			bool isClipped = false;


			for (auto& p : squProjected.points)
			{
				// clipped on bottom
				if (p.x > 1.0f || p.x < -1.0f)
				{
					isClipped = true;
					break;
				}


				if (p.y > 1.0f || p.y < -1.0f)
				{
					isClipped = true;
					break;
				}

				if (p.z > 1.0f)
				{
					isClipped = true;
					break;
				}
			}


			if (!isClipped)
			{
				// Viewport Transformation: NDC -> Screen Coordinate
				// Scale and trasform the points between -1 to 1 to fit actual screen
				Mat4 viewport;
				viewport.Viewport(0.0f, 0.0f, float(WINDOW_WIDTH), float(WINDOW_HEIGHT), m_near, m_far); // w = 1024, h = 768
				squProjected *= viewport;

				squProjected.DrawOutline();
			}
		}

	}
}

void Renderer::Draw(Camera camera, const Ray& ray, float t) const
{
	Vec3 ray_pos1 = ray.origin;
	Vec3 ray_pos2 = ray.origin + (ray.direction * t);

	// View Transformation: World Space -> View Space
	Mat4 matView;
	matView.PointAt(camera.pos, camera.forward, { 0.0f, 1.0f, 0.0f });
	ray_pos1 = matView * ray_pos1;
	ray_pos2 = matView * ray_pos2;

	//Projection Transformation: View Space -> Clip Space
	Mat4 projection;
	projection.Projection(m_fovAngle, float(WINDOW_WIDTH), float(WINDOW_HEIGHT), m_near, m_far); // w = 1024, h = 768
	ray_pos1 = projection * ray_pos1;
	ray_pos2 = projection * ray_pos2;

	// Check if triangle is within the NDC
	bool isClipped = false;
	

	if (!isClipped)
	{
		// Viewport Transformation: NDC -> Screen Coordinate
		// Scale and trasform the points between -1 to 1 to fit actual screen
		Mat4 viewport;
		viewport.Viewport(0.0f, 0.0f, float(WINDOW_WIDTH), float(WINDOW_HEIGHT), m_near, m_far); // w = 1024, h = 768
		ray_pos1 = viewport * ray_pos1;
		ray_pos2 = viewport * ray_pos2;

		App::DrawLine(ray_pos1.x, ray_pos1.y, ray_pos2.x, ray_pos2.y, 1.0f, 0.984f, 0.0f);
	}

}

void Renderer::Draw(Camera camera, const ParticleSystem& particleSystem) const
{
	std::vector<Triangle> transformedParticle;

	for (auto& p : particleSystem.vertexArray)
	{
		Triangle particleProjected;
		particleProjected = p;

		//Get Normal
		// Vec3 normal = normalExist ? triProjected.normals[0] : triProjected.GetNormal();
		Vec3 normal = particleProjected.GetNormal();
		Vec3 target = camera.forward;

		// Backface culling
		//if (normal.dot_product((target - camera.pos)) < 0.0f)
		{

			// View Transformation: World Space -> View Space
			Mat4 matView;
			matView.PointAt(camera.pos, target, { 0.0f, 1.0f, 0.0f });
			particleProjected *= matView;

			//Projection Transformation: View Space -> Clip Space
			Mat4 projection;
			projection.Projection(m_fovAngle, float(WINDOW_WIDTH), float(WINDOW_HEIGHT), m_near, m_far); // w = 1024, h = 768
			particleProjected *= projection;

			// Check if triangle is within the NDC
			bool isClipped = false;


			for (auto& p : particleProjected.points)
			{
				// clipped on bottom
				if (p.x > 1.0f || p.x < -1.0f)
				{
					isClipped = true;
					break;
				}


				if (p.y > 1.0f || p.y < -1.0f)
				{
					isClipped = true;
					break;
				}

				if (p.z > 1.0f)
				{
					isClipped = true;
					break;
				}
			}


			if (!isClipped)
			{
				// Viewport Transformation: NDC -> Screen Coordinate
				// Scale and trasform the points between -1 to 1 to fit actual screen
				Mat4 viewport;
				viewport.Viewport(0.0f, 0.0f, float(WINDOW_WIDTH), float(WINDOW_HEIGHT), m_near, m_far); // w = 1024, h = 768
				particleProjected *= viewport;

				particleProjected.DrawOutline();
				//Fill(particleProjected);
			}
		}

	}
}

// Using Pineda’s Rasterization Algorithm to use fill in triangle
// Unlike Scanline algorithm, rasterization can be done parallel
void Renderer::Fill(const Triangle& triangle) const
{
	float r = triangle.fillColor.x, g = triangle.fillColor.y, b = triangle.fillColor.z;

	// points of triangle
	Vec3 points[3];
	points[0] = triangle.points[0];
	points[1] = triangle.points[1];
	points[2] = triangle.points[2];

	// bounding box for the triangle, all the points in the box are candidates for triangle fill
	int x_min = static_cast<int>(floor(min(min(points[0].x, points[1].x), points[2].x)));
	int x_max = static_cast<int>(floor(max(max(points[0].x, points[1].x), points[2].x)));
	int y_min = static_cast<int>(floor(min(min(points[0].y, points[1].y), points[2].y)));
	int y_max = static_cast<int>(floor(max(max(points[0].y, points[1].y), points[2].y)));



	auto PointInTriangle = [](const Triangle& t, Vec2& pixel) -> bool
		{

			for (int i = 0; i < 3; i++)
			{
				int j = (i != 2) ? i + 1 : 0;
				Vec2 edge(t.points[i].x - t.points[j].x, t.points[i].y - t.points[j].y);
				Vec2 lineToPixel(t.points[i].x - pixel.x, t.points[i].y - pixel.y);

				if (!t.isCW && edge.cross_product(lineToPixel) > 0.0f)
				{
					return false;
				}

				else if (t.isCW && edge.cross_product(lineToPixel) < 0.0f)
				{
					return false;
				}
			}
			return true;
		};


	// need to determine whether each point is in the triangle or not
	for (int y = y_min; y <= y_max; y++)
	{
		for (int x = x_min; x <= x_max; x++)
		{
			Vec2 point(static_cast<float>(x), static_cast<float>(y));
			// point is in triangle if all 3 cross product is positive
			if (PointInTriangle(triangle, point))
			{
				App::DrawLine(point.x, point.y, point.x + 1.0f, point.y + 1.0f, r, g, b);
			}
		}
	}
}


// Using Scanline Filling Algorithm to use fill in square
void Renderer::Fill(const Square& square) const
{
	float r = square.fillColor.x, g = square.fillColor.y, b = square.fillColor.z;

	std::vector<Vec2> points;
	points.push_back({ square.points[0].x, square.points[0].y, });
	points.push_back({ square.points[1].x, square.points[1].y, });
	points.push_back({ square.points[2].x, square.points[2].y, });
	points.push_back({ square.points[3].x, square.points[3].y, });

	std::sort(points.begin(), points.end(), [](const Vec2& a, const Vec2& b) {
		return a.y < b.y; // Sort by y-coordinate
		});

	float topY = points[0].y;
	float bottomY = points[3].y;

	Vec2 left = points[0].x < points[1].x ? points[0] : points[1];
	Vec2 right = points[0].x >= points[1].x ? points[0] : points[1];
	Vec2 leftBottom = points[2].x < points[3].x ? points[2] : points[3];
	Vec2 rightBottom = points[2].x >= points[3].x ? points[2] : points[3];

	// Use scanline to draw horizontal lines from left to right
	for (float y = topY; y <= bottomY; ++y)
	{
		// Interpolate x-coordinates for the current scanline
		float xLeft = left.x + (y - left.y) * (leftBottom.x - left.x) / (leftBottom.y - left.y);
		float xRight = right.x + (y - right.y) * (rightBottom.x - right.x) / (rightBottom.y - right.y);

		// Ensure xLeft is less than xRight
		if (xLeft > xRight)
			std::swap(xLeft, xRight);

		// Draw the horizontal line
		App::DrawLine(xLeft, y, xRight, y, r, g, b);
	}
}

// Using Scanline Filling Algorithm to use fill in triangle
//void Renderer::Fill() const
//{
//	float r = fillColor.x, g = fillColor.y, b = fillColor.z;
//
//	App::DrawLine(points[0].x, points[0].y, points[1].x, points[1].y, r, g, b);
//	App::DrawLine(points[1].x, points[1].y, points[2].x, points[2].y, r, g, b);
//	App::DrawLine(points[2].x, points[2].y, points[0].x, points[0].y, r, g, b);
//
//	Vec2 orderedPoints[3] = {{points[0].x, points[0].y}, {points[1].x, points[1].y}, {points[2].x, points[2].y}};
//	
//	auto swap_elements = [&orderedPoints](int pos1, int pos2)
//		{
//			Vec2 temp = orderedPoints[pos1];
//			orderedPoints[pos1] = orderedPoints[pos2];
//			orderedPoints[pos2] = temp;
//		};
//
//	// Sort the points so that y_0 <= y_1 <= y_2
//	if (orderedPoints[1].y < orderedPoints[0].y) { swap_elements(1, 0); }
//	if (orderedPoints[2].y < orderedPoints[0].y) { swap_elements(2, 0); }
//	if (orderedPoints[2].y < orderedPoints[1].y) { swap_elements(2, 1); }
//
//	// Compute the interpolated points of the edges
//	std::vector<Vec2> side1 = orderedPoints[0].interpolateX(orderedPoints[2]);
//	std::vector<Vec2> side2 = orderedPoints[0].interpolateX(orderedPoints[1]);
//	std::vector<Vec2> side2_more = orderedPoints[1].interpolateX(orderedPoints[2]);
//
//	//// Fill in the middle at orderedPoint 1, with slope from side 1 
//	//float yi = orderedPoints[1].y;
//	//float xi = orderedPoints[1].x + ((yi - orderedPoints[0].y) * (orderedPoints[2].x - orderedPoints[0].x) / (orderedPoints[2].y - orderedPoints[0].y));
//
//	//App::DrawLine(xi, yi, orderedPoints[1].x, orderedPoints[1].y, r, g, b);
//
//	//// Pop the overllap
//	//if (side2.size() != 0 && side2_more.size() != 0)
//	//{
//	//	side2.pop_back();
//	//}
//
//	side2.insert(side2.end(), side2_more.begin(), side2_more.end());
//
//	size_t N = side2.size();
//
//	// need to determine whether each point is in the triangle or not
//	for (int i = 0; i < N; i++)
//	{
//		App::DrawLine(side1[i].x, side1[i].y, side2[i].x, side2[i].y, r, g, b);
//	}
//
//}