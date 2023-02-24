#pragma once
#include "Transaction.h"
#include <xutility>

#include <string>
#include <cstdint> 
#include <nlohmann/json.hpp>
#include <vector>
#include <iostream> 

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>



//////////////////////////////
using json = nlohmann::json;//
using std::cout;            //
using std::cin;             //
using std::endl;            //
//////////////////////////////



class Block
{

public:
	Block() = default;
	Block() { }


private:
	uint64_t index{ 0 };
	time_t timestamp{ NULL };
	std::string hash{ "" };
	std::string prevHash{ "" };
	std::string nonce{ "" };
	std::vector<Transaction> transaction;



};




 



