#include "nn.h"
#include <iostream>

std::vector<float> NN::forward_propagate(std::vector<float> inputs) {
	std::vector<float> newInputs;
	for (int nLayer = 0; nLayer < m_nLayers; ++nLayer) {
		newInputs.clear();

		// Get this layer's neurons.
		// Q: Does this need to be a reference?
		std::vector<Neuron>& layerNeurons = this->m_layers[nLayer].m_neurons;
		size_t nNeurons = layerNeurons.size();
		for (Neuron& neuron : layerNeurons) {
			neuron.activate(inputs);
			newInputs.emplace_back(neuron.m_output);
		}

		inputs = newInputs;
	}
	return inputs;
}

void NN::backward_propagate_error(std::vector<float> expected) {
	// Traverse the layers in reverse order
	for (int nLayer = m_nLayers - 1; nLayer > 0; --nLayer) {
		// Get this layer's neurons
		std::vector<Neuron>& layerNeurons = this->m_layers[nLayer].m_neurons;

		// Calculate error
		for (int n = 0; n < layerNeurons.size(); ++n) {
			double error = 0.0;

			if (nLayer == m_nLayers - 1) {
				// Error for the output layer
				error = expected[n] - layerNeurons[n].m_output;
			}
			else {
				for (Neuron& neuron : m_layers[nLayer + 1].m_neurons) {
					error += neuron.m_weights[n] * neuron.m_delta;
				}
			}

			// Update delta value of the neuron
			layerNeurons[n].m_delta = error * layerNeurons[n].transfer_derivative();
		}
	}
}

void NN::update_weights(std::vector<float> inputs, float l_rate) {
	for (size_t nLayer = 0; nLayer < m_nLayers; ++nLayer) {
		std::vector<Neuron>& layerNeurons = this->m_layers[nLayer].m_neurons;

		// If not the first layer, the inputs are the outputs of the previous layer
		if (nLayer != 0) {
			inputs.clear();
			for (const Neuron& prevNeuron : this->m_layers[nLayer - 1].m_neurons) {
				inputs.push_back(prevNeuron.m_output);
			}
		}

		for (Neuron& neuron : layerNeurons) {
			// Update each weight (excluding the bias weight)
			for (size_t w = 0; w < neuron.m_weights.size() - 1; w++) {
				neuron.m_weights[w] += l_rate * neuron.m_delta * inputs[w];
			}

			// Update the bias weight, which is the last weight in the array
			neuron.m_weights.back() += l_rate * neuron.m_delta;
		}
	}
}

void NN::train(std::vector<std::vector<float>>training_data, float l_rate, size_t n_epoch, size_t n_outputs) {
	for (size_t epoch = 0; epoch < n_epoch; ++epoch) {
		float sum_error = 0;

		for (const auto& row : training_data)
		{
			std::vector<float> outputs = this->forward_propagate(row);
			std::vector<float> expected(n_outputs, 0.0);
			expected[static_cast<int>(row.back())] = 1.0;
			for (size_t x = 0; x < n_outputs; x++)
			{
				// Squared error
				sum_error += static_cast<float>(std::pow((expected[x] - outputs[x]), 2));
			}
			this->backward_propagate_error(expected);
			this->update_weights(row, l_rate);
		}
		std::cout << "[>] epoch=" << epoch << ", l_rate=" << l_rate << ", error=" << sum_error << std::endl;
	}
}

int NN::predict(std::vector<float> inputs) {
	// Forward propagate
	std::vector<float> outputs = this->forward_propagate(inputs);

	// TODO: Change?
	return std::max_element(outputs.begin(), outputs.end()) - outputs.begin();
}

void NN::display_human() {
	std::cout << "[Network] (Layers: " << m_nLayers << ")" << std::endl;

	std::cout << "{" << std::endl;
	for (size_t l = 0; l < m_layers.size(); l++)
	{
		Layer layer = m_layers[l];
		std::cout << "\t (Layer " << l << "): {";
		for (size_t i = 0; i < layer.m_neurons.size(); i++)
		{
			Neuron neuron = layer.m_neurons[i];
			std::cout << "<(Neuron " << i << "): [ weights={";
			std::vector<float> weights = neuron.get_weights();
			for (size_t w = 0; w < weights.size(); ++w)
			{
				std::cout << weights[w];
				if (w < weights.size() - 1) {
					std::cout << ", ";
				}
			}
			std::cout << "}, output=" << neuron.m_output << ", activation=" << neuron.m_activation << ", delta=" << neuron.m_delta;
			std::cout << "]>";
			if (i < layer.m_neurons.size() - 1) {
				std::cout << ", ";
			}
		}
		std::cout << "}";
		if (l < m_layers.size() - 1) {
			std::cout << ", ";
		}
		std::cout << std::endl;
	}
	std::cout << "}" << std::endl;
}
