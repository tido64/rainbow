// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_LYRICALLABEL_H_
#define GRAPHICS_LYRICALLABEL_H_

#include <vector>

#include "Graphics/Label.h"

class Timer;

class LyricalLabel : private Label
{
public:
	enum class Animation
	{
		Shake,
		Typing,
		Count
	};

	struct Attribute
	{
		enum class Type
		{
			Color,
			Offset,
			Shake
		} type;
		unsigned int start;
		unsigned int length;

		union
		{
			unsigned char color[4];
			unsigned int magnitude;
			int offset[2];
		};

		Attribute(const Colorb color,
		          const unsigned int start,
		          const unsigned int len);

		Attribute(const unsigned int magnitude,
		          const unsigned int start,
		          const unsigned int len);

		Attribute(const Vec2i &offset,
		          const unsigned int start,
		          const unsigned int len);
	};

	LyricalLabel();
	~LyricalLabel() { clear_animations(); }

	Label* as_label() { return static_cast<Label*>(this); }

	/// Clears all animations.
	void clear_animations();

	/// Clears all attributes.
	void clear_attributes();

	/// Clears all attributes of given type.
	void clear_attributes(const Attribute::Type type);

	/// Sets color on characters in given range.
	/// \param color   The color to set.
	/// \param start   Offset to the first character in range.
	/// \param length  Number of characters in range.
	void set_color(const Colorb color,
	               const unsigned int start,
	               const unsigned int length);

	/// Offsets characters' positions in given range.
	/// \param offset  The vector to offset by.
	/// \param start   Offset to the first character in range.
	/// \param length  Number of characters in range.
	void set_offset(const Vec2i &offset,
	                const unsigned int start,
	                const unsigned int length);

	/// Shake characters in given range.
	/// \param magnitude  The magnitude of the shaking.
	/// \param start      Offset to the first character in range.
	/// \param length     Number of characters in range.
	void set_shaking(const unsigned int magnitude,
	                 const unsigned int start,
	                 const unsigned int length);

	/// Sets text to display.
	void set_text(const char *);

	/// Starts an animation.
	/// \param animation  The animation to start.
	/// \param interval   Interval between each frame.
	void start_animation(const Animation animation, const int interval);

	/// Stops an animation.
	/// \param animation  The animation to stop.
	void stop_animation(const Animation animation);

	void update() override;

	using Label::color;
	using Label::count;
	using Label::font;
	using Label::position;
	using Label::text;
	using Label::vertex_array;
	using Label::width;
	using Label::set_alignment;
	using Label::set_color;
	using Label::set_font;
	using Label::set_needs_update;
	using Label::set_position;
	using Label::set_rotation;
	using Label::set_scale;
	using Label::bind_textures;
	using Label::move;

private:
	std::vector<Attribute> attributes_;
	unsigned int applied_;
	bool did_shake_;
	Timer *animators_[static_cast<int>(Animation::Count)];

	Vec2u get_interval(const Attribute &attr);
	void undo_from(std::vector<Attribute>::const_iterator first);
};

#endif
