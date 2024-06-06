/*
 * A very contrived program showing how a functor can be used as a callable.
 */
#include <cmath>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

struct Droid {
    static Droid clone() { return Droid{}; }
};
struct DroidV2 {
    static Droid clones() { return Droid{}; }
};

template <typename C>
concept Clonable = requires(C clonable) {
    { clonable.clone() } -> std::same_as<C>;
    // requires std::same_as<C, decltype(clonable.clone())>;
};

class Canvas {
   public:
    Canvas() = default;

    Canvas(const std::size_t w, const std::size_t h) : width{w}, height{h} {
        data_points =
            std::vector<std::vector<int>>(height, std::vector<int>(width, 0));
    }

    // other functions
    // void resize (){}
    // void scale (){}
    void display() {
        std::cout << "*************Canvas ID: " << this << " ************\n";
        for (const auto& line : data_points) {
            for (const auto& point : line) {
                if (point == 0)
                    std::cout << " . ";
                else
                    std::cout << " * ";
            }
            std::cout << '\n';
        }
        std::cout << "*************Canvas ID: " << this << " ************"
                  << std::endl;
    }
    // 0 based index
    bool set_coord(const std::size_t x, const std::size_t y,
                   const int val) {  // returns true if set is successful
        if (!is_within_bounds(x, y)) return false;
        data_points[x][y] = val;
        return true;
    }

    std::optional<int> get_coord(const std::size_t x,
                                 const std::size_t y) const {
        // bounds check
        if (!is_within_bounds(x, y)) return std::nullopt;
        return data_points[y][x];
    }

    std::size_t get_width() const { return width; }
    std::size_t get_height() const { return height; }

   private:
    // Dimensions
    std::size_t width{16}, height{16};
    std::vector<std::vector<int>> data_points =
        std::vector<std::vector<int>>(height, std::vector<int>(width, 0));

    // bounds check
    bool is_within_bounds(const std::size_t x, const std::size_t y) const {
        if ((x > width - 1) | (y > height - 1) | x < 0 | y < 0) return false;
        return true;
    }
};

enum class Shape : std::uint8_t {
    SQUARE = 0x01,
    TRIANGLE = 0x02,
    CIRCLE = 0x03,
    TRAPEZIUM = 0x04,
    POLYGON = 0x05,
    RHOMBUS = 0x06,
    KITE = 0x07,
    LINE = 0x08,
    POINT = 0x09,
    CIRCLE_V2 = 0x10
};

template <typename C>
concept CanvasDrawer = requires(C canvasDrawer, Canvas cv, Shape shape) {
    canvasDrawer.setCanvas(&cv);
    { canvasDrawer.getCanvas() } -> std::same_as<std::shared_ptr<Canvas>>;
    { canvasDrawer.draw() } -> std::same_as<std::shared_ptr<Canvas>>;
    { canvasDrawer(shape) } -> std::same_as<std::shared_ptr<Canvas>>;
    //{ canvasDrawer.transferCanvas() } ->
    // std::same_as<std::shared_ptr<Canvas>>;  //can be set as a requirement
};

// class myDrawer;

class myDrawer {
   public:
    explicit myDrawer(std::shared_ptr<Canvas> cv) : sheet{std::move(cv)} {}
    std::shared_ptr<Canvas> draw() {
        sheet->display();
        std::cout << "displayed Canvas through Drawer\n\n";
        return sheet;
    }

