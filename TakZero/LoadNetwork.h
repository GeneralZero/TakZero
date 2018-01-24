#pragma once

//#define COMPILER_MSVC
//#define NOMINMAX

#include "DLNetwork.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/cc/saved_model/loader.h"
#include "tensorflow/cc/ops/array_ops.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/public/session.h"

class LoadNetwork
{
public:
	static void initalize(std::string const & model_path);
	~LoadNetwork();

	static NNOutput PredictOne(NNInput const & input);

	//int LoadCheckpoint(std::string const & checkpoint_path);
	static tensorflow::Session* tfsession;

private:
	
};