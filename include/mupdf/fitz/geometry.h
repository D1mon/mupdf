#ifndef MUPDF_FITZ_MATH_H
#define MUPDF_FITZ_MATH_H

#include "mupdf/fitz/system.h"

/*
	Multiply scaled two integers in the 0..255 range
*/
static inline int fz_mul255(int a, int b)
{
	/* see Jim Blinn's book "Dirty Pixels" for how this works */
	int x = a * b + 128;
	x += x >> 8;
	return x >> 8;
}

/*
	Undo alpha premultiplication.
*/
static inline int fz_div255(int c, int a)
{
	return a ? c * (255 * 256 / a) >> 8 : 0;
}

/*
	Expand a value A from the 0...255 range to the 0..256 range
*/
#define FZ_EXPAND(A) ((A)+((A)>>7))

/*
	Combine values A (in any range) and B (in the 0..256 range),
	to give a single value in the same range as A was.
*/
#define FZ_COMBINE(A,B) (((A)*(B))>>8)

/*
	Combine values A and C (in the same (any) range) and B and D (in
	the 0..256 range), to give a single value in the same range as A
	and C were.
*/
#define FZ_COMBINE2(A,B,C,D) (((A) * (B) + (C) * (D))>>8)

/*
	Blend SRC and DST (in the same range) together according to
	AMOUNT (in the 0...256 range).
*/
#define FZ_BLEND(SRC, DST, AMOUNT) ((((SRC)-(DST))*(AMOUNT) + ((DST)<<8))>>8)

/*
	Range checking atof
*/
float fz_atof(const char *s);

/*
	atoi that copes with NULL
*/
int fz_atoi(const char *s);
int64_t fz_atoi64(const char *s);

/*
	Some standard math functions, done as static inlines for speed.
	People with compilers that do not adequately implement inlines may
	like to reimplement these using macros.
*/
static inline float fz_abs(float f)
{
	return (f < 0 ? -f : f);
}

static inline int fz_absi(int i)
{
	return (i < 0 ? -i : i);
}

static inline float fz_min(float a, float b)
{
	return (a < b ? a : b);
}

static inline int fz_mini(int a, int b)
{
	return (a < b ? a : b);
}

static inline size_t fz_minz(size_t a, size_t b)
{
	return (a < b ? a : b);
}

static inline float fz_max(float a, float b)
{
	return (a > b ? a : b);
}

static inline int fz_maxi(int a, int b)
{
	return (a > b ? a : b);
}

static inline int64_t fz_maxi64(int64_t a, int64_t b)
{
	return (a > b ? a : b);
}

static inline float fz_clamp(float f, float min, float max)
{
	return (f > min ? (f < max ? f : max) : min);
}

static inline int fz_clampi(int i, int min, int max)
{
	return (i > min ? (i < max ? i : max) : min);
}

static inline double fz_clampd(double d, double min, double max)
{
	return (d > min ? (d < max ? d : max) : min);
}

static inline void *fz_clampp(void *p, void *min, void *max)
{
	return (p > min ? (p < max ? p : max) : min);
}

#define DIV_BY_ZERO(a, b, min, max) (((a) < 0) ^ ((b) < 0) ? (min) : (max))

/*
	fz_point is a point in a two-dimensional space.
*/
typedef struct fz_point_s fz_point;
struct fz_point_s
{
	float x, y;
};

static inline fz_point fz_make_point(float x, float y)
{
	fz_point p = { x, y };
	return p;
}

/*
	fz_rect is a rectangle represented by two diagonally opposite
	corners at arbitrary coordinates.

	Rectangles are always axis-aligned with the X- and Y- axes.
	The relationship between the coordinates are that x0 <= x1 and
	y0 <= y1 in all cases except for infinite rectangles. The area
	of a rectangle is defined as (x1 - x0) * (y1 - y0). If either
	x0 > x1 or y0 > y1 is true for a given rectangle then it is
	defined to be infinite.

	To check for empty or infinite rectangles use fz_is_empty_rect
	and fz_is_infinite_rect.

	x0, y0: The top left corner.

	x1, y1: The bottom right corner.
*/
typedef struct fz_rect_s fz_rect;
struct fz_rect_s
{
	float x0, y0;
	float x1, y1;
};

static inline fz_rect fz_make_rect(float x0, float y0, float x1, float y1)
{
	fz_rect r = { x0, y0, x1, y1 };
	return r;
}

/*
	fz_irect is a rectangle using integers instead of floats.

	It's used in the draw device and for pixmap dimensions.
*/
typedef struct fz_irect_s fz_irect;
struct fz_irect_s
{
	int x0, y0;
	int x1, y1;
};

