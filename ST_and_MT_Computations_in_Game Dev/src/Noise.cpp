#include "Noise.h"

/// <summary>
/// Noise constructor.
/// </summary>
Noise::Noise()
{
	initialise();
}

/// <summary>
/// Noise destructor.
/// </summary>
Noise::~Noise()
{
	
}

/// <summary>
/// Initialise.
/// </summary>
void Noise::initialise()
{
	for (int i = 0; i < 512; i++)
	{
		perm[i] = p[i & 255]; 
	}
}

/// <summary>
/// Floor function (faster than the standard floor function, apparently).
/// </summary>
/// <param name="val">The value to be floored.</param>
/// <returns>The result of the floor function.</returns>
int Noise::fastFloor(double val)
{
	return val > 0 ? (int)val : (int)val - 1;
}

/// <summary>
/// Dot product (2D vector).
/// </summary>
/// <param name="g">Gradient table.</param>
/// <param name="x">X value.</param>
/// <param name="y">Y value.</param>
/// <returns>The result of the dot product.</returns>
double Noise::dot(int g[], double x, double y)
{
	return g[0] * x + g[1] * y;
}

/// <summary>
/// Dot product (3D vector).
/// </summary>
/// <param name="g">Gradient table.</param>
/// <param name="x">X value.</param>
/// <param name="y">Y value.</param>
/// <param name="z">Z value.</param>
/// <returns>The result of the dot product.</returns>
double Noise::dot(int g[], double x, double y, double z)
{
	return g[0] * x + g[1] * y + g[2] * z;
}

/// <summary>
/// Dot product (4D vector).
/// </summary>
/// <param name="g">Gradient table.</param>
/// <param name="x">X value.</param>
/// <param name="y">Y value.</param>
/// <param name="z">Z value.</param>
/// <param name="w">W value.</param>
/// <returns>The result of the dot product.</returns>
double Noise::dot(int g[], double x, double y, double z, double w)
{
	return g[0] * x + g[1] * y + g[2] * z + g[3] * w;
}

/// <summary>
/// Noise generation function. Returns a value between 0 and 1.
/// </summary>
/// <param name="xin">The range's lowest value.</param>
/// <param name="yin">The range's highest value.</param>
/// <returns>The resulting noise value.</returns>
double Noise::generate(double xin, double yin)
{
	// Noise contributions from the three corners
	double n0;
	double n1;
	double n2;

	// Skew the input space to determine which simplex cell we're in
	const double F2 = 0.5 * (std::sqrt(3.0) - 1.0);

	// Hairy factor for 2D
	double s = (xin + yin) * F2;
	int i = fastFloor(xin + s);
	int j = fastFloor(yin + s);
	const double G2 = (3.0 - sqrt(3.0)) / 6.0;
	double t = ((double)i + j) * G2;

	// Unskew the cell origin back to (x, y) space
	double X0 = i - t;
	double Y0 = j - t;

	// The (x, y) distances from the cell origin
	double x0 = xin - X0;
	double y0 = yin - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle
	// The code below determines which simplex we are in

	// Offsets for second (middle) corner of simplex in (i, j) coords
	int i1;
	int j1;
	
	if (x0 > y0) // Lower triangle: (x, y) order: (0, 0)->(1, 0)->(1, 1)
	{
		i1 = 1;
		j1 = 0;
	}	
	else // Upper triangle: (y, x) order: (0, 0)->(0, 1)->(1, 1)
	{
		i1 = 0;
		j1 = 1;
	}

	// A step of (1, 0) in (i, j) means a step of (1 - c, -c) in (x, y)
	// A step of (0, 1) in (i, j) means a step of (-c, 1 - c) in (x, y) where c = (3 - sqrt(3)) / 6

	// Offsets for middle corner in (x, y) unskewed coords
	double x1 = x0 - i1 + G2;
	double y1 = y0 - j1 + G2;

	// Offsets for last corner in (x, y) unskewed coords
	double x2 = x0 - 1.0 + 2.0 * G2;
	double y2 = y0 - 1.0 + 2.0 * G2;

	// Work out the hashed gradient indices of the three simplex corners
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = perm[ii + perm[jj]] % 12;
	int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
	int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

	// Calculate the contribution from the three corners
	double t0 = 0.5 - x0 * x0 - y0 * y0;

	if (t0 < 0)
	{
		n0 = 0.0;
	}
	else 
	{
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0); // (x, y) of grad3 used for 2D gradient
	}

	double t1 = 0.5 - x1 * x1 - y1 * y1;

	if (t1 < 0)
	{
		n1 = 0.0;
	}
	else
	{
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
	}

	double t2 = 0.5 - x2 * x2 - y2 * y2;

	if (t2 < 0)
	{
		n2 = 0.0;
	}
	else 
	{
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
	}

	// Add contributions from each corner to get the final noise value
	// The result will be between [-1, 1]
	float v = 70.0f * (n0 + n1 + n2);

	// We want values between [0, 1]
	return normaliseToRange(v, -1.0f, 1.0f);
}

/// <summary>
/// Normalise value to a specified range.
/// </summary>
/// <param name="val">The value to normalise.</param>
/// <param name="min">The minimum number in the range.</param>
/// <param name="max">The maximum number in the range.</param>
/// <returns>The normalised value.</returns>
float Noise::normaliseToRange(float val, float min, float max)
{
	return (val - min) / (max - min);
}
