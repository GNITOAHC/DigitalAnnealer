#include "tri.h"
#include "../../include/Helper.h"
#include <complex>

namespace tri {
std::vector<double> getSquaredOP (const std::vector<Spin>& spins, const int length) {
    // const int height = graph.spins.size() / graph.length;
    // const int length = graph.length;
    const int height = spins.size() / length;
    // std::cout << "Height: " << height << std::endl;
    // std::cout << "Length: " << length << std::endl;
    const std::complex<double> image_pi(0.0, (4.0 / 3.0) * M_PI);
    const std::complex<double> math_e(M_E, 0.0);

    // Create a vector of vectors to store the color parameters and count of each color
    // m_color_params[layer][color] = count of blue, black, red
    std::vector<std::vector<double> > m_color_params(height, std::vector<double>(3, 0));

    auto get_sub_lattice = [&] (const int& index) { return ((index / length) + index) % 3; };

    const int each_color_count_per_layer_int = (length * length) / 3;
    const double each_color_count_per_layer  = (double)each_color_count_per_layer_int;
    const int total_count                    = spins.size();

    for (int i = 0; i < total_count; ++i) {
        const int layer = i / length;
        // std::cout << "Layer: " << layer << std::endl;
        switch (get_sub_lattice(i)) {
            case 0: m_color_params[layer][0] += (double)spins[i]; break;
            case 1: m_color_params[layer][1] += (double)spins[i]; break;
            case 2: m_color_params[layer][2] += (double)spins[i]; break;
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

    const std::complex<double> math_e_pow     = std::pow(math_e, image_pi);
    const std::complex<double> math_e_pow_inv = std::pow(math_e, -image_pi);

    std::vector<double> layer_squared_op(height, 0.0);

    // Iterate over each layer
    for (int i = 0; i < height; ++i) {
        const std::complex<double> m_blue(m_color_params[i][0] / each_color_count_per_layer, 0.0);
        const std::complex<double> m_black(m_color_params[i][1] / each_color_count_per_layer, 0.0);
        const std::complex<double> m_red(m_color_params[i][2] / each_color_count_per_layer, 0.0);

        // Calculate the order parameter
        const std::complex<double> order_parameter =
            (m_blue + m_black * math_e_pow + m_red * math_e_pow_inv) / sqrt(3.0);

        // Calculate the order parameter length squared
        const double squared_op =
            std::pow(order_parameter.real(), 2.0) + std::pow(order_parameter.imag(), 2.0);

        layer_squared_op[i] = squared_op;
    }

    // for (int i = 0; i < layer_order_parameter_length_squared.size(); ++i) {
    //     std::cout << "Layer " << i << ": " << layer_order_parameter_length_squared[i] <<
    //     std::endl;
    // }
    return layer_squared_op;
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
    int (*macro_array[])(const int&, const int&, const int&, const int&) = { GETRIGHT, GETBOTTOM,
                                                                             GETBOTTOMRIGHT };
    // for (int h = 0; h < height; ++h) {
    for (int i = 0; i < length; ++i) {
        for (int j = 0; j < length; ++j) {
            const int index = i * length + j;
            for (int c = 0; c < 3; ++c)
                graph.pushBack(index, (*macro_array[c])(0, i, j, length), 1.0);
            // if (height > 1) graph.pushBack(index, GETLAYERUP(h, i, j, length, height),
            // strength_between_layer);
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

void printTriConf (const std::vector<Spin>& spins, const int& length, std::ofstream& cout) {
    const int height = spins.size() / length;
    // Calculate params
    std::vector<std::vector<double> > m_color_params(height, std::vector<double>(3, 0));
    auto get_sub_lattice = [&] (const int& index) -> int { return ((index / length) + index) % 3; };
    // const std::vector<Spin> s = graph.getSpins();
    for (int i = 0; i < spins.size(); ++i) {
        const int layer = i / length;
        // std::cout << "Layer: " << layer << std::endl;
        switch (get_sub_lattice(i)) {
            case 0: m_color_params[layer][0] += (double)spins[i]; break;
            case 1: m_color_params[layer][1] += (double)spins[i]; break;
            case 2: m_color_params[layer][2] += (double)spins[i]; break;
            default: throw std::exception();
        }
    }

    /*
     * TODO: (m1 \t m2 \t m3)
     * tsc format: (Squared Order Parameter)
     * layer \t squared order parameter \t m1 \t m2 \t m3
     */
    cout << "layer\tsquared_op\tm1\tm2\tm3\n";
    const std::vector<double> squared_ops = getSquaredOP(spins, length);
    for (int i = 0; i < squared_ops.size(); ++i) {
        cout << i << "\t" << squared_ops[i] << "\t";
        /*
         * Print the color parameters here for each layer (m1, m2. m3)
         */
        cout << m_color_params[i][0] << "\t" << m_color_params[i][1] << "\t"
             << m_color_params[i][2];
        cout << std::endl;
    }
    return;
}

} // namespace tri

/*
 * Original make graph for length, height, gamma
 */
// Graph makeGraph (const int& length, const int& height, const double& gamma) {
//     Graph graph;
//     const int length2 = length * length;
//     const double strength_between_layer = gamma == 0.0 ? 0.0 : (-0.5) * loge(tanh(gamma));
//     int (*macro_array[])(const int&, const int&, const int&, const int&) = { GETRIGHT, GETBOTTOM,
//     GETBOTTOMRIGHT }; for (int h = 0; h < height; ++h) {
//         for (int i = 0; i < length; ++i) {
//             for (int j = 0; j < length; ++j) {
//                 const int index = h * length2 + i * length + j;
//                 for (int c = 0; c < 3; ++c)
//                     graph.pushBack(index, (*macro_array[c])(h, i, j, length), 1.0);
//                 if (height > 1) graph.pushBack(index, GETLAYERUP(h, i, j, length, height),
//                 strength_between_layer);
//             }
//         }
//     }
//
//     return graph;
// }