static inline fz_irect fz_make_irect(int x0, int y0, int x1, int y1)
{
	fz_irect r = { x0, y0, x1, y1 };
	return r;
}

/*
	A rectangle with sides of length one.

	The bottom left corner is at (0, 0) and the top right corner
	is at (1, 1).
*/
extern const fz_rect fz_unit_rect;

/*
	An empty rectangle with an area equal to zero.

	Both the top left and bottom right corner are at (0, 0).
*/
extern const fz_rect fz_empty_rect;
extern const fz_irect fz_empty_irect;

/*
	An infinite rectangle with negative area.

	The corner (x0, y0) is at (1, 1) while the corner (x1, y1) is
	at (-1, -1).
*/
extern const fz_rect fz_infinite_rect;
extern const fz_irect fz_infinite_irect;

/*
	fz_is_empty_rect: Check if rectangle is empty.

	An empty rectangle is defined as one whose area is zero.
*/
static inline int fz_is_empty_rect(fz_rect r)
{
	return (r.x0 == r.x1 || r.y0 == r.y1);
}

static inline int fz_is_empty_irect(fz_irect r)
{
	return (r.x0 == r.x1 || r.y0 == r.y1);
}

/*
	fz_is_infinite_rect: Check if rectangle is infinite.

	An infinite rectangle is defined as one where either of the
	two relationships between corner coordinates are not true.
*/
static inline int fz_is_infinite_rect(fz_rect r)
{
	return (r.x0 > r.x1 || r.y0 > r.y1);
}

/*
	fz_is_infinite_irect: Check if an integer rectangle
	is infinite.

	An infinite rectangle is defined as one where either of the
	two relationships between corner coordinates are not true.
*/
static inline int fz_is_infinite_irect(fz_irect r)
{
	return (r.x0 > r.x1 || r.y0 > r.y1);
}

/*
	fz_matrix is a row-major 3x3 matrix used for representing
	transformations of coordinates throughout MuPDF.

	Since all points reside in a two-dimensional space, one vector
	is always a constant unit vector; hence only some elements may
	vary in a matrix. Below is how the elements map between
	different representations.

	/ a b 0 \
	| c d 0 | normally represented as [ a b c d e f ].
	\ e f 1 /
*/
typedef struct fz_matrix_s fz_matrix;
struct fz_matrix_s
{
	float a, b, c, d, e, f;
};

/*
	fz_identity: Identity transform matrix.
*/
extern const fz_matrix fz_identity;

static inline fz_matrix fz_make_matrix(float a, float b, float c, float d, float e, float f)
{
	fz_matrix m = { a, b, c, d, e, f };
	return m;
}

static inline int fz_is_identity(fz_matrix m)
{
	return m.a == 1 && m.b == 0 && m.c == 0 && m.d == 1 && m.e == 0 && m.f == 0;
}

/*
	fz_concat: Multiply two matrices.

	The order of the two matrices are important since matrix
	multiplication is not commutative.

	Returns result.
*/
fz_matrix fz_concat(fz_matrix left, fz_matrix right);

/*
	fz_scale: Create a scaling matrix.

	The returned matrix is of the form [ sx 0 0 sy 0 0 ].

	m: Pointer to the matrix to populate

	sx, sy: Scaling factors along the X- and Y-axes. A scaling
	factor of 1.0 will not cause any scaling along the relevant
	axis.

	Returns m.
*/
fz_matrix fz_scale(float sx, float sy);

/*
	fz_pre_scale: Scale a matrix by premultiplication.

	m: Pointer to the matrix to scale

	sx, sy: Scaling factors along the X- and Y-axes. A scaling
	factor of 1.0 will not cause any scaling along the relevant
	axis.

	Returns m (updated).
*/
fz_matrix fz_pre_scale(fz_matrix m, float sx, float sy);

/*
	fz_post_scale: Scale a matrix by postmultiplication.

	m: Pointer to the matrix to scale

	sx, sy: Scaling factors along the X- and Y-axes. A scaling
	factor of 1.0 will not cause any scaling along the relevant
	axis.

	Returns m (updated).
*/
fz_matrix fz_post_scale(fz_matrix m, float sx, float sy);

/*
	fz_shear: Create a shearing matrix.

	The returned matrix is of the form [ 1 sy sx 1 0 0 ].

	m: pointer to place to store returned matrix

	sx, sy: Shearing factors. A shearing factor of 0.0 will not
	cause any shearing along the relevant axis.

	Returns m.
*/
fz_matrix fz_shear(float sx, float sy);

