#ifndef _NN_H_
#define _NN_H_

#include <vector>
#include "layer.h"

class NN {
public:
	/*NN(size_t input_size, size_t hidden_layers, size_t output_size) : m_nLayers{hidden_layers} {*/
	NN() : m_nLayers{ 0 } {
		add_layer(10, 0); // l1
		add_layer(5, 10); // l2
		add_layer(5, 5);  // l3
		add_layer(3, 5);  // l4
		add_layer(2, 3);  // l5
	}

	void add_layer(int n_neurons, int n_weights) {
		m_layers.push_back(Layer(n_neurons, n_weights));
		m_nLayers++;
	}

	std::vector<float> forward_propagate(std::vector<float> inputs);
	void backward_propagate_error(std::vector<float> expected);
	void update_weights(std::vector<float> inputs, float l_rate);
	void train(std::vector<std::vector<float>>trainings_data, float l_rate, size_t n_epoch, size_t n_outputs);
	int predict(std::vector<float> input);

	void display_human();

	int m_nLayers;
	std::vector<Layer> m_layers;
};

#endif
