#ifndef _NEURON_H_
#define _NEURON_H_

#include <cmath>
#include <vector>
#include <random>
#include <iostream>

class Neuron {
public:
	Neuron(size_t n_weights) : m_nWeights(n_weights), m_delta(0.0f), m_activation(0.0f), m_output(0.0f) {
		// Initialize weights to random values
		std::random_device rd;
		std::mt19937 gen(rd());
		// std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
		std::uniform_real_distribution<float> dist(0.0f, 1.0f);

		for (size_t i = 0; i < m_nWeights + 1; ++i) {
 			// m_weights.emplace_back(dist(gen));
			m_weights.emplace_back(0);
		}
	}

	void activate(const std::vector<float>& inputs) {
		// Last weight is the bias
		m_activation = m_weights[m_nWeights];

		// Calculate activation
		for (size_t i = 0; i < m_nWeights; ++i) {
			m_activation += inputs[i] * m_weights[i];
		}

		// Transfer. Sigmoid activation function.
		m_output = 1.0f / (1.0f + exp(-m_activation));
	};

	float transfer_derivative() { return static_cast<float>(m_output * (1.0 - m_output)); };

	// Return mutable reference to the neuron weights
	std::vector<float>& get_weights(void) { return m_weights; };

	float m_output;
	float m_delta;
	std::vector<float> m_weights;
	float m_activation;

private:
	size_t m_nWeights;
};

#endif