/*
	fz_pre_shear: Premultiply a matrix with a shearing matrix.

	The shearing matrix is of the form [ 1 sy sx 1 0 0 ].

	m: pointer to matrix to premultiply

	sx, sy: Shearing factors. A shearing factor of 0.0 will not
	cause any shearing along the relevant axis.

	Returns m (updated).
*/
fz_matrix fz_pre_shear(fz_matrix m, float sx, float sy);

/*
	fz_rotate: Create a rotation matrix.

	The returned matrix is of the form
	[ cos(deg) sin(deg) -sin(deg) cos(deg) 0 0 ].

	m: Pointer to place to store matrix

	degrees: Degrees of counter clockwise rotation. Values less
	than zero and greater than 360 are handled as expected.

	Returns m.
*/
fz_matrix fz_rotate(float degrees);

/*
	fz_pre_rotate: Rotate a transformation by premultiplying.

	The premultiplied matrix is of the form
	[ cos(deg) sin(deg) -sin(deg) cos(deg) 0 0 ].

	m: Pointer to matrix to premultiply.

	degrees: Degrees of counter clockwise rotation. Values less
	than zero and greater than 360 are handled as expected.

	Returns m (updated).
*/
fz_matrix fz_pre_rotate(fz_matrix m, float degrees);

/*
	fz_translate: Create a translation matrix.

	The returned matrix is of the form [ 1 0 0 1 tx ty ].

	m: A place to store the created matrix.

	tx, ty: Translation distances along the X- and Y-axes. A
	translation of 0 will not cause any translation along the
	relevant axis.

	Returns m.
*/
fz_matrix fz_translate(float tx, float ty);

/*
	fz_pre_translate: Translate a matrix by premultiplication.

	m: The matrix to translate

	tx, ty: Translation distances along the X- and Y-axes. A
	translation of 0 will not cause any translation along the
	relevant axis.

	Returns m.
*/
fz_matrix fz_pre_translate(fz_matrix m, float tx, float ty);

/*
	fz_transform_page: Create transform matrix to draw page
	at a given resolution and rotation. Adjusts the scaling
	factors so that the page covers whole number of
	pixels and adjust the page origin to be at 0,0.
*/
fz_matrix fz_transform_page(fz_rect mediabox, float resolution, float rotate);

/*
	fz_invert_matrix: Create an inverse matrix.

	inverse: Place to store inverse matrix.

	matrix: Matrix to invert. A degenerate matrix, where the
	determinant is equal to zero, can not be inverted and the
	original matrix is returned instead.

	Returns inverse.
*/
fz_matrix fz_invert_matrix(fz_matrix matrix);

/*
	fz_try_invert_matrix: Attempt to create an inverse matrix.

	inverse: Place to store inverse matrix.

	matrix: Matrix to invert. A degenerate matrix, where the
	determinant is equal to zero, can not be inverted.

	Returns 1 if matrix is degenerate (singular), or 0 otherwise.
*/
int fz_try_invert_matrix(fz_matrix *inv, fz_matrix src);

/*
	fz_is_rectilinear: Check if a transformation is rectilinear.

	Rectilinear means that no shearing is present and that any
	rotations present are a multiple of 90 degrees. Usually this
	is used to make sure that axis-aligned rectangles before the
	transformation are still axis-aligned rectangles afterwards.
*/
int fz_is_rectilinear(fz_matrix m);

/*
	fz_matrix_expansion: Calculate average scaling factor of matrix.
*/
float fz_matrix_expansion(fz_matrix m);

/*
	fz_intersect_rect: Compute intersection of two rectangles.

	Given two rectangles, update the first to be the smallest
	axis-aligned rectangle that covers the area covered by both
	given rectangles. If either rectangle is empty then the
	intersection is also empty. If either rectangle is infinite
	then the intersection is simply the non-infinite rectangle.
	Should both rectangles be infinite, then the intersection is
	also infinite.
*/
fz_rect fz_intersect_rect(fz_rect a, fz_rect b);

/*
	fz_intersect_irect: Compute intersection of two bounding boxes.

	Similar to fz_intersect_rect but operates on two bounding
	boxes instead of two rectangles.
*/
fz_irect fz_intersect_irect(fz_irect a, fz_irect b);

/*
	fz_union_rect: Compute union of two rectangles.

	Given two rectangles, update the first to be the smallest
	axis-aligned rectangle that encompasses both given rectangles.
	If either rectangle is infinite then the union is also infinite.
	If either rectangle is empty then the union is simply the
	non-empty rectangle. Should both rectangles be empty, then the
	union is also empty.
*/
fz_rect fz_union_rect(fz_rect a, fz_rect b);

