#include "LoadNetwork.h"

//#undef min
//#undef max

#include <iostream>
#include <algorithm>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

tensorflow::Session* LoadNetwork::tfsession;

void LoadNetwork::initalize(std::string const& model_path) {

	tensorflow::SessionOptions sess_opt;
	//sess_opt.config.mutable_gpu_options()->set_allow_growth(true);

	tensorflow::Status status = tensorflow::NewSession(sess_opt, &tfsession);
	if (!status.ok()) {
		throw std::runtime_error(status.ToString());
	}

	tensorflow::GraphDef graph_def;
	status = ReadBinaryProto(tensorflow::Env::Default(), model_path, &graph_def);
	if (!status.ok()) {
		throw std::runtime_error(status.ToString());
	}

	status = tfsession->Create(graph_def);
	if (!status.ok()) {
		throw std::runtime_error(status.ToString());
	}
}

LoadNetwork::~LoadNetwork() {
	tfsession->Close();
}

NNOutput LoadNetwork::PredictOne(NNInput const& input)
{
	std::vector<tensorflow::Tensor> outputs;
	std::pair<std::array<float, 1575>, float> ret;

	//Refactor input tensor
	tensorflow::Tensor input_tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({ 1,8,5,5,32 }));
	//input_tensor.dim_size(5);
	//std::cout << input_tensor.shape().DebugString() << std::endl;

	for (int i = 0; i < (int)input.size(); ++i) {
		//for each layer

		for (int j = 0; j < (int)input[i].size(); ++j) {
			//Calculate board offets
			std::uint32_t index = j / (32) ;
			std::uint32_t depth = j % 32 ;
			std::uint32_t x = (std::uint32_t)std::floor(index / 5);
			std::uint32_t y = (index % 5);
			//input_tensor.matrix<std::uint8_t>()(0, i, x, y, depth) = input[i][j];
			input_tensor.flat<float>()(i*input[i].size() + j) = (float)input[i][j];
		}
	}

	tensorflow::Status status = tfsession->Run({{ "input_0", input_tensor }}, { "output_node0", "output_node1" }, {}, &outputs);
	if (!status.ok()) {
		throw std::runtime_error(status.ToString());
	}

	//std::cout << "Output0: " << outputs[0].DebugString() << "\n";
	//std::cout << "Output1: " << outputs[1].DebugString() << "\n";
	auto out_0 = outputs[0].flat<float>();
	auto out_1 = outputs[1].flat<float>();

	//std::cout << "Output0: " << out_0.data() << "\n";
	//std::cout << "Output1: " << out_1 << "\n";

	for (int i = 0; i < 1575; ++i)
	{
		ret.first[i] = out_0.data()[i];
	}

	ret.second = out_1.data()[0];

	return ret;
}

/*
int LoadNetwork::LoadCheckpoint(std::string const& checkpoint_path) {
	tensorflow::SavedModelBundle bundle;
	std::unordered_set<std::string> tmp_set({ "serve" });
	tensorflow::Status load_status = tensorflow::LoadSavedModel(
		tensorflow::SessionOptions(), tensorflow::RunOptions(), models, tmp_set, &bundle);
	if (!load_status.ok()) {
		std::cout << "Error loading model: " << load_status << std::endl;
		return -1;
	}
	std::cout << "model loaded" << std::endl;
	return 0;
}
*/
