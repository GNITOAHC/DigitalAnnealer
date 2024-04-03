#include "../../include/Helper.h"
#include "tri.h"
#include <complex>

namespace tri {
std::vector<double> getSquaredOP (const Graph& graph) {
    const int height = graph.spins.size() / graph.length;
    const int length = graph.length;
    // std::cout << "Height: " << height << std::endl;
    // std::cout << "Length: " << length << std::endl;
    const std::complex<double> image_pi(0.0, (4.0 / 3.0) * M_PI);
    const std::complex<double> math_e(M_E, 0.0);

    // Create a vector of vectors to store the color parameters and count of each color
    // m_color_params[layer][color] = count of blue, black, red
    std::vector<std::vector<double> > m_color_params(height, std::vector<double>(3, 0));

    auto get_sub_lattice = [&] (const int& index) { return ((index / length) + index) % 3; };

    const int each_color_count_per_layer_int = (length * length) / 3;
    const double each_color_count_per_layer = (double)each_color_count_per_layer_int;
    const int total_count = graph.spins.size();

    for (int i = 0; i < total_count; ++i) {
        const int layer = i / length;
        // std::cout << "Layer: " << layer << std::endl;
        switch (get_sub_lattice(i)) {
            case 0: m_color_params[layer][0] += (double)graph.spins[i]; break;
            case 1: m_color_params[layer][1] += (double)graph.spins[i]; break;
            case 2: m_color_params[layer][2] += (double)graph.spins[i]; break;
            default: throw std::exception();
        }
    }

    // Print out the m_color_params
    // for (int i = 0; i < height; ++i) {
    //     std::cout << "Layer " << i << ": ";
    //     for (int j = 0; j < 3; ++j) {
    //         std::cout << m_color_params[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }

    const std::complex<double> math_e_pow = std::pow(math_e, image_pi);
    const std::complex<double> math_e_pow_inv = std::pow(math_e, -image_pi);

    std::vector<double> layer_order_parameter_length_squared(height, 0.0);

    // Iterate over each layer
    for (int i = 0; i < height; ++i) {
        const std::complex<double> m_blue(m_color_params[i][0] / each_color_count_per_layer, 0.0);
        const std::complex<double> m_black(m_color_params[i][1] / each_color_count_per_layer, 0.0);
        const std::complex<double> m_red(m_color_params[i][2] / each_color_count_per_layer, 0.0);

        // Calculate the order parameter
        const std::complex<double> order_parameter = (m_blue + m_black * math_e_pow + m_red * math_e_pow_inv) / sqrt(3.0);

        // Calculate the order parameter length squared
        const double order_parameter_length_squared = std::pow(order_parameter.real(), 2.0) + std::pow(order_parameter.imag(), 2.0);

        layer_order_parameter_length_squared[i] = order_parameter_length_squared;
    }

    // for (int i = 0; i < layer_order_parameter_length_squared.size(); ++i) {
    //     std::cout << "Layer " << i << ": " << layer_order_parameter_length_squared[i] << std::endl;
    // }
    return layer_order_parameter_length_squared;
    // return std::vector<double>();
}

Graph makeGraph (const int& length) {
    // const double E = std::exp(1.0);
    // auto loge = [&] (double x) -> double { return std::log(x) / std::log(E); };

    Graph graph;
    // const int height = 1;
    // const int length2 = length * length;
    // const double gamma = 0.2;
    // const double strength_between_layer = gamma == 0.0 ? 0.0 : (-0.5) * loge(tanh(gamma));
    int (*macro_array[])(const int&, const int&, const int&, const int&) = { GETRIGHT, GETBOTTOM, GETBOTTOMRIGHT };
    // for (int h = 0; h < height; ++h) {
    for (int i = 0; i < length; ++i) {
        for (int j = 0; j < length; ++j) {
            const int index = i * length + j;
            for (int c = 0; c < 3; ++c)
                graph.pushBack(index, (*macro_array[c])(0, i, j, length), 1.0);
            // if (height > 1) graph.pushBack(index, GETLAYERUP(h, i, j, length, height), strength_between_layer);
        }
    }
    // }

    return graph;
    /*
     * Debug section
     */
    // std::ofstream outfile;
    // std::string filename = "output_a.txt";
    // outfile.open(filename, std::ios::out);
    // graph.print(outfile);
    // outfile.close();
    // return graph;
}
} // namespace tri

/*
 * Original make graph for length, height, gamma
 */
// Graph makeGraph (const int& length, const int& height, const double& gamma) {
//     Graph graph;
//     const int length2 = length * length;
//     const double strength_between_layer = gamma == 0.0 ? 0.0 : (-0.5) * loge(tanh(gamma));
//     int (*macro_array[])(const int&, const int&, const int&, const int&) = { GETRIGHT, GETBOTTOM, GETBOTTOMRIGHT };
//     for (int h = 0; h < height; ++h) {
//         for (int i = 0; i < length; ++i) {
//             for (int j = 0; j < length; ++j) {
//                 const int index = h * length2 + i * length + j;
//                 for (int c = 0; c < 3; ++c)
//                     graph.pushBack(index, (*macro_array[c])(h, i, j, length), 1.0);
//                 if (height > 1) graph.pushBack(index, GETLAYERUP(h, i, j, length, height), strength_between_layer);
//             }
//         }
//     }
//
//     return graph;
// }
