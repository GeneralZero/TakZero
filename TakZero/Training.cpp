#include "Training.h"
#include "DLNetwork.h"
//#include "FakeNetwork.h"
#include <experimental/filesystem>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#define SLASH "\\"
#else
#include <unistd.h>
#define SLASH "/"
#endif

#include <math.h>
#include <cmath>
#include "MD5.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#include "H5Cpp.h"

#include <curl/curl.h>
#include <curl/multi.h>

std::vector<TimeStep> Training::m_data{};

void Training::record(Board & state, UCTNode & root)
{
	auto step = TimeStep{};
	step.planes = state.getMLData();

	auto result =
		DLNetwork::get_scored_moves(&state);
	step.net_winrate = result.second;

	// Get total visit amount. We count rather
	// than trust the root to avoid ttable issues.
	std::uint64_t sum_visits = 0;

	auto a = root.possoble_moves.size();

	std::fill(step.probabilities.begin(), step.probabilities.end(), -1.0);
	
	for (size_t i = 0; i < a; i++)
	{
		sum_visits += root.possoble_moves[i]->get_visits();
	}

	if (sum_visits <= 0.0) {
		return;
	}

	for (size_t i = 0; i < a; i++) {
		float prob = static_cast<float>((float)root.possoble_moves[i]->get_visits() / (float)sum_visits);
		std::uint16_t move = root.possoble_moves[i]->get_move();
		step.probabilities[move] = prob;
	}

	this->m_data.emplace_back(step);

	//Rotate Board
	for (size_t i = 1; i < 8; i++)
	{
		auto stepr = transformTimeStep(step, i);
		this->m_data.push_back(stepr);
	}
}

std::string Training::dump_game(float winner)
{
	std::string directory = "Testing";

	//Check for directory

	//Create MD5 hash

	auto data_md5 = MD5((char*)this->m_data.data(), this->m_data.size() * sizeof(TimeStep));
	std::string filename = "Game_" + data_md5.hexdigest() + ".hdf5";

	//Save Game
	save_game(filename, winner);

	//Upload Game
	//uploadGame(directory, filename);
	return filename;
}

int Training::get_new_index(std::uint8_t index, std::uint8_t transformation)
{
	int old_x = index % this->rotate_Board.SIZE;
	int old_y = std::floor(index / this->rotate_Board.SIZE);
	int new_x = old_x;
	int new_y = old_y;
	if (transformation == 0) {
		return index;
	}
	switch (transformation)
	{
	default:
	case 1:
		//Flip Vertical
		new_x = this->rotate_Board.SIZE - old_x - 1;

		return (new_x + new_y * this->rotate_Board.SIZE);
	case 2:
		//Flip Horozontal and Vertical
		//Flip Vertical
		new_x = this->rotate_Board.SIZE - old_x - 1;

		//Flip Horozontal
		new_y = this->rotate_Board.SIZE - old_y - 1;

		return new_x + new_y * this->rotate_Board.SIZE;
	case 3:
		//Flip Horozontal
		new_y = this->rotate_Board.SIZE - old_y - 1;

		return new_x + new_y * this->rotate_Board.SIZE;
	case 4:
		//Rotate 90
		new_x = this->rotate_Board.SIZE - old_y -1;
		new_y = old_x;

		return new_x + new_y * this->rotate_Board.SIZE;
	case 5:
		//Rotate 90 and Flip Vertical
		//Flip Vertical
		new_x = this->rotate_Board.SIZE - old_x - 1;
		//Rotate 90
		new_x = this->rotate_Board.SIZE - old_y - 1;
		new_y = old_x;

		return new_x + new_y * this->rotate_Board.SIZE;
	case 6:
		//Rotate 90 and Flip Horozontal and Vertical
		//Flip Vertical
		new_x = this->rotate_Board.SIZE - old_x - 1;
		//Flip Horozontal
		new_y = this->rotate_Board.SIZE - old_y - 1;
		//Rotate 90
		new_x = this->rotate_Board.SIZE - old_y - 1;
		new_y = old_x;

		return new_x + new_y * this->rotate_Board.SIZE;
	case 7:
		//Rotate 90 and Flip Horozontal
		//Flip Horozontal
		new_y = this->rotate_Board.SIZE - old_y - 1;
		//Rotate 90
		new_x = this->rotate_Board.SIZE - old_y - 1;
		new_y = old_x;

		return new_x + new_y * this->rotate_Board.SIZE;
	}
}

FastBoard Training::rotateBoard(FastBoard board, std::uint8_t transformation)
{
	FastBoard new_board;

	for (size_t i = 0; i < 25; i++)
	{
		auto new_index = get_new_index(i, transformation);
		auto start = board.begin() + (i * 32);
		auto end = board.begin() + ((i + 1) * 32);
		std::copy(start, end, new_board.begin() + (new_index * 32));
	}
	return new_board;
}

