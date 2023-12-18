#ifndef _HELPER_H_
#define _HELPER_H_

/* Triangular lattice helper functions */

// Node's current height, current row, current column, lattice's length
inline int GETRIGHT (const int& h, const int& i, const int& j, const int& l) { return ((h * l * l) + (i * l) + ((j + 1) % l)); }
inline int GETBOTTOM (const int& h, const int& i, const int& j, const int& l) { return ((h * l * l) + (((i + 1) % l) * l) + j); }
inline int GETBOTTOMRIGHT (const int& h, const int& i, const int& j, const int& l) { return ((h * l * l) + (((i + 1) % l) * l) + ((j + 1) % l)); }
// Node's current height, current row, current column, lattice's length, lattice's height
inline int GETLAYERUP (const int& h, const int& i, const int& j, const int& l, const int& height) {
    return ((((h + 1) % height) * l * l) + (i * l) + j);
}

// i for index, l for length, h for height
#define GET_I(index, length) ((index / length) % length)
#define GET_J(index, length) (index % length)
#define GET_H(index, length) (index / (length * length))
inline int get_left (const int i, const int l) {
    const int _j = (GET_J(i, l) - 1) >= 0 ? (GET_J(i, l) - 1) : (GET_J(i, l) - 1) + l;
    return GET_H(i, l) * l * l + GET_I(i, l) * l + _j;
}
inline int get_up_left (const int i, const int l) {
    const int _i = (GET_I(i, l) - 1) >= 0 ? GET_I(i, l) - 1 : GET_I(i, l) - 1 + l;
    const int _j = (GET_J(i, l) - 1) >= 0 ? GET_J(i, l) - 1 : GET_J(i, l) - 1 + l;
    return GET_H(i, l) * l * l + _i * l + _j;
}
inline int get_up (const int i, const int l) {
    const int _i = (GET_I(i, l) - 1) >= 0 ? GET_I(i, l) - 1 : GET_I(i, l) - 1 + l;
    return GET_H(i, l) * l * l + _i * l + GET_J(i, l);
}
inline int get_layer_down (const int i, const int l, const int h) {
    const int _h = (GET_H(i, l) - 1) >= 0 ? GET_H(i, l) - 1 : GET_H(i, l) - 1 + h;
    return _h * l * l + GET_I(i, l) * l + GET_J(i, l);
}
inline int get_layer_up (const int i, const int l, const int h) {
    const int _h = (GET_H(i, l) + 1) % h;
    return _h * l * l + GET_I(i, l) * l + GET_J(i, l);
}

#endif
