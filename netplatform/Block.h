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

#include "sha512.h"
#include <memory>


#include <chrono>
#include <cstdlib>
#include <ctime>
 
#include <memory>
#include <string>

#include <random>

 
#define MAX_BLOCK_SIZE 2
 


 

/*+-+-+-+-+-+-+-+-+-+-+-+-+-*/
using json = nlohmann::json; ///
using std::cout;             ///
using std::cin;              ///
using std::endl;             ///
/*+-+-+-+-+-+-+-+-+-+-+-+-+-*/

constexpr int difficulty{ 0 }; // block-mining difficulty
constexpr double reward{ 20.00070 };

class Block
{

public:
	Block() = default;
	Block(int index) : index(index), timestamp(std::time(nullptr))
	{
		hash = calculateHash();
	}

	inline const uint64_t getIndex() const noexcept { return index; }
	inline const time_t getTimestamp() const noexcept { return timestamp; }
	inline const std::string getHash() const noexcept { return hash; }
	inline const std::string getPrevHash() const noexcept { return prevHash; }
	inline const std::vector<Transaction> getTransaction() const noexcept { return transaction; }
	
	// beta : calculating hash / @ynwqmv
	inline const std::string calculateHash() noexcept
	{
		std::string strData;

		for (const auto& tx : transaction)
		{
			strData += tx.getFrom() + tx.getRecipient() + std::to_string(tx.getAmount());
		}

		strData += prevHash;

		std::string hashPrefix = std::string(difficulty, '0'); // set the hash prefix to look for

		std::string result;
		unsigned long long nonce = 0;
		do {
			nonce++;
			std::string nonceStr = std::to_string(nonce);
			std::string dataWithNonce = strData + nonceStr;
			result = sha512(dataWithNonce);
		} while (result.substr(0, difficulty) != hashPrefix);

		this->nonce = std::to_string(nonce);
		this->hash = result;
		return result;
	}


	void setPrevHash(std::string ph) { this->prevHash = ph; }
	
	const bool isValid() const noexcept
	{
 
	}
bool getValidate(void) {
		  return true;
	}
private:
	friend class Blockchain;
	friend class http_connection;
	uint64_t index;
	time_t timestamp;
	
	std::string hash;
	std::string prevHash;
	std::string nonce;
	std::vector<Transaction> transaction;
	 
};