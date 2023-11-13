#include "line.h"

size_t std::hash<Line>::operator()(const Line &key) const {
    return (std::hash<len_t>()(key.low())) ^ (std::hash<len_t>()(key.high()));
}

std::ostream &operator<<(std::ostream &os, const Line &c) {
    os << "[" << c.low() << ", " << c.high() << "]";
    return os;
}