    // overloaded call operator
    std::shared_ptr<Canvas> operator()(Shape sp) {
        std::cout << "Drawing on Canvas:\n";
        std::size_t canvas_width{sheet->get_width()},
            canvas_height{sheet->get_height()};
        // Type Checks
        switch (sp) {
            case Shape::SQUARE:
                // Draw a square on the extreme dimensions of canvas
                // Calculate the coordinates for all the extreme points
                // draw along the top and bottom widths
                for (std::size_t i{0}; i < canvas_width; i++) {
                    sheet->set_coord(0, i, 1);
                    sheet->set_coord(canvas_height - 1, i, 1);
                }
                // draw along the left and right height
                for (std::size_t i{0}; i < canvas_height; i++) {
                    sheet->set_coord(i, 0, 1);
                    sheet->set_coord(i, canvas_width - 1, 1);
                }
                break;
            // NOLINTBEGIN
            case Shape::TRIANGLE:
                // Centre a triangle within the canvas
                // Add your code here
                break;
            case Shape::CIRCLE:
                // Circle inscribed within a canvas
                {
                    // Center of the canvas
                    int mid_x = canvas_width / 2;
                    int mid_y = canvas_height / 2;
                    int radius = std::min(mid_x, mid_y);
                    int diameter = 2 * radius;

                    for (int i = 0; i <= diameter; ++i) {
                        for (int j = 0; j <= diameter; ++j) {
                            // Calculate the distance from the center
                            int distance = std::round(
                                std::sqrt((i - radius) * (i - radius) +
                                          (j - radius) * (j - radius)));

                            if (distance == radius - 1) {
                                sheet->set_coord(i, j, 1);
                            }
                        }
                    }
                    break;
                }
            case Shape::TRAPEZIUM:
                // Centre a trapezium within the canvas
                // Add your code here
                break;
            case Shape::POLYGON:
                // Centre a polygon within the canvas
                // Add your code here
                break;
            case Shape::RHOMBUS:
                // Centre a rhombus within the canvas
                // Add your code here
                break;
            case Shape::KITE:
                // Centre a kite within the canvas
                // Add your code here
                break;
            case Shape::LINE:
                // Centre a line within the canvas
                // Add your code here
                break;
            // NOLINTEND
            case Shape::POINT:
                // Centre a point within the canvas
                sheet->set_coord(canvas_width / 2, canvas_height / 2, 1);

            case Shape::CIRCLE_V2:
                // Alternate algo for a circle inscribed in a canvas
                {
                    // Center of the canvas
                    int mid_x = canvas_width / 2;
                    int mid_y = canvas_height / 2;

                    int x_cursor = std::min(mid_x, mid_y);
                    int y_cursor = 0;
                    int axis = 0;
                    while (x_cursor >= y_cursor) {
                        // programmatic approach
                        /* bool flip = false;
                        int sign = -1;
                        for (int i = 0; i < 4; i++) {
                            // flip sign every 2 iterations
                            if ((i) % 2 == 0) sign *= -1;
                            sheet->set_coord(mid_y + sign * y,
                                             mid_x + std::pow(-1, i) * x, 1);

                        }
                        for (int i = 0; i < 4; i++) {
                            if ((i) % 2 == 0) sign *= -1;
                            sheet->set_coord(mid_x + sign * x,
                                             mid_y + std::pow(-1, i) * y, 1);

                        } */
                        // set_coord is bounds checked
                        sheet->set_coord(mid_y + y_cursor, mid_x + x_cursor, 1);
                        sheet->set_coord(mid_y + y_cursor, mid_x - x_cursor, 1);
                        sheet->set_coord(mid_y - y_cursor, mid_x + x_cursor, 1);
                        sheet->set_coord(mid_y - y_cursor, mid_x - x_cursor, 1);
                        sheet->set_coord(mid_y + x_cursor, mid_x + y_cursor, 1);
                        sheet->set_coord(mid_y + x_cursor, mid_x - y_cursor, 1);
                        sheet->set_coord(mid_y - x_cursor, mid_x + y_cursor, 1);
                        sheet->set_coord(mid_y - x_cursor, mid_x - y_cursor, 1);

                        if (axis <= 0) {
                            y_cursor += 1;
                            axis += 2 * y_cursor + 1;
                        }

                        if (axis > 0) {
                            x_cursor -= 1;
                            axis -= 2 * x_cursor + 1;
                        }
                    }
                    break;
                }
        }
        std::cout << "Drew on Canvas\n";
        return draw();
    }

    // Getters and setters
    std::shared_ptr<Canvas> getCanvas() { return sheet; }

    // Required for the constraint to hold
    // 0 is error
    bool setCanvas(Canvas* cv) {
        if (cv != nullptr)
            sheet.reset(cv);
        else
            return false;
        return true;
    }

    std::shared_ptr<Canvas> transferCanvas() { return std::move(sheet); }

   private:
    // Data members
    std::shared_ptr<Canvas> sheet;
};

// A Higher order function accepting CanvasDrawer callable
// For Higher order functions it is always a good idea to provide a default
// callable in case it is not provided
void canvas_mask_painter(
    CanvasDrawer auto cdraw = myDrawer(std::make_shared<Canvas>()),
    Shape shape = Shape::SQUARE, int colour = 1) {
    // draw first
    cdraw(shape);

    // Scale to colour
    for (std::size_t i{0}; i < cdraw.getCanvas()->get_width(); i++) {
        for (std::size_t j{0}; j < cdraw.getCanvas()->get_height(); j++) {
            cdraw.getCanvas()->set_coord(
                i, j, cdraw.getCanvas()->get_coord(i, j).value_or(0) * colour);
        }
    }
}

int main() {
    // Testing clonable concept
    Clonable auto c = Droid{};
    // Clonable auto c2 = DroidV2{}; //{ clonable.clone() } -> std::same_as<C>;
    // not satisfied

    // Canvas constructor
    Canvas rectangular_canvas(8, 4);
    rectangular_canvas.display();

    // Canvas to draw on throughout demo
    std::cout << "New Canvas size 17 X 17\n";
    Canvas cv(17, 17);
    cv.display();

    // shared pointer with custom empty deleter (because object is on stack)
    std::shared_ptr<Canvas> canvasPtr(
        &cv, [](Canvas*) { std::cout << "Tried to delete\n\n"; });

    // Passed canvas to Drawer
    myDrawer draw_for_me(canvasPtr);
    // myDrawer draw_for_me(std::make_shared<Canvas>());  //temporary created
    // and moved

    // calling functor (function object), and pass Shape
    draw_for_me(Shape::SQUARE);

    // compare cv address to canvas in functor
    cv.display();
    draw_for_me.draw();  // The Same!!

    // Test Higher order function with CanvasDrawer concept for callable
    // Pass functor as a callable
    std::cout << "Call to Higher order function, passed Drawer callable\n";
    canvas_mask_painter(draw_for_me, Shape::POINT, 42);

    canvas_mask_painter(draw_for_me, Shape::CIRCLE_V2, 42);

    // canvasPtr = draw_for_me.transferCanvas();   //can be used to return
    // ownership

    // Display to test
    std::cout << "Call display on Canvas on the stack\n";
    cv.display();
    std::cout << "CV display was called!!!\n";
    std::cout << "The value of a point of a line on the canvas is: "
              << cv.get_coord(0, 0).value_or(0) << std::endl;
}