TimeStep Training::transformTimeStep(TimeStep input, std::uint8_t transformation)
{
	TimeStep output;
	//Transform Boards
	std::uint16_t i = 0;
	for (; i < input.planes.size() - 2; i++) {
		output.planes[i] = rotateBoard(input.planes[i], transformation);
	}

	//Copy White Move Doard
	output.planes[i] = input.planes[i];
	i++;

	//Copy Black Move Board
	output.planes[i] = input.planes[i];


	std::fill(output.probabilities.begin(), output.probabilities.end(), -1.0);

	//Transform Probs
	for (i = 0; i < input.probabilities.size(); i++) {
		if (input.probabilities[i] != -1.0) {
			//Get move
			Play move = this->rotate_Board.GetMoveFromIndex(i);
			Play mover{move.type};

			//Rotate Move
			if (move.type == MoveStack) {
				mover.type = MoveStack;
				mover.order = move.order;

				int index = get_new_index(move.start.x + move.start.y * this->rotate_Board.SIZE, transformation);
				mover.start.x = index % this->rotate_Board.SIZE;
				mover.start.y = std::floor(index / this->rotate_Board.SIZE);

				index = get_new_index(move.end.x + move.end.y * this->rotate_Board.SIZE, transformation);
				mover.end.x = index % this->rotate_Board.SIZE;
				mover.end.y = std::floor(index / this->rotate_Board.SIZE);
			}
			else {
				mover.type = Placement;
				mover.piece = move.piece;
				
				int index = get_new_index(move.location.x + move.location.y * this->rotate_Board.SIZE, transformation);
				mover.location.x = index % this->rotate_Board.SIZE;
				mover.location.y = std::floor(index / this->rotate_Board.SIZE);
			}

			//Get new index
			int new_index = this->rotate_Board.GetPlayIndex(mover);

			//Save to new index
			output.probabilities[new_index] = input.probabilities[i];
		}
	}

	//Copy old
	output.net_winrate = input.net_winrate;

	return output;
}
void Training::setFolderName(std::string foldername){
	this->foldername = foldername;
}

int Training::save_game(std::string filename, float winner)
{
	const size_t size = this->m_data.size();
	std::cout << "Saving " << size << " boards to " << filename << std::endl;

	std::experimental::filesystem::path myDirectory = this->foldername;
	if (!std::experimental::filesystem::is_directory(this->foldername)) {
		std::experimental::filesystem::create_directory(this->foldername);
	}

	std::string location = this->foldername + SLASH + filename;
	H5::H5File file(location.c_str(), H5F_ACC_TRUNC);

	NNInput *planes = new NNInput[size];
	PlayProb *probs = new PlayProb[size];
	float *win_rates = new float[size];

	for (size_t i = 0; i < size; i++)
	{
		for(size_t j = 0; j < 8; j++){
			std::memcpy(planes[i][j].data(), this->m_data[i].planes[j].data(),sizeof(std::uint8_t)* 5 * 5 * 32);
		}
		std::memcpy(probs[i].data(), this->m_data[i].probabilities.data(), sizeof(float) * 1575);
		win_rates[i] = winner;
	}
	//Planes input
	hsize_t     x_input[3] = { size,8, 5 * 5 * 32 };

	H5::DataSpace x_dataspace(3, x_input);

	H5::DSetCreatPropList  x_plist = H5::DSetCreatPropList();
	x_plist.setChunk(3, x_input);
	x_plist.setDeflate(9);

	H5::IntType x_datatype(H5::PredType::NATIVE_INT8);
	x_datatype.setOrder(H5T_ORDER_LE);

	H5::DataSet x_dataset = file.createDataSet("x_input", x_datatype, x_dataspace, x_plist);

	x_dataset.write(planes, H5::PredType::NATIVE_INT8);

	//probs output
	hsize_t     y_input[2] = { size,1575 };

	H5::DataSpace y_dataspace(2, y_input);

	H5::DSetCreatPropList  y_plist = H5::DSetCreatPropList();
	y_plist.setChunk(2, y_input);
	y_plist.setDeflate(9);

	H5::IntType y_datatype(H5::PredType::NATIVE_FLOAT);
	y_datatype.setOrder(H5T_ORDER_LE);

	H5::DataSet y_dataset = file.createDataSet("y_input_1", y_datatype, y_dataspace, y_plist);

	y_dataset.write(probs, H5::PredType::NATIVE_FLOAT);

	//winrate output
	hsize_t     y_input_1[1] = { size };

	H5::DataSpace y_dataspace_1(1, y_input_1);

	H5::DSetCreatPropList  y_plist_1 = H5::DSetCreatPropList();
	y_plist_1.setChunk(1, y_input_1);
	y_plist_1.setDeflate(9);

	H5::IntType y_datatype_1(H5::PredType::NATIVE_FLOAT);
	y_datatype_1.setOrder(H5T_ORDER_LE);

	H5::DataSet y_dataset_1 = file.createDataSet("y_input_2", y_datatype_1, y_dataspace_1, y_plist_1);

	y_dataset_1.write(win_rates, H5::PredType::NATIVE_FLOAT);

	file.close();

	m_data.clear();

	delete[] planes;
	delete[] probs;
	delete[] win_rates;
	return 1;
}

int Training::uploadGame(std::string filename)
{
	CURLcode res;
	CURL *curl;
	struct curl_httppost *post1;
	struct curl_httppost *postend;

	//Open file
	std::string location = this->foldername + SLASH + filename;
	//std::ifstream input(location, std::ios::binary);
	// copies all data into buffer
	//std::vector<char> buffer((
	//	std::istreambuf_iterator<char>(input)),
	//4	(std::istreambuf_iterator<char>()));

	post1 = NULL;
	postend = NULL;
	curl_formadd(&post1, &postend,
		CURLFORM_COPYNAME, "game",
		CURLFORM_FILE, location.c_str(),
		CURLFORM_CONTENTTYPE, "application/octet-stream",
		CURLFORM_END);
	curl_formadd(&post1, &postend,
		CURLFORM_COPYNAME, "network",
		CURLFORM_COPYCONTENTS, this->foldername,
		CURLFORM_END);


	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://zero.generalzero.org/submit_game");
		curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post1);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.47.0");
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
		curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		curl = NULL;
		curl_formfree(post1);
		post1 = NULL;

		/* Check for errors */
		if (res != CURLE_OK){
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
			return 0;
		}
		else {
			// If uploaded file delete file
			std::experimental::filesystem::remove(location);
		}
		return 1;
	}
	return 0;
}
