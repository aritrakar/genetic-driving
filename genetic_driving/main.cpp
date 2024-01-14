#include <SFML/Graphics.hpp>
#include "NeuralNetwork/nn.h"
#include "Car/car.h"

#include <iostream>
#include <thread>

void savebestCar(std::vector<Car>& cars, Car& bestCar) {
	float maxScore = 0; float indexMax = 0;

	for (int i = 0; i < cars.size(); i++) {
		if (cars[i].score > maxScore) {
			indexMax = i;
			maxScore = cars[i].score;
		}
	}

	bestCar.nn = cars[indexMax].nn;
	bestCar.score = cars[indexMax].score;
}

void evolveNN(NN& neuralNet, float percent) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> weightDist(-3.5f, 3.5f); // Adjust range as needed
	std::uniform_real_distribution<float> chanceDist(0.0f, 100.0f);

	for (Layer& layer : neuralNet.m_layers) {
		for (Neuron& neuron : layer.m_neurons) {
			std::vector<float>& weights = neuron.get_weights();
			for (float& weight : weights) {
				if (chanceDist(gen) < percent) {
					// weight = weightDist(gen); // Randomly mutate the weight (and bias)
					weight = (rand() % 1000 - 500) / 200;
				}
			}
		}
	}
}


void updateCars(std::vector<Car>& cars, sf::Image mapImg, std::vector<Checkpoint>& checkpoints, std::vector<Car>& bestCars, bool& end) {
	bool alldead = true;
	for (int i = 0; i < cars.size(); i++) {
		cars[i].update(mapImg, checkpoints, bestCars);
		alldead = alldead && cars[i].dead;
	}

	if (alldead) {
		savebestCar(cars, bestCars[0]);
		end = true;
	}
}

void drawCars(std::vector<Car> cars, sf::RenderWindow& window) {
	for (int i = 0; i < cars.size(); i++) {
		if (!cars[i].dead) {
			cars[i].draw(window);
		}
	}
}

void drawCheckpoints(std::vector<Checkpoint>& checkpoints, sf::RenderWindow& window) {
	for (int i = 0; i < checkpoints.size(); i++) {
		window.draw(checkpoints[i].rect);
	}
}

void initCheckpoints(std::vector<Checkpoint>& checkpoints, int num) {
	checkpoints.clear();

	if (num == 1) {
		checkpoints.emplace_back(sf::Vector2f(400, 230), sf::Vector2f(200, 15), M_PI / 4, 1);
		checkpoints.emplace_back(sf::Vector2f(980, 260), sf::Vector2f(210, 15), M_PI / 1.9, 2);
		checkpoints.emplace_back(sf::Vector2f(1550, 375), sf::Vector2f(300, 15), M_PI / -4, 3);
		checkpoints.emplace_back(sf::Vector2f(1521, 800), sf::Vector2f(300, 15), 0.1, 4);
		checkpoints.emplace_back(sf::Vector2f(990, 890), sf::Vector2f(200, 15), M_PI / 2, 5);
		checkpoints.emplace_back(sf::Vector2f(440, 870), sf::Vector2f(200, 15), -M_PI / 4, 6);
		checkpoints.emplace_back(sf::Vector2f(405, 575), sf::Vector2f(120, 40), -0.1, 7);
	}
	else if (num == 2) {
		// For map2.png
		checkpoints.emplace_back(sf::Vector2f(400, 230), sf::Vector2f(200, 15), M_PI / 4, 1);
		checkpoints.emplace_back(sf::Vector2f(1083, 189), sf::Vector2f(210, 15), M_PI / 1.9, 2);
		checkpoints.emplace_back(sf::Vector2f(1600, 375), sf::Vector2f(470, 15), M_PI / -4, 3);
		checkpoints.emplace_back(sf::Vector2f(1571, 910), sf::Vector2f(135, 15), 0.1, 4);
		checkpoints.emplace_back(sf::Vector2f(980, 800), sf::Vector2f(120, 15), M_PI / 2, 5);
		checkpoints.emplace_back(sf::Vector2f(400, 876), sf::Vector2f(170, 15), -M_PI / 4, 6);
		checkpoints.emplace_back(sf::Vector2f(410, 613), sf::Vector2f(120, 30), 0.08, 7);
	}
	else if (num == 3) {
		checkpoints.emplace_back(sf::Vector2f(1450, 800), sf::Vector2f(175, 15), M_PI / 6, 1);
		checkpoints.emplace_back(sf::Vector2f(1567, 367), sf::Vector2f(110, 15), M_PI / 2, 2);
		checkpoints.emplace_back(sf::Vector2f(741, 201), sf::Vector2f(115, 15), M_PI / 2, 3);
		checkpoints.emplace_back(sf::Vector2f(385, 787), sf::Vector2f(180, 15), -0.1, 4);
		checkpoints.emplace_back(sf::Vector2f(849, 965), sf::Vector2f(130, 15), M_PI / 2, 5);
	}

	std::cout << "Initialized checkpoints." << std::endl;
}

