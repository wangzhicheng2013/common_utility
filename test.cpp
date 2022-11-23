#include <iostream>
#include "image_utility.hpp"
int main() {
	std::cout << G_IMAGE_UTILITY.convert_uyvy_nv21("./dump0001_564x564.uyvy", 564, 564, "./dump0001_564x564.NV21") << std::endl;
    return 0;
}