/*
	fz_irect_from_rect: Convert a rect into the minimal bounding box
	that covers the rectangle.

	bbox: Place to store the returned bbox.

	rect: The rectangle to convert to a bbox.

	Coordinates in a bounding box are integers, so rounding of the
	rects coordinates takes place. The top left corner is rounded
	upwards and left while the bottom right corner is rounded
	downwards and to the right.

	Returns bbox (updated).
*/

fz_irect fz_irect_from_rect(fz_rect rect);

/*
	fz_round_rect: Round rectangle coordinates.

	Coordinates in a bounding box are integers, so rounding of the
	rects coordinates takes place. The top left corner is rounded
	upwards and left while the bottom right corner is rounded
	downwards and to the right.

	This differs from fz_irect_from_rect, in that fz_irect_from_rect
	slavishly follows the numbers (i.e any slight over/under calculations
	can cause whole extra pixels to be added). fz_round_rect
	allows for a small amount of rounding error when calculating
	the bbox.
*/
fz_irect fz_round_rect(fz_rect rect);

/*
	fz_rect_from_irect: Convert a bbox into a rect.

	For our purposes, a rect can represent all the values we meet in
	a bbox, so nothing can go wrong.

	rect: A place to store the generated rectangle.

	bbox: The bbox to convert.

	Returns rect (updated).
*/
fz_rect fz_rect_from_irect(fz_irect bbox);

/*
	fz_expand_rect: Expand a bbox by a given amount in all directions.
*/
fz_rect fz_expand_rect(fz_rect b, float expand);
fz_irect fz_expand_irect(fz_irect a, int expand);

/*
	fz_include_point_in_rect: Expand a bbox to include a given point.
	To create a rectangle that encompasses a sequence of points, the
	rectangle must first be set to be the empty rectangle at one of
	the points before including the others.
*/
fz_rect fz_include_point_in_rect(fz_rect r, fz_point p);

/*
	fz_translate_irect: Translate bounding box.

	Translate a bbox by a given x and y offset. Allows for overflow.
*/
fz_rect fz_translate_rect(fz_rect a, float xoff, float yoff);
fz_irect fz_translate_irect(fz_irect a, int xoff, int yoff);

/*
	fz_contains_rect: Test rectangle inclusion.

	Return true if a entirely contains b.
*/
int fz_contains_rect(fz_rect a, fz_rect b);

/*
	fz_transform_point: Apply a transformation to a point.

	transform: Transformation matrix to apply. See fz_concat,
	fz_scale, fz_rotate and fz_translate for how to create a
	matrix.

	point: Pointer to point to update.

	Returns transform (unchanged).
*/
fz_point fz_transform_point(fz_point point, fz_matrix m);
fz_point fz_transform_point_xy(float x, float y, fz_matrix m);

/*
	fz_transform_vector: Apply a transformation to a vector.

	transform: Transformation matrix to apply. See fz_concat,
	fz_scale and fz_rotate for how to create a matrix. Any
	translation will be ignored.

	vector: Pointer to vector to update.
*/
fz_point fz_transform_vector(fz_point vector, fz_matrix m);

/*
	fz_transform_rect: Apply a transform to a rectangle.

	After the four corner points of the axis-aligned rectangle
	have been transformed it may not longer be axis-aligned. So a
	new axis-aligned rectangle is created covering at least the
	area of the transformed rectangle.

	transform: Transformation matrix to apply. See fz_concat,
	fz_scale and fz_rotate for how to create a matrix.

	rect: Rectangle to be transformed. The two special cases
	fz_empty_rect and fz_infinite_rect, may be used but are
	returned unchanged as expected.
*/
fz_rect fz_transform_rect(fz_rect rect, fz_matrix m);

/*
	fz_normalize_vector: Normalize a vector to length one.
*/
fz_point fz_normalize_vector(fz_point p);

fz_matrix fz_gridfit_matrix(int as_tiled, fz_matrix m);

float fz_matrix_max_expansion(fz_matrix m);

typedef struct fz_quad_s fz_quad;
struct fz_quad_s
{
	fz_point ul, ur, ll, lr;
};

fz_rect fz_rect_from_quad(fz_quad q);
fz_quad fz_transform_quad(fz_quad q, fz_matrix m);

int fz_is_point_inside_rect(fz_point p, fz_rect r);
int fz_is_point_inside_irect(int x, int y, fz_irect r);

#endif
