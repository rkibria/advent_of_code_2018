#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>
#include <array>
#include <algorithm>

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
		eqrr,

		nop
	};

	static constexpr int num_opcodes() {
		return static_cast<int>(Opcode::nop);
	}

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

constexpr auto opcode_to_int(Device::Opcode opcode) {return static_cast<int>(opcode);}

auto& operator<<(std::ostream& os, const Device::Opcode& opcode) {
	os << "op_" << opcode_to_int(opcode);
	return os;
}

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


using OpcodeVector = std::vector<Device::Opcode>;

constexpr auto NO_ENCODING = Device::num_opcodes();

class OpcodeMap {
private:
	std::array<Device::Opcode, Device::num_opcodes()> opcodes;
	std::array<int, Device::num_opcodes()> encodings;
	int mapped_count = 0;

public:
	OpcodeMap() {
		std::fill(opcodes.begin(), opcodes.end(), Device::Opcode::nop);
		std::fill(encodings.begin(), encodings.end(), NO_ENCODING);
	}

	auto num_mapped() const {return mapped_count;}

	Device::Opcode get(int encoding) const {return opcodes[encoding];}
	auto get(Device::Opcode opcode) const {return encodings[opcode_to_int(opcode)];}

	bool is_encoding_mapped(int encoding) {return opcodes[encoding] != Device::Opcode::nop;}
	bool is_opcode_mapped(Device::Opcode opcode) {return encodings[opcode_to_int(opcode)] != NO_ENCODING;}

	void map_encoding_to_opcode(int encoding, Device::Opcode opcode) {
		assert(encoding >=0 && encoding < Device::num_opcodes());
		if(is_encoding_mapped(encoding))
			throw std::runtime_error("Opcode already mapped");

		++mapped_count;
		opcodes[encoding] = opcode;
		encodings[opcode_to_int(opcode)] = encoding;
	}

	static OpcodeVector get_matching_opcodes(const Sample& smpl);
};

OpcodeVector OpcodeMap::get_matching_opcodes(const Sample& smpl) {
	OpcodeVector ops;
	Device dev;
	for(auto i = 0; i < 16; ++i) {
		dev.get_regs() = smpl.regs_before;
		auto inst = smpl.inst;
		inst.op = static_cast<Device::Opcode>(i);
		dev.execute(inst);
		if(dev.get_regs() == smpl.regs_after)
			ops.push_back(inst.op);
	}
	return ops;
}

void part_1(const SampleVector& smpl_vec) {
	auto three_or_more_opcodes = 0;
	for(const auto& smpl : smpl_vec) {
		const auto matches = OpcodeMap::get_matching_opcodes(smpl);
		if(matches.size() >= 3)
			++three_or_more_opcodes;
	}
	std::cout << three_or_more_opcodes << std::endl;
}

OpcodeMap map_encodings(const SampleVector& smpl_vec) {
	OpcodeMap opmap;
	do {
		std::vector<std::vector<Device::Opcode>> possible_maps(Device::num_opcodes());
		for(const auto& smpl : smpl_vec) {
			auto matches = OpcodeMap::get_matching_opcodes(smpl);
			matches.erase(std::remove_if(matches.begin(), matches.end(), [&opmap](auto opcode) {return opmap.is_opcode_mapped(opcode);}), matches.end());
			if(matches.size() == 1) {
				const auto encoding = opcode_to_int(smpl.inst.op);
				auto& ops = possible_maps[encoding];
				const auto opcode = matches[0];
				if(std::find(ops.begin(), ops.end(), opcode) == ops.end())
					ops.push_back(opcode);
			}
		}
		for(size_t i=0; i < possible_maps.size(); ++i) {
			if(possible_maps[i].size() == 1) {
				opmap.map_encoding_to_opcode(i, possible_maps[i].front());
			}
		}
	} while(opmap.num_mapped() < Device::num_opcodes());
	return opmap;
}

void part_2(const SampleVector& smpl_vec) {
	auto opmap = map_encodings(smpl_vec);
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
		return -1;
	}

	const auto smpl_vec = load(argv[1]);
	std::clog << "num samples: " << smpl_vec.size() << std::endl;

	// part_1(smpl_vec);
	part_2(smpl_vec);
}
