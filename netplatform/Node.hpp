#pragma once
#include "Network.h"

class Node
{

public:
	Node() = default;
	Node(Blockchain& chain, Block& block)
		: _chain(chain), _block(block)
	{
        
	}

	void connect(const std::string& address, UINT port)
	{
         /* ... */
	}
	
private:
	Blockchain& _chain;
	Block& _block;
	std::vector<std::shared_ptr<tcp::socket>> _sockets;
	net::io_context io_context;
	UINT _port;
};
