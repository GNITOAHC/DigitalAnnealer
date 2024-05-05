#include "../include/Helper.h"
#include "Graph.h"

#include <cmath>

#define debug(n) std::cerr << n << std::endl;

const double E = std::exp(1.0);
inline double loge (double x) { return std::log(x) / std::log(E); }

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
double Graph::getHamiltonianEnergy() const {
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
std::vector<double> Graph::getLayerHamiltonianEnergy() {
    const int height = this->spins.size() / this->length;
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

int Graph::getLength() const { return this->length; }
int Graph::getHeight() const { return this->spins.size() / this->length; }

/* Constructor */
Graph::Graph() {
    this->adj_list = std::vector<AdjNode *> {};
    this->adj_map = std::map<int, std::vector<int> > {};
    this->constant_map = std::map<int, double> {};
    this->spins = std::vector<Spin> {};
    this->constant = 0.0;
    this->length = 0;
}

Graph::Graph(const Graph& g) {
    this->adj_list = g.adj_list;
    this->adj_map = g.adj_map;
    this->constant_map = g.constant_map;
    this->spins = g.spins;
    this->constant = g.constant;
    this->length = g.length;
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

void Graph::growLayer(const int& grow_count, const double& gamma) {
    const int length = this->getLength();
    const int height = this->spins.size() / length;
    const int origin_constant = this->constant / height;
    const double g = (-0.5) * loge(tanh(gamma));
    for (int i = 0; i < grow_count; ++i) {
        // Duplicate the current layer to the new layer
        for (int j = 0; j < length; ++j) {
            const int index = (i + 1) * length + j; // New layer's index
            // Add edge between the new layer
            AdjNode *tmp = adj_list[j];
            while (tmp != nullptr) {
                const int corr_node = tmp->val + (length * (i + 1));
                // Prevent from adding edge to the next layer
                if (tmp->val == j + length) {
                    tmp = tmp->next;
                    continue;
                }
                // Prevent from adding duplicate edge
                if (index < corr_node) this->pushBack(index, corr_node, tmp->weight);
                tmp = tmp->next;
            }
            // Add constant map of the new layer
            if (constant_map.find(j) != constant_map.end()) { this->pushBack(index, constant_map[j]); }
        }
        // Add constant of the new layer
        this->constant += origin_constant;
        // Add edge between the new layer & the previous layer
        for (int j = 0; j < length; ++j) {
            const int index = i * length + j; // Previous layer's index
            const int next_layer_idx = (index + length) % (length * (grow_count + 1));
            // std::cout << "index = " << index << " next_layer_idx = " << next_layer_idx << std::endl;
            this->pushBack(index, next_layer_idx, g);
        }
    }
    // Link back to the first layer
    for (int j = 0; j < length; ++j) {
        const int index = grow_count * length + j;
        const int next_layer_idx = j;
        // std::cout << "index = " << index << " next_layer_idx = " << next_layer_idx << std::endl;
        this->pushBack(index, next_layer_idx, g);
    }
    return;
}

// Flip the spin of the given index
void Graph::flipSpin(const int& index) { spins[index] = (spins[index] == UP) ? DOWN : UP; }

// Update the gamma of the graph
void Graph::updateGamma(const double& gamma) {
    char gamma_update_flag = 0X00; // check if gamma is updated for both up and down
    const int length = this->getLength();
    const int height = this->spins.size() / length;
    // std::cout << "length = " << length << " height = " << height << std::endl;
    for (int i = 0; i < adj_list.size(); ++i) {
        AdjNode *tmp = adj_list[i];
        // const int next_layer_idx = get_layer_up(i, length, height);
        // const int prev_layer_idx = get_layer_down(i, length, height);
        const int next_layer_idx = (i + length) % (length * height);
        const int prev_layer_idx = (i - length) >= 0 ? i - length : i % length;
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

void Graph::lockLength() { this->length = this->spins.size(); }
void Graph::lockLength(const int& ll) { this->length = ll; }

/* Printer */

void Graph::print(std::ofstream& cout) {
    cout << "Adjacency List:" << std::endl;
    for (int i = 0; i < adj_list.size(); i++) {
        AdjNode *tmp = adj_list[i];
        cout << i << ": ";
        while (tmp != nullptr) {
            cout << tmp->val << " " << tmp->weight << " | ";
            tmp = tmp->next;
        }
        cout << std::endl;
    }

    cout << std::endl << "Adjacency Map:" << std::endl;
    for (std::map<int, std::vector<int> >::iterator it = adj_map.begin(); it != adj_map.end(); it++) {
        cout << it->first << ": ";
        for (int i = 0; i < it->second.size(); i++) {
            cout << it->second[i] << " ";
        }
        cout << std::endl;
    }

    cout << std::endl << "Constant Map: " << std::endl;
    for (std::map<int, double>::iterator it = constant_map.begin(); it != constant_map.end(); it++) {
        cout << it->first << ": " << it->second << std::endl;
    }

    cout << std::endl << "Constant: " << constant << std::endl;

    cout << std::endl << "Spins:" << std::endl;
    for (int i = 0; i < spins.size(); i++) {
        cout << i << ": " << spins[i] << std::endl;
    }
    cout << std::endl;

    return;
}

void Graph::printHLayer(std::ofstream& cout) {
    cout << "layer\thamiltonian\th_per_layer\n";
    const std::vector<double> h_per_layer = getLayerHamiltonianEnergy();
    for (int i = 0; i < h_per_layer.size(); ++i) {
        cout << i << "\t" << h_per_layer[i] << "\t" << h_per_layer[i] / this->length << std::endl;
    }
    return;
}

void Graph::printConfig(std::ofstream& cout) {
    const int length = this->getLength();
    cout << "index\tspin\n";
    for (int i = 0; i < length; ++i) {
        cout << i << "\t" << spins[i] << std::endl;
    }
    return;
}
