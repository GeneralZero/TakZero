#pragma once


#include <vector>
#include <string>
#include <bitset>
#include <memory>
#include <array>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#endif

#include <zmq.h>
#include <mutex>

#include "Board.h"
#include "config.h"

typedef std::pair<float, int> scored_node;
typedef std::pair<std::vector<scored_node>, float> Netresult;

typedef std::array<float, 1575> PlayProb;
typedef std::array<FastBoard, 8> NNInput;
typedef std::pair<PlayProb, float> NNOutput;

typedef std::vector<FastBoard> NNPlanes;

typedef float net_t;

class DLNetwork
{
public:
	static void initialize();
	static void initialize(std::uint16_t server_port);

	static Netresult get_scored_moves(Board * state);

	static void benchmark(Board * state, int iterations = 1600);
	static void show_heatmap(Board * state, Netresult & netres, bool topmoves);
	static void softmax(const std::vector<float>& input,
		std::vector<float>& output,
		float temperature = 1.0f);
	static void gather_features(Board* state, NNPlanes & planes);

	//static zmq::context_t context;
	//static zmq::socket_t socket;
	static std::mutex m_mutex;
private:
	static NNOutput get_ml_out(NNInput ml_data);

	static NNOutput get_ml_out_socket(NNInput ml_data);


};
