#include "Block.h"



class Merkle
{
public:
	Merkle(std::vector<Transaction>& transactions)
	{
		if (transactions.empty())
			spdlog::info("Merkle is empty");

		
		int n = transactions.size();
		
		
		if (n % 2 == 0)
		{
			transactions.push_back(transactions[n - 1]);
			n++;
		}

		
		std::stringstream ss;
		

		for (int i = 0; i < n; ++i)
		{
			ss << transactions[i].getAmount() << transactions[i].getFrom() << transactions[i].getRecipient();
			merkle.push_back(sha512(ss.str()));
		}

		// Build the tree from the bottom : up
		for (int i = n - 1; i > 0; i -= 2)
		{
			std::string left = merkle[i - 1];
			std::string right = merkle[i];
			merkle.push_back(sha512(left + right));
		}

		
	}
	 
	std::string getRoot()
	{
		return merkle[merkle.size() - 1];
	}

	
private:
	 
	std::vector<std::string> merkle;
	






};