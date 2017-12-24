#include "Training.h"
#include "FakeNetwork.h"
#include <experimental/filesystem>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#else

#endif


#include "MD5.h"
#include <iostream>
#include "H5Cpp.h"

std::vector<TimeStep> Training::m_data{};

void Training::record(Board & state, UCTNode & root)
{
	auto step = TimeStep{};
	step.planes = NNInput{};

	auto result =
		FakeNetwork::get_scored_moves(&state);
	step.net_winrate = result.second;

	// Get total visit amount. We count rather
	// than trust the root to avoid ttable issues.
	uint64_t sum_visits = 0;
	for (size_t i = 0; i < root.possoble_moves.size(); i++)
	{
		sum_visits += root.possoble_moves[i]->get_visits();
	}

	if (sum_visits <= 0.0) {
		return;
	}

	for (size_t i = 0; i < root.possoble_moves.size(); i++) {
		auto prob = static_cast<float>(root.possoble_moves[i]->get_visits() / sum_visits);
		auto move = root.possoble_moves[i]->get_move();
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

void Training::dump_game()
{
	std::string directory = "Testing";

	//Check for directory

	//Create MD5 hash

	auto data_md5 = MD5((char*)this->m_data.data(), this->m_data.size() * sizeof(TimeStep));
	std::string file_name = "Game_" + data_md5.hexdigest() + ".hdf5";

	//Save Game
	save_game(directory, file_name);

	//Upload Game
	//uploadGame(directory, file_name);
}

int Training::get_new_index(uint8_t index, uint8_t transformation)
{
	int old_x = index % this->rotate_Board.SIZE;
	int old_y = floorl(index / this->rotate_Board.SIZE);
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

		return new_x + new_y * this->rotate_Board.SIZE;
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

FastBoard Training::rotateBoard(FastBoard board, uint8_t transformation)
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

TimeStep Training::transformTimeStep(TimeStep input, uint8_t transformation)
{
	TimeStep output;
	//Transform Boards
	uint i = 0;
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
			Play mover;

			//Rotate Move
			if (move.type == MoveStack) {
				mover.type = MoveStack;
				mover.order = move.order;

				int index = get_new_index(move.start.x + move.start.y * this->rotate_Board.SIZE, transformation);
				mover.start.x = index % this->rotate_Board.SIZE;
				mover.start.y = floorl(index / this->rotate_Board.SIZE);

				index = get_new_index(move.end.x + move.end.y * this->rotate_Board.SIZE, transformation);
				mover.end.x = index % this->rotate_Board.SIZE;
				mover.end.y = floorl(index / this->rotate_Board.SIZE);
			}
			else {
				mover.type = Placement;
				mover.piece = move.piece;
				
				int index = get_new_index(move.start.x + move.start.y * this->rotate_Board.SIZE, transformation);
				mover.location.x = index % this->rotate_Board.SIZE;
				mover.location.y = floorl(index / this->rotate_Board.SIZE);
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

int Training::save_game(std::string foldername, std::string file_name)
{
	const size_t size = this->m_data.size();

	std::experimental::filesystem::path myDirectory = foldername;
	if (!std::experimental::filesystem::is_directory(foldername)) {
		std::experimental::filesystem::create_directory(foldername);
		//_chdir(foldername.c_str());
	}

	//std::string location = foldername + "\\" + file_name;
	H5::H5File file(file_name.c_str(), H5F_ACC_TRUNC);

	NNInput *planes = new NNInput[size];
	PlayProb * probs = new PlayProb[size];
	double *win_rates = new double[size];

	for (size_t i = 0; i < size; i++)
	{
		std::memcpy(planes[i].data(), this->m_data[i].planes.data(), sizeof(NNInput));
		std::memcpy(probs[i].data(), this->m_data[i].probabilities.data(), sizeof(PlayProb));
		win_rates[i] = this->m_data[i].net_winrate;
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

	H5::IntType y_datatype(H5::PredType::NATIVE_DOUBLE);
	y_datatype.setOrder(H5T_ORDER_LE);

	H5::DataSet y_dataset = file.createDataSet("y_input_1", y_datatype, y_dataspace, y_plist);

	y_dataset.write(probs, H5::PredType::NATIVE_DOUBLE);

	//winrate output
	//const uint16_t a = win_rates.size();
	hsize_t     y_input_1[1] = { size };

	H5::DataSpace y_dataspace_1(1, y_input_1);

	H5::DSetCreatPropList  y_plist_1 = H5::DSetCreatPropList();
	y_plist_1.setChunk(1, y_input_1);
	y_plist_1.setDeflate(9);

	H5::IntType y_datatype_1(H5::PredType::NATIVE_DOUBLE);
	y_datatype_1.setOrder(H5T_ORDER_LE);

	H5::DataSet y_dataset_1 = file.createDataSet("y_input_2", y_datatype_1, y_dataspace_1, y_plist_1);

	y_dataset_1.write(win_rates, H5::PredType::NATIVE_DOUBLE);

	file.close();

	m_data.clear();
	return 1;
}

int Training::uploadGame(std::string foldername, std::string filename)
{
	return 0;
}
