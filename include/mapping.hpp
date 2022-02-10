#ifndef MAPPING_HPP
#define MAPPING_HPP
#include <map>
#include <algorithm>
#include <string>
#include "delay.hpp"
#include "node.hpp"
#include "orbit.hpp"
#include "stream.hpp"
//std::map<std::string, typename> creation_mapping();

ExponentialDelay CreateExponentialDelay();
UniformDelay CreateUniformDelay();
GammaDelay CreateGammaDelay();

RQTNode CreateRQTNode();
RQNode CreateRQNode();
SimpleNode CreateSimpleNode();

Orbit CreateOrbit();

SimpleInput CreateSimpleInput();
MMPP CreateMMPP();

#endif
