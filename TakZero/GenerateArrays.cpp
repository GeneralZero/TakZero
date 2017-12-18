
#include <vector>
#include <array>

const std::array<uint8_t, 4> distance_0_cap[] = { std::array<uint8_t, 4>({1}) };
const std::array<uint8_t, 4> distance_0[] = { std::array<uint8_t, 4>() };

const std::array<uint8_t, 4> distance_1_cap_2[] = { std::array<uint8_t, 4>({ 1 }),std::array<uint8_t, 4>({ 2 }),std::array<uint8_t, 4>({ 1,1 }) };
const std::array<uint8_t, 4> distance_1_2[] = { std::array<uint8_t, 4>({ 1 }),std::array<uint8_t, 4>({ 2 })};
const std::array<uint8_t, 4> distance_1_cap_3[] = { std::array<uint8_t, 4>({ 1 }),std::array<uint8_t, 4>({ 2 }),std::array<uint8_t, 4>({ 3 }),std::array<uint8_t, 4>({ 1,1 }),std::array<uint8_t, 4>({ 2,1 }) };
const std::array<uint8_t, 4> distance_1_3[] = { std::array<uint8_t, 4>({ 1 }),std::array<uint8_t, 4>({ 2 }), std::array<uint8_t, 4>({ 3 }) };
const std::array<uint8_t, 4> distance_1_cap_4[] = { std::array<uint8_t, 4>({ 1 }),std::array<uint8_t, 4>({ 2 }),std::array<uint8_t, 4>({ 3 }),std::array<uint8_t, 4>({ 4 }),std::array<uint8_t, 4>({ 1,1 }),std::array<uint8_t, 4>({ 2,1 }),std::array<uint8_t, 4>({ 3,1 }) };
const std::array<uint8_t, 4> distance_1_4[] = { std::array<uint8_t, 4>({ 1 }),std::array<uint8_t, 4>({ 2 }), std::array<uint8_t, 4>({ 3 }), std::array<uint8_t, 4>({ 4 }) };
const std::array<uint8_t, 4> distance_1_cap_5[] = { std::array<uint8_t, 4>({ 1 }),std::array<uint8_t, 4>({ 2 }),std::array<uint8_t, 4>({ 3 }),std::array<uint8_t, 4>({ 4 }), std::array<uint8_t, 4>({ 5 }),std::array<uint8_t, 4>({ 1,1 }),std::array<uint8_t, 4>({ 2,1 }),std::array<uint8_t, 4>({ 3,1 }),std::array<uint8_t, 4>({ 4,1 }) };
const std::array<uint8_t, 4> distance_1_5[] = { std::array<uint8_t, 4>({ 1 }),std::array<uint8_t, 4>({ 2 }), std::array<uint8_t, 4>({ 3 }), std::array<uint8_t, 4>({ 4 }), std::array<uint8_t, 4>({ 5 }) };

const std::array<uint8_t, 4> distance_2_cap_3[] = { std::array<uint8_t, 4>({ 1 }),std::array<uint8_t, 4>({ 2 }),std::array<uint8_t, 4>({ 3 }),std::array<uint8_t, 4>({ 1,1 }),std::array<uint8_t, 4>({ 2,1 }),std::array<uint8_t, 4>({ 1,2 }), std::array<uint8_t, 4>({ 1,1,1 }) };
const std::array<uint8_t, 4> distance_2_3[] = { std::array<uint8_t, 4>({ 1 }),std::array<uint8_t, 4>({ 2 }),std::array<uint8_t, 4>({ 3 }),std::array<uint8_t, 4>({ 1,1 }),std::array<uint8_t, 4>({ 2,1 }),std::array<uint8_t, 4>({ 1,2 }) };



