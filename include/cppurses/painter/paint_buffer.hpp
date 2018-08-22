#ifndef PAINTER_DETAIL_PAINT_BUFFER_HPP
#define PAINTER_DETAIL_PAINT_BUFFER_HPP
#include <cstddef>
#include <mutex>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/ncurses_paint_engine.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/painter/rgb.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
struct Glyph;
class Painter;

// TODO Remove this class and place functionality elsewhere.
class Paint_buffer {
   public:
    Paint_buffer();

    /// Apply staged_diff_ to the physical screen via engine_.
    // void flush(bool optimize);
    void flush(const detail::Staged_changes& changes);

    // TODO Does not need to be public?
    void move_cursor(std::size_t x, std::size_t y);

    // TODO don't need this interface, put this in screen_width directly
    std::size_t update_width();
    std::size_t update_height();

    std::size_t screen_width() const;
    std::size_t screen_height() const;

    // void set_repaint_all();

    void set_color_definition(Color c, RGB values);

    void set_global_background_tile(const Glyph& tile);
    Glyph get_global_background_tile() const;

   private:
    // TODO Move to system or some shared object with limited access to flush
    // and whoever holds set_color_definition()
    detail::NCurses_paint_engine engine_;

    // bool repaint_all_{false};  // TODO remove?

    // TODO where to put the global background tile?
    Glyph global_background_tile_{L' '};
    std::size_t width_{0};  // TODO put these within paint engine instead
    std::size_t height_{0};

    // TODO probably dont' need these, especially if you no longer have this
    // class. Though look into if they are useful, because if they are you can't
    // just get rid of them.
    using Mutex_t = std::recursive_mutex;
    mutable Mutex_t mutex_;

    // TODO These would no longer be needed, paint_engine just retrieves the
    // size, you don't tell it its size.
    // void resize_width(std::size_t new_width);
    // void resize_height(std::size_t new_height);

    // TODO these are used by flush, i think, keep them around. For some reason
    // you kept flush within a class, might have been the friend declaration on
    // Widget::screen_state? or something like that. or for the mutex in
    // ncurses_data.
    // void cover_all_with_background();
    // void cover_with_background(Widget& w);
    // bool within_screen(const Point& p);

    /// Paints every point of \p w with wallpaper or from \p changes.
    void full_paint(Widget* w, const detail::Screen_descriptor& changes);

    /// Performs a full paint of a single tile at \p p.
    void full_paint_point(Widget* w,
                          const detail::Screen_descriptor& changes,
                          const Point& p);

    /// Used when a child has just been enabled after not being on the screen.
    void paint_just_enabled(Widget* w,
                            const detail::Screen_descriptor& changes);

    /// Repaints the empty space of the layout, then a basic paint of \p w.
    void paint_child_event(Widget* w, const detail::Screen_descriptor& changes);

    ///
    void paint_resize_event(Widget* w,
                            const detail::Screen_descriptor& changes);

    ///
    void paint_move_event(Widget* w, const detail::Screen_descriptor& changes);

    /// Covers points in w->screen_state that are not found in \p changes.
    void cover_leftovers(Widget* w, const detail::Screen_descriptor& changes);

    /// Paints tiles from \p changes, if needed, covers leftovers w/ wallpaper.
    void basic_paint(Widget* w, const detail::Screen_descriptor& changes);

    /// Performs a basic paint of a single point at \p p.
    void basic_paint_point(Widget* w, const Point& p, Glyph tile);

    /// Paints wallpaper on layouts where there is no child widget.
    void paint_empty_tiles(Widget* w);
};

}  // namespace cppurses
#endif  // PAINTER_DETAIL_PAINT_BUFFER_HPP
