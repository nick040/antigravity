#include <iostream>
#include <vector>
#include <string>
#include <sstream>

// A simple C++98 class representing a Point
class Point {
private:
    int x;
    int y;

public:
    Point(int xVal, int yVal) : x(xVal), y(yVal) {}

    int getX() const { return x; }
    int getY() const { return y; }

    std::string toString() const {
        std::stringstream ss;
        ss << "(" << x << ", " << y << ")";
        return ss.str();
    }
};

int main() {
    std::cout << "===========================================\n";
    std::cout << "  C++ MinGW / w64devkit Project Setup      \n";
    std::cout << "===========================================\n\n";

    // 1. Check C++ Standard
    std::cout << "C++ Standard version (__cplusplus): " << __cplusplus << "\n";
    if (__cplusplus == 199711L) {
        std::cout << "-> Enforcing C++98 standard successfully!\n\n";
    } else {
        std::cout << "-> Running on standard version: " << __cplusplus << "\n\n";
    }

    // 2. Check compiler details
#if defined(__GNUC__)
    std::cout << "Compiler: GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << "\n\n";
#elif defined(_MSC_VER)
    std::cout << "Compiler: MSVC (Visual Studio) " << _MSC_VER << "\n\n";
#else
    std::cout << "Compiler: Unknown compiler\n\n";
#endif

    // 3. C++98 container and iterator demonstration
    std::cout << "Demonstrating C++98 standard library features:\n";
    std::vector<Point> points;
    points.push_back(Point(10, 20));
    points.push_back(Point(30, 45));
    points.push_back(Point(-5, 12));

    std::cout << "Points list:\n";
    for (std::vector<Point>::const_iterator it = points.begin(); it != points.end(); ++it) {
        std::cout << " - " << it->toString() << "\n";
    }
    std::cout << "\n";

    std::cout << "Project set up successfully!\n";
    return 0;
}
