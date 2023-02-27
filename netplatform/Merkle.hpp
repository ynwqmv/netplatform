#include "Block.h"



class Merkle
{
public:
	Merkle(const std::vector<Transaction>& transactions)
	{
		if (transactions.empty())
		{
			throw std::runtime_error("No transactions");
		}
		
		 

	}

	

	
private:
	std::vector<Transaction> hashes_;

	/*std::string HashTransaction(const Transaction& tx) const
	{
	 	//Compute the hash of the transaction using a hash function like SHA-256
	 	//and return the hash as a string
		return "";
	}*/
};