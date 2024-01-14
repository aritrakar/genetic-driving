#ifndef _LAYER_H_
#define _LAYER_H_

#include <vector>
#include "neuron.h"

class Layer {
public:
	Layer(size_t n_neurons, size_t n_weights) {
		for (int n = 0; n < n_neurons; ++n) {
			m_neurons.emplace_back(Neuron(n_weights));
		}
	};

	std::vector<Neuron> m_neurons;
};

#endif
