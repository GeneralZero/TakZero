#include <string>
#include <iostream>
#include <assert.h>
#include <random>

#include "Random.h"
#include "config.h"
#include "NNCache.h"
#include "DLNetwork.h"
#include "Utility.h"
#include "ThreadPool.h"
#include "LoadNetwork.h"


extern Utils::ThreadPool thread_pool;
std::mutex DLNetwork::m_mutex;


void DLNetwork::initialize() {
	std::cout << "Connecting to hello world server" << std::endl;
}

Netresult DLNetwork::get_scored_moves(Board * state) {
	NNOutput ml_out;
	NNInput ml_in;
	std::vector<scored_node> result;

	ml_in = state->getMLData();
	// See if we already have this in the cache.
	if (!ConfigStore::get().bools.at("skip_cache")) {
		if (!NNCache::get_NNCache().lookup(state.get_hash(), ml_out)) {
			// Insert result into cache.
			ml_out = get_ml_out(ml_in);
			NNCache::get_NNCache().insert(state.get_hash(), ml_out);
		}
	}
	else {
		ml_out = get_ml_out(ml_in);
	}

	//Get board winrate
	float black_winrate = ml_out.second;
	float winrate_sig = (1.0f + std::tanh(black_winrate)) / 2.0f;

	//Return Valid Moves	
	std::vector<Play> moves = state->getAllPlays();
	std::default_random_engine generator(std::random_device{}());
	std::shuffle(moves.begin(), moves.end(), generator);

	//Include only valid moves.
	for (Play move : moves)
	{
		result.emplace_back(1 / (double)moves.size(), move.index);
	}

	return std::make_pair(result, winrate_sig);
}

NNOutput DLNetwork::get_ml_out(NNInput ml_data) {
	//Lock for socket connection
	//std::lock_guard<std::mutex> lock(m_mutex);

	return LoadNetwork::PredictOne(ml_data);

	//Send by socket
	//return get_ml_out_socket(ml_data);
}

NNOutput DLNetwork::get_ml_out_socket(NNInput ml_data) {
	//Lock for socket connection
	//std::lock_guard<std::mutex> lock(m_mutex);

	NNOutput result;

	//Calculate Message length
	std::uint64_t size = sizeof(NNInput);
	char recv_buffer[8000];
	char send_buffer[8000];
	memcpy(send_buffer, ml_data.data(), size);

	void *context = zmq_ctx_new();
	void *requester = zmq_socket(context, ZMQ_REQ);

	//Send Message
	//std::cout << "Sending NNinput to Python process" << std::endl;
	std::string dest = "tcp://localhost:" + std::to_string(ConfigStore::get().ints.at("ipc_port"));
	zmq_connect(requester, dest.c_str());

	zmq_send(requester, send_buffer, size, 0);

	// Get the reply.
	zmq_recv(requester, recv_buffer, 1576 * sizeof(float), 0);

	//Unlock after socket read
	//m_mutex.unlock();

	std::array<float, 1576> tmp;
	//std::cout << "Receiving NNOutput from python of size "<< 1576 * sizeof(float) << std::endl;

	std::memcpy(tmp.data(), recv_buffer, 1576 * sizeof(float));
	result.second = tmp[1575];
	zmq_close(requester);
	zmq_ctx_destroy(context);
	return result;
}


