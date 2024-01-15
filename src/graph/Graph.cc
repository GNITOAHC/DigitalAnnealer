#include <complex>

#include "../Helper.h"
#include "Graph.h"

#define debug(n) std::cerr << n << std::endl;

/* Private functions */

void Graph::privatePushBack(const int& index, AdjNode *node) {
    /* Insert into adj_list */
    if (index >= adj_list.size()) { adj_list.resize(index + 1, nullptr); }
    if (adj_list[index] == nullptr) {
        adj_list[index] = node;
    } else {
        AdjNode *tmp = adj_list[index];
        while (tmp->next != nullptr) {
            tmp = tmp->next;
        }
        tmp->next = node;
    }
    /* Insert into adj_map */
    std::map<int, std::vector<int> >::iterator it = adj_map.find(index);
    if (it == adj_map.end()) {
        adj_map[index] = std::vector<int>({ node->val });
    } else {
        it->second.push_back(node->val);
    }
    /* Append the Spin vector */
    if (index >= spins.size()) { spins.resize(index + 1, UP); }
}

void Graph::privatePushBack(const int& index, const double& constant) {
    /* Insert into constant_map */
    std::map<int, double>::iterator it = constant_map.find(index);
    if (it == constant_map.end()) {
        constant_map.insert(std::pair<int, double>(index, constant));
    } else {
        it->second += constant;
    }
    /* Append the Spin vector */
    if (index >= spins.size()) { spins.resize(index + 1, UP); }
    return;
}

/* Accessors */

// Get the spin config vector of the graph
std::vector<Spin> Graph::getSpins() const { return this->spins; }

// Get the vertical energy of the graph
std::vector<double> Graph::getVerticalEnergyProduct(const int& length) {
    std::vector<double> list_of_energy(length, 0.0);

    for (int i = 0; i < length; ++i) {
        const int self_idx = adj_list[i]->val;
        AdjNode *tmp = adj_list[i];
        int current_layer = 0;

        // \sum_{i=1}^L { \sum_{l=1}^{L_tau} { s_i^l * s_i^{l+1} } }
        while (tmp->val != self_idx) {
            const int layer_up_idx = get_layer_up(tmp->val, length, current_layer);
            list_of_energy[i] += (double)spins[tmp->val] * (double)spins[layer_up_idx];
            ++current_layer;
        }
    }

    return list_of_energy;
}

// Get the Hamiltonian energy of the graph
double Graph::getHamiltonianEnergy() {
    double sum = 0.0;
    // std::cout << "adj_list.size() = " << adj_list.size() << std::endl;
    for (int i = 0; i < adj_list.size(); ++i) {
        // std::cout << i << std::endl;
        AdjNode *tmp = adj_list[i];
        if (tmp == nullptr) continue;
        const double spin = (double)spins[i]; // Get spin of current node

        while (tmp != nullptr) {
            if (tmp->val > i) {
                tmp = tmp->next;
                continue;
            }
            // std::cout << "i = " << i << " tmp->val = " << tmp->val << std::endl;
            // std::cout << "tmp->val = " << (double)spins[tmp->val] << std::endl;
            // std::cout << "sum: " << sum << std::endl;
            sum += tmp->weight * spin * (double)spins[tmp->val];
            tmp = tmp->next;
        }
    }
    // Calculate the linear terms
    for (auto const& it : constant_map) {
        sum += it.second * (double)spins[it.first];
    }
    // Calculate the constant term
    sum += constant;
    return sum;
}

// Get the Hamiltonian energy of each layer of the graph
std::vector<double> Graph::getLayerHamiltonianEnergy(const int& height) {
    std::vector<double> list_of_energy(height, 0.0);
    double current_sum = 0.0;
    for (int i = 0; i < adj_list.size(); ++i) {
        AdjNode *tmp = adj_list[i];
        if (tmp == nullptr) continue;
        const double spin = (double)spins[i]; // Get spin of current node

        while (tmp != nullptr) {
            if (tmp->val > i) {
                tmp = tmp->next;
                continue;
            }
            current_sum += tmp->weight * spin * (double)spins[tmp->val];
            tmp = tmp->next;
        }

        // Refresh the current_sum if the current node is the last node of the layer
        const int length_square = adj_list.size() / height;
        if ((i + 1) % length_square == 0) {
            list_of_energy[i / length_square] = current_sum;
            current_sum = 0.0;
            // std::cout << "Current index = " << i << std::endl;
        }
    }

    return list_of_energy;
}