const std::array<uint8_t, 4>* Board::GenerateMoveArrays(uint8_t distance, uint8_t to_move, bool cap) {
	if (distance == 0) {
		if (cap) {
			return distance_0_cap;
		}
		return distance_0;
	}

	if (to_move == 0) {
		return distance_0;
	}

	if (to_move == 1) {
		return distance_0_cap;
	}

	if (distance == 1) {
		switch (to_move)
		{
		case 2:
			if (cap) {
				return distance_1_cap_2;
			}
			return distance_1_2;
		case 3:
			if (cap) {
				return distance_1_cap_3;
			}
			return distance_1_3;

		case 4:
			if (cap) {
				return distance_1_cap_4;
			}
			return distance_1_4;

		default:
			if (cap) {
				return distance_1_cap_5;
			}
			return distance_1_5;
		}
	}

	if (to_move == 2) {
		return distance_1_cap_2;
	}

	if (distance == 2) {
		switch (to_move)
		{
		case 3:
			if (cap) {
				return distance_2_cap_3;
			}
			return distance_2_3;

		case 4:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 1,2 }, { 1,3 },
				{ 2, 1 }, { 2,2 }, { 3, 1 }, { 1,1,1 }, { 1,2,1 }, { 2,1,1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 1,2 }, { 1,3 }, { 2, 1 }, { 2,2 }, { 3, 1 }};

		default:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1,2 }, { 1,3 }, { 1,4 },
				{ 2, 1 }, { 2,2 }, { 2,3 }, { 3, 1 }, { 3,2 }, { 4,1 }, { 1, 1, 1 }, { 1,1,2 }, { 1,1,3 }, { 1, 2, 1 }, { 1,2,2 },
				{ 1,3,1 }, { 2, 1, 1 }, { 2,1,2 }, { 2,2,1 }, { 3,1,1 }, { 1,1,1,1 }, { 1,1,2,1 }, { 1,2,1,1 }, { 2,1,1,1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1,2 }, { 1,3 }, { 1,4 },
			{ 2, 1 }, { 2,2 }, { 2,3 }, { 3, 1 }, { 3,2 }, { 4,1 }, { 1, 1, 1 }, { 1,1,2 }, { 1,1,3 }, { 1, 2, 1 }, { 1,2,2 },
			{ 1,3,1 }, { 2, 1, 1 }, { 2,1,2 }, { 2,2,1 }, { 3,1,1 }};
		}
	}

	if (to_move == 3) {
		return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 1, 1 }, { 1,2 }, { 2,1 }, { 1,1,1 }};
	}

	if (distance == 3) {
		switch (to_move)
		{
		case 4:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 2, 1 }, { 2, 2 }, { 3, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 2, 1 }, { 2, 1, 1 }, { 1, 1, 1, 1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 2, 1 }, { 2, 2 }, { 3, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 2, 1 }, { 2, 1, 1 }};

		default:
			if (cap) {
				return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 }, { 3, 2 }, { 4, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 1, 3 }, { 1, 2, 1 }, { 1, 2, 2 }, { 1, 3, 1 }, { 2, 1, 1 }, { 2, 1, 2 }, { 2, 2, 1 }, { 3, 1, 1 }, { 1, 1, 1, 1 }, { 1, 1, 2, 1 }, { 1, 2, 1, 1 }, { 2, 1, 1, 1 }};
			}
			return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 }, { 3, 2 }, { 4, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 1, 3 }, { 1, 2, 1 }, { 1, 2, 2 }, { 1, 3, 1 }, { 2, 1, 1 }, { 2, 1, 2 }, { 2, 2, 1 }, { 3, 1, 1 }};
		}
	}

	if (to_move == 4) {
		return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 2, 1 }, { 2, 2 }, { 3, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 2, 1 }, { 2, 1, 1 }, { 1, 1, 1, 1 }};
	}

	else {
		return std::vector<std::vector<uint8_t>>{ {1}, { 2 }, { 3 }, { 4 }, { 5 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 }, { 3, 2 }, { 4, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 1, 3 }, { 1, 2, 1 }, { 1, 2, 2 }, { 1, 3, 1 }, { 2, 1, 1 }, { 2, 1, 2 }, { 2, 2, 1 }, { 3, 1, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 2 }, { 1, 1, 2, 1 }, { 1, 2, 1, 1 }, { 2, 1, 1, 1 }};
	}
}
