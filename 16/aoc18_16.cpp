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

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
		return -1;
	}

	// PART 1
	std::ifstream file(argv[1]);
	if(file.is_open())
	{
		std::string line;

		while(getline(file, line))
		{
			if(line.empty())
				continue;

			std::istringstream iss(line);

			std::string token;
			iss >> token;
			if(token != "Before:")
				break;

			std::clog << line << std::endl;


			Device::RegArray regs_before;

			iss >> token;
			token.pop_back();
			token.erase(0, 1);
			regs_before[0] = stoi(token);

			iss >> token;
			token.pop_back();
			regs_before[1] = stoi(token);

			iss >> token;
			token.pop_back();
			regs_before[2] = stoi(token);

			iss >> token;
			token.pop_back();
			regs_before[3] = stoi(token);


			Device::Instr inst;

			getline(file, line);
			iss.clear();
			iss.str(line);

			iss >> token;
			Device::RegType opcode = stoi(token);

			iss >> token;
			inst.a = stoi(token);

			iss >> token;
			inst.b = stoi(token);

			iss >> token;
			inst.c = stoi(token);


			getline(file, line);
			iss.clear();
			iss.str(line);

			Device::RegArray regs_after;

			iss >> token;
			assert(token == "After:");

			iss >> token;
			token.pop_back();
			token.erase(0, 1);
			regs_after[0] = stoi(token);

			iss >> token;
			token.pop_back();
			regs_after[1] = stoi(token);

			iss >> token;
			token.pop_back();
			regs_after[2] = stoi(token);

			iss >> token;
			token.pop_back();
			regs_after[3] = stoi(token);

		}
		file.close();
	}


}
