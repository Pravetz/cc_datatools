#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

#define CSVCE_LOG_SUCCESS(...)\
	std::cout<<"[^] "<<__VA_ARGS__<<std::endl;
#define CSVCE_LOG_FAILURE(...)\
	std::cout<<"[x] "<<__VA_ARGS__<<std::endl;
#define CSVCE_LOG_INFO(...)\
	std::cout<<"[i] "<<__VA_ARGS__<<std::endl;

struct CSVCE_STATE {
	std::string endl_replacement = "";
	std::string input_fpath = "";
	std::string output_fpath = "";
	size_t column = 0;
};

/*
 * RETURN CODES:
 * 0 = ALL OK
 * 1 = ERROR: INPUT AND OUTPUT PATHS ARE EQUAL
 * 2 = ERROR: FAILED TO OPEN [I/O]-FILE
 * 3 = NONEXISTENT COLUMN
 * 
*/
uint8_t extract_column(const CSVCE_STATE &state){
	if(state.input_fpath == state.output_fpath || state.input_fpath.empty() || state.output_fpath.empty()){
		return 1;
	}
	
	bool is_str_sequence = false;
	std::ifstream ifile;
	std::ofstream ofile;
	ifile.open(state.input_fpath.c_str(), std::fstream::binary);
	ofile.open(state.output_fpath.c_str(), std::fstream::binary);
	if(!ifile || !ofile){
		return 2;
	}
	size_t ccolumn = 0;
	
	char byte;
	while(ifile.read(&byte, sizeof(byte))){
		switch(byte){
			case '\"':
				ifile.read(&byte, sizeof(byte));
				if(byte != '\"'){
					is_str_sequence = !is_str_sequence;
				}
				if(ccolumn == state.column && (byte != ',' || is_str_sequence)){
					ofile.write(&byte, sizeof(byte));
				}
				else if(byte == ',' || !is_str_sequence){
					ccolumn++;
				}
			break;
			case ',':
				if(!is_str_sequence){
					ccolumn++;
				}
				else if(ccolumn == state.column){
					ofile.write(&byte, sizeof(byte));
				}
			break;
			case '\n':
				if(!is_str_sequence){
					if(state.column > ccolumn){
						return 3;
					}
					ccolumn = 0;
					ofile.write(&byte, sizeof(byte));
				}
				else if(ccolumn == state.column){
					ofile.write(state.endl_replacement.c_str(), state.endl_replacement.size());
				}
			break;
			default:
				if(ccolumn == state.column){
					ofile.write(&byte, sizeof(byte));
				}
			break;
		}
	}
	
	return 0;
}

int main(int argc, char **argv) {
	if(argc == 1){
		std::cout<<"Usage: "<<argv[0]<<" <parameters>\n\n\t\tPossible parameters:\n\t\t-i = input file path\n\t\t-o = output file path\n\t\t-c = desired state.column(1-based).\n\t\t-noendl = discard(or replace, optionally) every occurence of \'\\n\' character in strings."<<std::endl;
		return EXIT_SUCCESS;
	}
	CSVCE_STATE state = { "\n", "", "", 0 };
	
	for(size_t i = 1; i < (size_t)argc; i++){
		if(!strcmp(argv[i], "-i")){
			state.input_fpath = std::string(argv[++i]);
		}
		if(!strcmp(argv[i], "-o")){
			state.output_fpath = std::string(argv[++i]);
		}
		if(!strcmp(argv[i], "-c")){
			state.column = (size_t)(atoll(argv[++i]) - 1);
		}
		if(!strcmp(argv[i], "-noendl")){
			if(i + 1 < (size_t)argc && argv[i + 1][0] != '-'){
				state.endl_replacement = std::string(argv[++i]);
			}
			else{
				CSVCE_LOG_INFO("Every \'\\n\' character in strings will be discarded.");
				state.endl_replacement = "";
			}
		}
	}
	if(!extract_column(state)){
		CSVCE_LOG_SUCCESS("Successfully extracted data in column #"<<state.column + 1<<" from file \'"<<state.input_fpath<<"\' to file \'"<<state.output_fpath<<"\'.")
	}
	else{
		CSVCE_LOG_FAILURE("Failed to extract data from file \'"<<state.input_fpath<<"\' to file \'"<<state.output_fpath<<"\'.");
	}
	
	return EXIT_SUCCESS;
}
