#pragma once
 

#include <vector>
#include <string>


class Transaction
{
public:
	Transaction() = default;
	Transaction(std::string from, std::string to, double amount)
	{
		this->_from = from;
		this->_to = to;
		this->_amount = amount;
	}

private:
	std::string _from;
	std::string _to;
	double _amount;
};




 

