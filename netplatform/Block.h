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


/*+-+-+-+-+-+-+-+-+-+-+-+-+-*/
using json = nlohmann::json; ///
using std::cout;             ///
using std::cin;              ///
using std::endl;             ///
/*+-+-+-+-+-+-+-+-+-+-+-+-+-*/

constexpr uint32_t difficulty = 4;

class Block
{

public:
	Block() = default;

	inline const uint64_t getIndex()                        const noexcept { return index; }
	inline const time_t getTimestamp()						const noexcept { return timestamp; }
	inline const std::string getHash()					    const noexcept { return hash; }
	inline const std::string getPrevHash()				    const noexcept { return prevHash; }
	inline const std::vector<Transaction> getTransaction()  const noexcept { return transaction; }

	
	void addTransaction(const Transaction& transaction) { this->transaction.push_back(transaction); }

	inline const std::string calculateHash() const noexcept
	{
		std::string hashtmp;
		for (const auto& tx : transaction)
		{
			hashtmp += tx.getFrom() + tx.getTo() + std::to_string(tx.getAmount());
		}

		hashtmp = std::to_string(index) + std::to_string(timestamp) + prevHash;
		std::string hash = sha512(hashtmp);
		return hash;
	}

private:
	uint64_t index{ 0 };
	time_t timestamp{ NULL };
	std::string hash{ "" };
	std::string prevHash{ "" };
	std::string nonce{ "" };
	std::vector<Transaction> transaction;



};
 
