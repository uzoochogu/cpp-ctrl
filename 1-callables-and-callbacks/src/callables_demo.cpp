#include <algorithm>
#include <complex>
#include <concepts>
#include <functional>
#include <iostream>
#include <type_traits>

// Concept ensuring Func(int) exists
template <typename Func>
    requires std::invocable<Func&, int>
int higher_order_func(Func fn, int val) {
    // Do some complex operations
    return fn(val) * 20;
}

// Short hand analog of the above
int higher_order_func2(std::invocable<int> auto fn, int val) { return fn(val); }

// Using std::invoke and std::same_as
template <typename Func>
    requires requires(Func& fn, int i) {
        { std::invoke(fn, i) } -> std::same_as<int>;
    }
int higher_order_func3(Func fn, int val) {
    // Do some complex operations
    return fn(val) * 20;
}

// Named concept defining integral and floating point numbers
template <typename T>
concept Number =
    (std::integral<T> || std::floating_point<T>)&&!std::same_as<T, bool> &&
    !std::same_as<T, char> && !std::same_as<T, unsigned char> &&
    !std::same_as<T, char8_t> && !std::same_as<T, char16_t> &&
    !std::same_as<T, char32_t> && !std::same_as<T, wchar_t>;

// concept expressed succintly through syntax and semantic constraints
// This defines callables that creates a complex number using 2 numbers
template <typename Func, typename T>
concept ComplexCreateFunc = Number<T> && requires(Func& fn, T d, T d2) {
    { fn(d, d2) } -> std::same_as<std::complex<T>>;
};

// Concept defining Unary operators on a complex number,
template <typename Func, typename T>
concept ComplexNumberUnaryOp =
    Number<T> && requires(Func& fn, std::complex<T> inumber) {
        { fn(inumber) } -> std::same_as<std::complex<T>>;
        // or {std::invoke(fn, inumber)} -> std::same_as<std::complex<T>>;
        //  to allow any valid callable.
    };

// templated functions
template <typename T>
std::complex<T> complex_calculator(ComplexCreateFunc<T> auto& fn, T arg1,
                                   T arg2) {
    return fn(arg1, arg2);
}

template <typename T>
std::complex<T> complex_calculator(ComplexNumberUnaryOp<T> auto& fn,
                                   std::complex<T> inumber) {
    return fn(inumber);
}

// templated ComplexCreate callback
template <typename T>
std::complex<T> complex_reactance(const T inductive_react,
                                  const T capacitive_react) {
    return std::complex<T>{0, inductive_react - capacitive_react};
}

// testing member function callback
class Vector {
   private:
    int i, j, k;

   public:
    Vector(int i_val, int j_val, int k_val) : i{i_val}, j{j_val}, k{k_val} {}
    // getter
    int get_i() const { return i; }
    int get_j() const { return j; }
    int get_k() const { return k; }
};

int component_getter(Vector val, int (Vector::*getter)() const) {
    return (val.*getter)();
}

// Used in bad cast section
int valhalla(int val, int halla = 10) {
    val += halla;
    return val;
}

// Used to demonstrate std::bind

double triangle_area(double b, double h) { return 0.5 * b * h; }
double trapezium_area(double a, double b, double h) {
    return 0.5 * h * (a + b);
}
double rectangle_area(double l, double b) { return l * b; }

double uniform_prism(double length, double breadth, double depth,
                     const std::function<double(double, double)>& shape_area) {
    return depth * shape_area(length, breadth);
}

// Demonstrating Variadic std::function callbacks
// Polynomials
double quadratic(double x, double a, double b, double c) {
    return a * std::pow(x, 2) + b * std::pow(x, 1) + c * std::pow(x, 0);
}
double mag_vector(double i, double j, double k) {
    return std::sqrt(std::pow(i, 2) + std::pow(j, 2) + std::pow(k, 2));
};

// scale_args
template <typename... T>
double scale_args(double scale, std::function<double(T...)> fn, T... args) {
    // decltype(fn) sd = [](double a, double b, double c) {return a+b+c;};
    // std::cout << sd(4,5,4);
    std::cout << "\n" << typeid(fn).name() << " ";

    // just to display it
    std::vector argvs{scale * args...};  // modify through a fold expression
    std::cout << "\nThe modified arguments: ";
    for (auto i : argvs) {
        std::cout << i << ", ";
    }

    std::cout << "\nFn result: ";
    return fn(scale * args...);
}

template <typename... T>
double plot(std::function<double(double, T...)> fn, double x, T... args) {
    return fn(x, args...);
}

