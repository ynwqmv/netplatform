#pragma once


#include "Block.h"
#include "Merkle.hpp"

 

class Blockchain  
{

public:
	Blockchain()      /*   Genesis Block  */
		: blocks_{} {
		create_genesis_block();
		spdlog::info("> Genesis block forged!");
	}
	 
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
		 
		std::string target(difficulty, '0');  // create target value
		std::string hash;
		std::string nonceStr;

		int nonce = 0;
		bool found = false;
		while (!found)
		{ 
			nonceStr = std::to_string(nonce);          // convert nonce to string
			block.nonce = nonceStr;					   // set nonce of the block
			hash = block.calculateHash();	           // calculate the hash of the block
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
	 
	
	void addBlock(Block& block)
	{

		
		if (blocks_.empty())
		{
			block.setPrevHash("");
		}
		else
		{
			block.index++;
			block.timestamp = std::time(nullptr);
			block.setPrevHash(blocks_.back().getHash());
		}

		if (!pendingTransaction.empty())
		{
			std::vector<Transaction> transactions;
			
			
			if (pendingTransaction.size() <= MAX_BLOCK_SIZE)
			{
				transactions = pendingTransaction;
				pendingTransaction.clear();
				
			}
			else
			{
				transactions = std::vector<Transaction>(pendingTransaction.begin(), pendingTransaction.begin() + MAX_BLOCK_SIZE);
				pendingTransaction = std::vector<Transaction>(pendingTransaction.begin() + MAX_BLOCK_SIZE, pendingTransaction.end());
				
			}

			
			block.transaction.insert(block.transaction.end(), transactions.begin(), transactions.end());
			
		}

		blocks_.push_back(block);
		block.block_count++;
		 
		
		
		spdlog::info("Block was added successfully!");
		print_chain();
		std::cout << std::endl;
		
		if (block.block_count % EBLOCKS == 0)
		{
			/*                * HALVING *
				* Each `50000` blocks difficulty(+2) | reward(/2)
			*/	
			spdlog::info("Halving at block # {}", block.block_count);
			difficulty += 2;
			reward /= 2;
		}
	}

	const void pending_transactions() const noexcept 
	{
		spdlog::set_level(spdlog::level::debug);
		if (!pendingTransaction.empty())
		{
			 
			
			for (const auto& i : pendingTransaction)
			{
				spdlog::info("*+=+=+=+=+=+=+=+=+=+=+");
				spdlog::info("Amount: {}", i.getAmount());
				spdlog::info("inTx: {}", i.getFrom());
				spdlog::info("outTx: {}", i.getRecipient());
				 
			}
			std::cout << std::endl;
		}
		else
		{
			spdlog::debug("No transactions yet");
		}
		
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
			spdlog::info("Nonce: {}", block.nonce);
			spdlog::info("Transactions:");
			for (auto transaction : block.transaction) {
				
				spdlog::info("\tSender: {}", transaction.getFrom());
				spdlog::info("\tRecipient: {}", transaction.getRecipient());
				spdlog::info("\tAmount: {}", transaction.getAmount());
				spdlog::info("TX Hash: {}", transaction.getTransactionHash());
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
	const Blockchain getChain() const
	{
		return *this;
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
		block.index =     NULL;
		block.timestamp = std::time(nullptr);
		block.hash =	 "NULL";
		block.prevHash = "NULL";
		block.nonce =	 "NULL";

		blocks_.push_back(block);
	}

	
	 
};
 