// Get the Hamiltonian difference given the indices to flip and the spin
double Graph::getHamiltonianDifference(const int& index) {
    double sum_to_modify = 0.0;
    const double spin = (double)spins[index];
    AdjNode *tmp = adj_list[index];
    while (tmp != nullptr) {
        sum_to_modify += tmp->weight * spin * (double)spins[tmp->val];
        tmp = tmp->next;
    }
    // std::cout << -2.0 * sum_to_modify << std::endl;
    if (constant_map.find(index) != constant_map.end()) { sum_to_modify += constant_map[index] * (double)spins[index]; }
    return -2.0 * sum_to_modify;
}

// Get the order parameter length squared
std::vector<double> Graph::getOrderParameterLengthSquared(const int& length, const int& height) {
    const std::complex<double> image_pi(0.0, (4.0 / 3.0) * M_PI);
    const std::complex<double> math_e(M_E, 0.0);

    // Create a vector of vectors to store the color parameters and count of each color
    // m_color_params[layer][color] = count of blue, black, red
    std::vector<std::vector<double> > m_color_params(height, std::vector<double>(3, 0));

    auto get_sub_lattice = [&] (const int& index) { return ((index / length) + index) % 3; };

    const int each_color_count_per_layer_int = (length * length) / 3;
    const double each_color_count_per_layer = (double)each_color_count_per_layer_int;
    const int total_count = this->spins.size();

    for (int i = 0; i < total_count; ++i) {
        const int layer = i / (length * length);
        switch (get_sub_lattice(i)) {
            case 0: m_color_params[layer][0] += (double)this->spins[i]; break;
            case 1: m_color_params[layer][1] += (double)this->spins[i]; break;
            case 2: m_color_params[layer][2] += (double)this->spins[i]; break;
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

    return layer_order_parameter_length_squared;
}

/* Constructor */
Graph::Graph() {
    this->adj_list = std::vector<AdjNode *> {};
    this->adj_map = std::map<int, std::vector<int> > {};
    this->constant_map = std::map<int, double> {};
    this->spins = std::vector<Spin> {};
    this->constant = 0.0;
}

/* Manipulator */

// Push back an edge
void Graph::pushBack(const int& po1, const int& po2, const double& co) {
    AdjNode *node1 = new AdjNode(po2, co);
    privatePushBack(po1, node1);
    if (po1 == po2) return; // self loop
    AdjNode *node2 = new AdjNode(po1, co);
    privatePushBack(po2, node2);
    return;
};

void Graph::pushBack(const int& po, const double& co) {
    privatePushBack(po, co);
    return;
}

void Graph::pushBack(const double& co) {
    this->constant += co;
    return;
}

// Flip the spin of the given index
void Graph::flipSpin(const int& index) { spins[index] = (spins[index] == UP) ? DOWN : UP; }

// Update the gamma of the graph
void Graph::updateGamma(const double& gamma, const int& length, const int& height) {
    char gamma_update_flag = 0X00; // check if gamma is updated for both up and down
    for (int i = 0; i < adj_list.size(); ++i) {
        AdjNode *tmp = adj_list[i];
        const int next_layer_idx = get_layer_up(i, length, height);
        const int prev_layer_idx = get_layer_down(i, length, height);
        // char gamma_update_flag = 0X00;
        while (tmp != nullptr) {
            if (!(gamma_update_flag ^ 0X03)) { // if gamma_update_flag == 0X03
                break;
            } else if (tmp->val == next_layer_idx) {
                tmp->weight = gamma;
                gamma_update_flag |= 1;
            } else if (tmp->val == prev_layer_idx) {
                tmp->weight = gamma;
                gamma_update_flag |= 2;
            }
            tmp = tmp->next;
        }
        gamma_update_flag = 0X00; // reset gamma_update_flag
    }
    return;
}

/* Printer */

void Graph::print() {
    std::cout << "Adjacency List:" << std::endl;
    for (int i = 0; i < adj_list.size(); i++) {
        AdjNode *tmp = adj_list[i];
        std::cout << i << ": ";
        while (tmp != nullptr) {
            std::cout << tmp->val << " " << tmp->weight << " | ";
            tmp = tmp->next;
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << "Adjacency Map:" << std::endl;
    for (std::map<int, std::vector<int> >::iterator it = adj_map.begin(); it != adj_map.end(); it++) {
        std::cout << it->first << ": ";
        for (int i = 0; i < it->second.size(); i++) {
            std::cout << it->second[i] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << "Constant Map: " << std::endl;
    for (std::map<int, double>::iterator it = constant_map.begin(); it != constant_map.end(); it++) {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    std::cout << std::endl << "Constant: " << constant << std::endl;

    std::cout << std::endl << "Spins:" << std::endl;
    for (int i = 0; i < spins.size(); i++) {
        std::cout << i << ": " << spins[i] << std::endl;
    }
    std::cout << std::endl;

    return;
}
