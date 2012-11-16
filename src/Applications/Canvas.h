#ifndef RAINBOW_CANVAS_H_
#define RAINBOW_CANVAS_H_

#ifdef USE_CANVAS
#include "Common/SpriteVertex.h"
#include "Graphics/Drawable.h"
#include "Input/Touch.h"
#include "Input/Touchable.h"

struct Texture;

/// Canvas is a painting application.
///
/// Currently setup to "erase" the foreground, giving the illusion of revealing
/// the background. However, its function is entirely determined by the
/// fragment shader.
///
/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Canvas : public Drawable, public NonCopyable<Canvas>, public Touchable
{
public:
	Canvas();
	virtual ~Canvas();

	/// Clear the canvas.
	void clear();

	/// Get percentage of the canvas being filled.
	float get_filled();

	/// Set background colour.
	/// \param color  RGBA value.
	void set_background(const unsigned int color);

	/// Set background texture. Note that the texture will be stretched to fill
	/// the canvas, maintaining ratio.
	/// \param texture       Texture to set as background.
	/// \param width,height  Dimension of texture.
	void set_background(const Texture &texture, const int width, const int height);

	inline void set_brush(const Texture &brush);

	/// Set brush diameter size.
	inline void set_brush_size(const unsigned int size);

	/// Set foreground colour.
	/// \param color  RGBA value.
	void set_foreground(const unsigned int colour);

	/// Returns \c true if the canvas was successfully created, otherwise \c false.
	inline operator bool() const;

	// Implement Drawable

	virtual void draw() override;
	virtual void update() override;

	// Implement Touchable

	virtual void touch_began(const Touch *const touches, const size_t count) override;
	virtual void touch_canceled() override;
	virtual void touch_ended(const Touch *const touches, const size_t count) override;
	virtual void touch_moved(const Touch *const touches, const size_t count) override;

private:
	static int canvas_program;

	bool changed;             ///< Whether an update is needed.
	bool down;                ///< Whether the brush is down.
	Colorb foreground_color;  ///< Brush colour.
	float fill;               ///< Percentage of the canvas being filled.
	unsigned int brush_size;  ///< Size of the brush in diameter.
	int width, height;        ///< Width and height of canvas.

	unsigned int background_tex;  ///< Background texture.
	unsigned int canvas_buffer;   ///< Canvas buffer.
	unsigned int canvas_fb;       ///< Canvas frame buffer.
	unsigned int canvas_tex;      ///< Canvas texture.
	unsigned int canvas_vao;      ///< Canvas vertex array object.

	const Texture *brush;    ///< Brush texture.

	Vec2f prev_point;         ///< Previous touch point.
	SpriteVertex sprite[4];   ///< Canvas sprite.
	Touch touch;              ///< Current touch point.

	/// Create a sprite from a point.
	void create_point(SpriteVertex *vertex, const int x, const int y);

	/// Release all resources.
	void release();
};

void Canvas::set_brush(const Texture &brush)
{
	this->brush = &brush;
}

void Canvas::set_brush_size(const unsigned int size)
{
	this->brush_size = size;
}

Canvas::operator bool() const
{
	return this->canvas_fb;
}

#endif  // USE_CANVAS
#endif
