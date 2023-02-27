#pragma once


#include "Block.h"
//#include "Network.h"

 

class Blockchain  
{

public:
	Blockchain()      /*   Genesis Block  */
		: blocks_{} { create_genesis_block(); }
	 
	const void addTransaction(const Transaction& tx) 
	{
		if (blocks_.empty() || blocks_.back().getTransaction().size() >= MAX_BLOCK_SIZE)
		{
			pendingTransaction.push_back(tx);
			spdlog::info("Transaction added to the pending transaction lists");
			
		}
		else
		{
			blocks_.back().transaction.push_back(tx);
			
		}
		
	}
	std::string Mine(Block& block)
	{
		spdlog::info("Mininig ... [ difficulty: {} ]", difficulty);
		std::string target(difficulty, '0');  // create target value
		std::string hash;
		std::string nonceStr;

		int nonce = 0;
		bool found = false;
		while (!found)
		{
			nonceStr = std::to_string(nonce);  // convert nonce to string
			block.nonce = nonceStr;  // set nonce of the block
			hash = block.calculateHash();  // calculate the hash of the block
			if (hash.substr(0, difficulty) == target)  // check if hash meets target difficulty
			{
				found = true;
			}
			else
			{
				nonce++;  // increment nonce and try again
			}
		}

		return hash;
	}


	// beta : 128 bit address generation (??? collisions ???)
	std::string generateAddress() {
		 
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, INT_MAX);
		std::string ss;
		
		for (int i = 0; i < 128; i++) {
			ss += dis(gen);
		}
		time_t current = std::time(nullptr);
		std::string hash = ss;
		std::string makeHash = sha512(hash + std::to_string(current));
		return makeHash;
	}
	// blockchain info : cmd / @lexndrr
	const void get_chain_info() const noexcept
	{
		spdlog::info("Difficulty: {}", difficulty);
		spdlog::info("Block Reward: {}", reward);
		spdlog::info("Max Block Size: {}", MAX_BLOCK_SIZE);
		spdlog::info("Encryption: SHA512");
	}
	void addBlock(Block& block)
	{
		if (blocks_.empty())
		{
			block.setPrevHash("");
		}
		else
		{
			block.setPrevHash(blocks_.back().getHash());
		}

		if (!pendingTransaction.empty())
		{
			std::vector<Transaction> transactions = block.transaction;
			transactions.insert(transactions.end(), pendingTransaction.begin(), pendingTransaction.end());
			block.transaction = transactions;
			pendingTransaction.clear();
		}
		else
		{

		}

		blocks_.push_back(block);
		spdlog::info("Block was added successfully!");
		print_chain();
	}


	Block getLastBlock() const noexcept
	{
		if (blocks_.empty())
		{
			spdlog::error("Couldn't get the last block");
			return Block{};
			 
		}
		return blocks_.back();
	}

	bool isValid() const noexcept
	{
		 
	}
	// Prints all blocks in blockchain : cmd / @lexndrr
	void print_chain() {
		spdlog::info("Printing blockchain:");
		std::cout << std::endl;
		for (auto block : blocks_) {
			std::cout << "*+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+*\n";
			spdlog::info("Block index: {}", block.index);
			
			spdlog::info("Block timestamp: {}", block.timestamp);
			spdlog::info("Block hash: {}", block.hash);
			spdlog::info("Previous block hash: {}", block.prevHash);
			spdlog::info("Difficulty: {}", difficulty);
			spdlog::info("Transactions:");
			for (auto transaction : block.transaction) {
				
				spdlog::info("Sender: {}", transaction.getFrom());
				spdlog::info("Recipient: {}", transaction.getRecipient());
				spdlog::info("Amount: {}", transaction.getAmount());
			}
			std::cout << "*+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+*\n";
		}
	}
	// prints last block from blockchain in JSON-format / @ynwqmv
	const json toJSON_last_block(Block& b) const noexcept
	{
		json j;
		
		j["index"] = b.index;
		j["timestamp"] = b.timestamp;
		j["hash"] = b.hash;
		j["prevHash"] = b.prevHash;
		j["nonce"] = b.nonce;
		for (const auto& i : b.getTransaction())
		{
			j["transactions"] = json::object();
			j["amount"] = i.getAmount();
			j["from"] = i.getFrom();
			j["recipient"] = i.getRecipient();
		}
		return j;
	}
	// prints all blocks in blockchain
	const json toJSON(const Blockchain& blockchain) const noexcept
	{
		json j;

		for (const auto& block : blockchain.blocks_)
		{
			json blockJson;
			blockJson["index"] = block.index;
			blockJson["timestamp"] = block.timestamp;
			blockJson["hash"] = block.hash;
			blockJson["prevHash"] = block.prevHash;
			blockJson["nonce"] = block.nonce;

			json transactionArray;
			for (const auto& transaction : block.getTransaction())
			{
				json transactionJson;
				transactionJson["from"] = transaction.getFrom();
				transactionJson["recipient"] = transaction.getRecipient();
				transactionJson["amount"] = transaction.getAmount();
				transactionArray.push_back(transactionJson);
			}
			blockJson["transactions"] = transactionArray;

			j.push_back(blockJson);
		}

		return j;
	}


private:
	std::vector<Block> blocks_;
	std::vector<Transaction> pendingTransaction;
	void create_genesis_block()
	{
		Block block;
		block.index =     0;
		block.timestamp = NULL;
		block.hash =	 "00000000000000000000000000000000000000";
		block.prevHash = "00000000000000000000000000000000000000";
		block.nonce =	 "0";

		blocks_.push_back(block);
	}
	friend class http_connection;
};
 