void initStartPos(Car& c, int lvl) {
	if (lvl == 1 || lvl == 2) {
		c.position = sf::Vector2f(400, 500);
		c.orientation = sf::Vector2f(0, -1);
	}
	else {
		c.position = sf::Vector2f(1059, 951);
		c.orientation = sf::Vector2f(1, 0);
	}
}

void write(sf::RenderWindow& window, std::string t1, sf::Vector3f v, sf::Color col) {
	// Load font
	sf::Font font;
	font.loadFromFile("./Roboto-Regular.ttf");

	//create the text element
	sf::Text text(t1, font);
	text.setStyle(sf::Text::Bold);
	text.setFillColor(col);

	text.setCharacterSize(v.z);

	text.setPosition(sf::Vector2f(v.x, v.y));

	window.draw(text);
}

int main()
{
	const int FONT_SIZE = 30;
	const int WAIT_TIME = 0; // in ms, between each frame
	const int N_CARS = 1500;
	const int N_SAVED_CARS = 10;
	const float EVOLUTION = 7; // Average percent change of each generation
	const bool CHANGEMAP = false;
	const float FRICTION = 0.9;
	const float CAR_SPEED = 1;
	const bool SHOW_CHECKPOINTS = false;

	int mapn = 1;

	std::srand(std::time(nullptr));

	int windowWidth = MAX_PIXEL_X;
	int windowHeight = MAX_PIXEL_Y;

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "DrivingSimulation", sf::Style::Fullscreen);
	sf::Texture mapTxt;
	if (!mapTxt.loadFromFile("../images/map" + std::to_string(mapn) + "Texture.png")) {
		std::cerr << "Error: Map texture not found." << std::endl;
		return -1;
	}

	sf::Image mapImg;
	if (!mapImg.loadFromFile("../images/map" + std::to_string(mapn) + ".png")) {
		std::cerr << "Error: Map image not found." << std::endl;
		return -1;
	}

	sf::RectangleShape Map;
	Map.setTexture(&mapTxt);
	Map.setSize(sf::Vector2f(windowWidth, windowHeight));
	Map.setPosition(sf::Vector2f(0, 0)); // Top left corner
	window.draw(Map);

	sf::RectangleShape whiteRect;
	whiteRect.setFillColor(sf::Color(255, 255, 255, 150));
	whiteRect.setSize(sf::Vector2f(240, 175));
	whiteRect.setPosition(sf::Vector2f(0, 0)); // Top left corner

	Car bestCar;
	bestCar.init(sf::Vector2f(0, 0), sf::Vector2f(1, 0.01));
	bestCar.friction = FRICTION;
	initStartPos(bestCar, mapn);

	// Initialize the cars
	std::vector<Car> cars;
	cars.reserve(N_CARS); // Reserve space for N_CARS elements
	for (int i = 0; i < N_CARS; i++) {
		Car c;
		c.init(bestCar.position, bestCar.orientation);
		c.friction = FRICTION;
		c.speedFactor = CAR_SPEED;
		cars.emplace_back(c);
	}

	std::cout << "Initialized cars." << std::endl;

	// Vector to store the best cars which'll be the next generation's parents
	std::vector<Car> bestCarsVector(N_SAVED_CARS, bestCar);

	// Initialize checkpoints
	std::vector<Checkpoint> checkpoints;
	initCheckpoints(checkpoints, mapn);

	int generation = 0;
	bool endOfGeneration = false;

	// Sleeping in small increments allows checking for user input
	for (int i = 0; i < 10; i++) { // was 100
		sf::sleep(sf::seconds(0.1));
	}

	std::cout << "Starting main loop..." << std::endl;

	// Main loop
	while (window.isOpen()) {
		if (endOfGeneration) {
			std::cout << "Generation " << generation << ". Best score: " << bestCarsVector[0].score << std::endl;
			generation++;
			if (CHANGEMAP) {
				mapn = generation % 3 + 1;
				std::string mapBasePath = "../images/map" + std::to_string(mapn);
				if (!mapTxt.loadFromFile(mapBasePath + "Texture.png") || !mapImg.loadFromFile(mapBasePath + ".png"))
					return -1;
				initCheckpoints(checkpoints, mapn);

				Map.setTexture(&mapTxt);
			}

			initStartPos(bestCar, mapn);

			// General the next generation via evolution
			int parentIndex = 0;
			for (int i = 0; i < N_CARS; i++) {
				cars[i].init(sf::Vector2f(400, 500), sf::Vector2f(0, -1));
				initStartPos(cars[i], mapn);

				// parentIndex = floor((float)(i) / N_CARS * N_SAVED_CARS);
				parentIndex = floor((float)(i) / N_CARS * N_SAVED_CARS);
				cars[i].nn = bestCarsVector[parentIndex].nn;
				cars[i].speedFactor = CAR_SPEED;
				evolveNN(cars[i].nn, EVOLUTION);
			}

			// Update the best score so far
			bestCar.score = bestCarsVector[0].score;
			for (int i = 0; i < N_SAVED_CARS; i++) {
				bestCarsVector[i].score = 0;
			}

			endOfGeneration = false;
		}

		std::thread tup(updateCars, std::ref(cars), mapImg, std::ref(checkpoints), std::ref(bestCarsVector), std::ref(endOfGeneration));
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		window.clear();

		window.draw(Map);
		drawCars(cars, window);
		window.draw(whiteRect);

		if (SHOW_CHECKPOINTS) drawCheckpoints(checkpoints, window);
		
		write(window, "Generation #" + std::to_string(generation), sf::Vector3f(10, 10, FONT_SIZE), sf::Color::Black);
		write(window, "Best: " + std::to_string((int)(bestCarsVector[0].score * M_PI)), sf::Vector3f(10, 50, FONT_SIZE), sf::Color::Black);

		if (!CHANGEMAP) {
			write(window, "Prev. best: " + std::to_string((int)(bestCar.score * M_PI)), sf::Vector3f(10, 90, FONT_SIZE), sf::Color::Black); //don't display the true score because it's not precise enough when rounded
			write(window, "Friction: " + std::to_string((int)(100 * (1 - bestCar.friction))) + "%", sf::Vector3f(10, 130, FONT_SIZE), sf::Color::Black);
		}
		else {
			write(window, "Friction: " + std::to_string((int)(100 * (1 - bestCar.friction))) + "%", sf::Vector3f(10, 90, FONT_SIZE), sf::Color::Black);
		}

		tup.join();
		window.display();

		// sf::sleep(sf::milliseconds(WAIT_TIME));
	}

    return 0;
}