int main() {
    std::cout << "**************************************************\n"
              << "Complex Number callables specified using concepts:\n";
    std::complex<double> cmplx{4.0, 3.9};

    std::cout << cmplx << '\n';

    // Below code will not compile because, char is not a "Number".
    //(Doesn't satisfy its constraint)
    // std::cout << complex_calculator(std::polar<char>, 'c', 'c');

    std::cout << "Creating a Complex number using std::polar:\n";
    // callables need to be wrapped for it to work on MSVC
    auto polar_wrapper = [](auto r, auto theta) -> std::complex<decltype(r)> {
        return std::polar(r, theta);
    };

    std::cout << complex_calculator(polar_wrapper, 3.4, 3.5) << '\n';

    std::cout << "Calling with custom ComplexCreate callback:\n";

    std::cout << complex_calculator(complex_reactance<double>, 3.4, 5.0)
              << '\n';

    std::cout << "Unary operation on a Complex number:\n";
    std::cout << complex_calculator(std::exp<double>, cmplx);

    std::cout << "\n**************************************************\n";

    // Demonstrating Member function pointers
    std::cout << "\n*******************************************************\n"
              << "Member function pointers:\n";
    Vector vect(1, 2, 4), vect2(3, 4, 5);
    int (Vector::*iget)() const = &Vector::get_i;
    int (Vector::*jget)() const = &Vector::get_j;
    std::cout << "Calling getters of a objects through function pointers:\n";
    std::cout << "i component of Vector vect:" << component_getter(vect, iget)
              << "\n";  // equivalent to vect.iget()
    std::cout << "j component of Vector vect2:" << component_getter(vect2, jget)
              << "\n";
    std::cout << "*******************************************************\n";

    std::cout << "\n*******************************\n"
              << "Uniform Prism:";
    // NOLINTBEGIN
    using namespace std::placeholders;
    // Function was made with Rectangle in mind, but we want to use Trapezium
    // and triangle prisms
    std::cout << "\nVol of Cubiod: "
              << uniform_prism(
                     2.2, 4.3, 6.5,
                     std::bind(rectangle_area, _1, _2));  // Order matches
    std::cout << "\nVol of Triangle prism: "
              << uniform_prism(
                     2.2, 4.3, 6.5,
                     std::bind(triangle_area, _2, _1));  // swap dimensions
    std::cout << "\nVol of Trapeziodal prism: "
              << uniform_prism(2.2, 4.3, 6.5,
                               std::bind(trapezium_area, _1, 2.6,
                                         _2));  // adapt dimensions
    std::cout << "\n*******************************\n";

    // Demonstrating function cast
    std::cout << "\n********************\n"
              << "Bad Function cast:\n";

    using f_int_t = int (*)(int);
    using f_ints_t_refs = int (*)(int&, int&);

    f_int_t bad_cast = (f_int_t)&valhalla;               // dangerous cast
    f_ints_t_refs evil_cast = (f_ints_t_refs)&valhalla;  // dangerous cast

    int hel{10};
    std::cout << "bad cast:" << bad_cast(hel);  // some random jibberish
    std::cout << "\nevil cast:" << evil_cast(hel, hel);  // some random
                                                         // jibberish

    std::function<int(int)> good_cast = std::bind(
        valhalla, std::placeholders::_1, 10);         // pass the default here
    std::cout << "\n\ngood cast:" << good_cast(hel);  // 20
    std::function<int(int&, int&)> correct_cast =
        std::bind(valhalla, std::placeholders::_1, std::placeholders::_2);
    std::cout << "\nhel before:" << hel;                       // 10
    std::cout << "\ncorrect cast:" << correct_cast(hel, hel);  // 20
    std::cout << "\nhel after:" << hel;                        // 10
    std::cout << "\n********************\n";

    // Using std::bind
    std::cout << "\n*******************************\n"
              << "std::bind demo:\n";

    std::vector<double> input_vec = {1, 2, 3, 4, 5};
    std::transform(input_vec.begin(), input_vec.end(), input_vec.begin(),
                   std::bind((double (*)(double, int))std::pow, _1, 7));
    // NOLINTEND [modernize-avoid-bind], clang-tidy
    for (const auto& i : input_vec) {
        std::cout << i << ", ";
    }
    std::cout << "\nSame result with a lambda:\n";
    // Same result with a lambda
    input_vec = {1, 2, 3, 4, 5};
    std::transform(input_vec.begin(), input_vec.end(), input_vec.begin(),
                   [](double val) -> double { return std::pow(val, 7); });
    for (const auto& i : input_vec) {
        std::cout << i << ", ";
    }

    // Generic Lambda  that can be used as a predicate
    auto compare_less = [](auto a, auto b) {
        return a < static_cast<decltype(a)>(b);
    };
    std::cout << "\nCompare less than: 4 < 7.0 =  " << compare_less(4, 7.0);
    std::cout << "\n*******************************\n";

    // Variadic std::function template used for scaling
    std::cout << "\n***************************************\n"
              << "Variadic std::function:\n";

    std::cout << "\nQuadratic scale args: "
              << scale_args(5.0, std::function(quadratic), 4.0, 3.0, 4.0, 6.0);
    std::cout << "\nMagnitude of Vector scale args: "
              << scale_args(5.0, std::function(mag_vector), 3.0, 4.0, 6.0);

    // Another Variadic Templated std::function callback
    std::function sd = [](double a, double b, double c) { return a + b + c; };
    std::cout << "\nPlot: " << plot(sd, 2.0, 3.0, 4.0);

    // stateful lambdas
    int value = 10;
    std::function<double(double)> add2value = [&](double num) {
        return num + value + 2;
    };
    std::cout << scale_args(5.0, add2value, 4.0);
    std::cout << "\n***************************************\n";

    return 0;
}
