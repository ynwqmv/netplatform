#pragma once
 
#include "sha512.h"
#include <vector>
#include <string>

/* Transactions for Blockchain : from->to:amount 
|  implemented: pending transactions   |*  
|  future: transaction fee  *  @lexndr |* 
|  ^^^^^^^ approximate ~ 0.000000140   |*/
class Transaction
{
public:
	Transaction() = default;
	Transaction(std::string& from, std::string& to, double& amount)
	{
		this->_from = from;
		this->_to = to;
		this->_amount = amount;
	}
	
	/// getters
	inline const std::string getFrom() const noexcept { return _from; }
	inline const std::string getRecipient()   const noexcept { return _to; }
	inline const double getAmount()    const noexcept { return _amount; }

	/* Hash of each transaction in blocks
       ^^ including : _from, _to, _amount
	   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	*/
	const std::string getTransactionHash() noexcept
	{
		std::string transaction_hash = getFrom() + getRecipient() + std::to_string(getAmount());
		std::string _hash = sha512(transaction_hash);
		return _hash;
	}
private:
	 
	std::string _from;
	std::string _to;
	double _amount;
};




 

