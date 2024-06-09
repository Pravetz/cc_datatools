#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#define EMSS_LOG_SUCCESS(...)\
	std::cout<<"[^] "<<__VA_ARGS__<<std::endl;
#define EMSS_LOG_FAILURE(...)\
	std::cout<<"[x] "<<__VA_ARGS__<<std::endl;
#define EMSS_LOG_INFO(...)\
	std::cout<<"[i] "<<__VA_ARGS__<<std::endl;

bool contains_keywords(const std::string &str, const std::vector<std::string> &keyword_list){
	return !str.empty() && std::find_if(keyword_list.begin(), keyword_list.end(), [&](const std::string &keywordstr){ return str.find(keywordstr) != std::string::npos; }) != keyword_list.end();
}

bool is_filtered(const std::string &str, const std::vector<std::string> &filter_list){
	return !str.empty() && std::find_if(filter_list.begin(), filter_list.end(), [&](const std::string &filterstr){ return str.find(filterstr) != std::string::npos; }) != filter_list.end();
}

uint8_t get_lines(const std::string &input_path, const std::string &output_path, const std::vector<std::string> &filter_list, const std::vector<std::string> &keyword_list){
	if(input_path.empty() || output_path.empty() || input_path == output_path){
		return 1;
	}
	std::ifstream ifile;
	std::ofstream ofile;
	
	ifile.open(input_path.c_str());
	if(!ifile){
		return 2;
	}
	
	std::string accum = "";
	std::string line = "";
	
	while(std::getline(ifile, line)){
		if(line != "!"){
			accum += line + "\n";
		}
		else{
			if((!is_filtered(accum, filter_list) && keyword_list.empty()) || contains_keywords(accum, keyword_list)){
				size_t hash = std::hash<std::string>{}(accum);
				std::stringstream ss;
				ss << std::hex << hash;
				std::string fname = ss.str();
				ofile.open(output_path + fname + ".txt");
				ofile << accum;
				ofile.close();
			}
			accum = "";
		}
	}
	
	return 0;
}

int main(int argc, char **argv){
	if(argc == 1){
		std::cout<<"Usage: "<<argv[0]<<" <parameters>\nPossible parameters:\n-i <path> = give input file path\n-o = give output file path\n-f <filtered words> -f = filter out strings containing given keywords\n-k <keywords> -k = only extract strings containing given keywords."<<std::endl;
		return EXIT_SUCCESS;
	}
	std::string input_path = "";
	std::string output_path = "";
	std::vector<std::string> filter_list;
	std::vector<std::string> kwd_list;
	bool filtering = false;
	bool picking_kwds = false;
	for(size_t i = 0; i < (size_t)argc; i++){
		if(!strcmp(argv[i], "-i")){
			input_path = std::string(argv[++i]);
		}
		else if(!strcmp(argv[i], "-o")){
			output_path = std::string(argv[++i]);
		}
		else if(!strcmp(argv[i], "-f")){
			filtering = !filtering;
		}
		else if(!strcmp(argv[i], "-k")){
			picking_kwds = !picking_kwds;
		}
		else if(filtering){
			EMSS_LOG_INFO("Filter "<<argv[i])
			filter_list.push_back(std::string(argv[i]));
		}
		else if(picking_kwds){
			EMSS_LOG_INFO("Keyword "<<argv[i])
			kwd_list.push_back(std::string(argv[i]));
		}
	}
	
	if(!get_lines(input_path, output_path, filter_list, kwd_list)){
		EMSS_LOG_SUCCESS("Extracted strings from \'"<<input_path<<"\' to \'"<<output_path<<"\'")
	}
	else{
		EMSS_LOG_FAILURE("Failed to extract strings from \'"<<input_path<<"\' to \'"<<output_path<<"\'")
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
