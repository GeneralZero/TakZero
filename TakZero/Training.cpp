#include "Training.h"
#include "FakeNetwork.h"

#include "MD5.h"

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

	auto data_md5 = MD5::MD5((char*)this->m_data.data(), this->m_data.size() * sizeof(TimeStep));
	std::string file_name = data_md5.hexdigest();

	//Save Game
	save_game(directory, file_name);

	//Upload Game
	uploadGame(directory, file_name);
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
	int i = 0;
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

void Training::save_game(std::string foldername, std::string file_name)
{
	H5::H5File file(foldername + "\\" + file_name, H5F_ACC_TRUNC);

	std::vector<NNInput> planes;

	std::vector<PlayProb> probs;
	std::vector<double> win_rates;


	for (size_t i = 0; i < this->m_data.size(); i++)
	{
		planes.push_back(this->m_data[i].planes);
		probs.push_back(this->m_data[i].probabilities);
		win_rates.push_back(this->m_data[i].net_winrate);
	}

	//Planes input
	hsize_t     x_input[3] = {planes.size(),8, 5*5*32};
	
	H5::DataSpace x_dataspace(3, x_input);

	H5::DSetCreatPropList  plist = H5::DSetCreatPropList();
	plist.setDeflate(9);

	H5::IntType x_datatype(H5::PredType::NATIVE_INT8);
	x_datatype.setOrder(H5T_ORDER_LE);

	H5::DataSet x_dataset = file.createDataSet("x_input", x_datatype, x_dataspace, plist);

	x_dataset.write(planes.data(), H5::PredType::NATIVE_INT8);

	//probs output
	hsize_t     y_input[2] = { probs.size(),1575 };

	H5::DataSpace y_dataspace(2, y_input);

	H5::IntType y_datatype(H5::PredType::NATIVE_DOUBLE);
	y_datatype.setOrder(H5T_ORDER_LE);

	H5::DataSet y_dataset = file.createDataSet("y_input_1", y_datatype, y_dataspace, plist);

	y_dataset.write(probs.data(), H5::PredType::NATIVE_DOUBLE);

	//winrate output
	hsize_t     y_input_1[1] = { win_rates.size()};

	H5::DataSpace y_dataspace_1(1, y_input_1);

	H5::IntType y_datatype_1(H5::PredType::NATIVE_DOUBLE);
	y_datatype_1.setOrder(H5T_ORDER_LE);

	H5::DataSet y_dataset_1 = file.createDataSet("y_input_2", y_datatype_1, y_dataspace_1, plist);

	y_dataset_1.write(win_rates.data(), H5::PredType::NATIVE_DOUBLE);

	//Close
	file.close();
}

int Training::uploadGame(std::string foldername, std::string filename)
{
	return 0;
}
