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

/* Accessors */

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
    return sum;
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
    return -2.0 * sum_to_modify;
}

/* Manipulator */

// Push back an edge
void Graph::pushBack(const int& po1, const int& po2, const double& co) {
    AdjNode *node1 = new AdjNode(po2, co);
    privatePushBack(po1, node1);
    if (po1 == po2) return; // self loop
    AdjNode *node2 = new AdjNode(po1, co);
    privatePushBack(po2, node2);
};

// Flip the spin of the given index
void Graph::flipSpin(const int& index) { spins[index] = (spins[index] == UP) ? DOWN : UP; }

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

    std::cout << std::endl << "Spins:" << std::endl;
    for (int i = 0; i < spins.size(); i++) {
        std::cout << i << ": " << spins[i] << std::endl;
    }
    std::cout << std::endl;

    return;
}
