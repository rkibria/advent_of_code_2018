#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>
#include <array>

class Device {
public:
	Device() {};

	enum class Opcode {
		addr,
		addi,

		mulr,
		muli,

		banr,
		bani,

		borr,
		bori,

		setr,
		seti,

		gtir,
		gtri,
		gtrr,

		eqir,
		eqri,
		eqrr
	};

	using RegType = int;

	using RegArray = std::array<RegType, 4>;

	struct Instr {
		Opcode op;
		RegType a, b, c;
	};

	void execute(const Instr& inst);

	const RegType& 	get(size_t reg_i) const {return regs[reg_i];}
	RegType& 		get(size_t reg_i) {return regs[reg_i];}

	RegArray& get_regs() {return regs;}

private:
	RegArray regs;
};

void Device::execute(const Instr& inst) {
	assert(inst.c >= 0 && inst.c <= 3);

	switch(inst.op) {
	case Opcode::addr: regs[inst.c] = regs[inst.a] + regs[inst.b]; break;
	case Opcode::addi: regs[inst.c] = regs[inst.a] + inst.b; break;

	case Opcode::mulr: regs[inst.c] = regs[inst.a] * regs[inst.b]; break;
	case Opcode::muli: regs[inst.c] = regs[inst.a] * inst.b; break;

	case Opcode::banr: regs[inst.c] = regs[inst.a] & regs[inst.b]; break;
	case Opcode::bani: regs[inst.c] = regs[inst.a] & inst.b; break;

	case Opcode::borr: regs[inst.c] = regs[inst.a] | regs[inst.b]; break;
	case Opcode::bori: regs[inst.c] = regs[inst.a] | inst.b; break;

	case Opcode::setr: regs[inst.c] = regs[inst.a]; break;
	case Opcode::seti: regs[inst.c] = inst.a; break;

	case Opcode::gtir: regs[inst.c] = (inst.a > regs[inst.b]) ? 1 : 0; break;
	case Opcode::gtri: regs[inst.c] = (regs[inst.a] > inst.b) ? 1 : 0; break;
	case Opcode::gtrr: regs[inst.c] = (regs[inst.a] > regs[inst.b]) ? 1 : 0; break;

	case Opcode::eqir: regs[inst.c] = (inst.a == regs[inst.b]) ? 1 : 0; break;
	case Opcode::eqri: regs[inst.c] = (regs[inst.a] == inst.b) ? 1 : 0; break;
	case Opcode::eqrr: regs[inst.c] = (regs[inst.a] == regs[inst.b]) ? 1 : 0; break;

	default: break;
	}
}

struct Sample {
	Device::RegArray regs_before;
	Device::Instr inst;
	Device::RegArray regs_after;
};

using SampleVector = std::vector<Sample>;

SampleVector load(const char* filename) {
	SampleVector smpl_vec;

	std::ifstream file(filename);
	if(file.is_open())
	{
		std::string line;
		while(getline(file, line))
		{
			if(line.empty())
				continue;

			Sample smpl;

			std::istringstream iss(line);

			std::string token;
			iss >> token;
			if(token != "Before:")
				break;

			for(auto i = 0; i <= 3; ++i) {
				iss >> token;
				token.pop_back();
				if(i == 0)
					token.erase(0, 1);
				smpl.regs_before[i] = stoi(token);
			}


			getline(file, line);
			iss.clear();
			iss.str(line);

			iss >> token;
			smpl.inst.op = static_cast<Device::Opcode>(stoi(token));

			iss >> token;
			smpl.inst.a = stoi(token);

			iss >> token;
			smpl.inst.b = stoi(token);

			iss >> token;
			smpl.inst.c = stoi(token);


			getline(file, line);
			iss.clear();
			iss.str(line);

			iss >> token;
			assert(token == "After:");

			for(auto i = 0; i <= 3; ++i) {
				iss >> token;
				token.pop_back();
				if(i == 0)
					token.erase(0, 1);
				smpl.regs_after[i] = stoi(token);
			}

			smpl_vec.push_back(smpl);
		}
		file.close();
	}

	return smpl_vec;
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
		return -1;
	}

	const auto smpl_vec = load(argv[1]);
	std::clog << "num samples: " << smpl_vec.size() << std::endl;

	// PART 1
	auto three_or_more_opcodes = 0;
	for(const auto& smpl : smpl_vec) {
		Device dev;
		auto matches = 0;
		for(auto i = 0; i < 16; ++i) {
			dev.get_regs() = smpl.regs_before;
			auto inst = smpl.inst;
			inst.op = static_cast<Device::Opcode>(i);
			dev.execute(inst);
			if(dev.get_regs() == smpl.regs_after)
				++matches;
		}
		if(matches >= 3)
			++three_or_more_opcodes;
	}
	std::cout << three_or_more_opcodes << std::endl;
}
