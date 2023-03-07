/*
	MIT License

	Copyright (c) lexndrr *


	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/


#ifndef SERVER_H
#define SERVER_H


#include "Blockchain.h"
#include <windows.h>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp> 
#define _WIN32_WINNT 0x0601
#define __NETV 0x0110409
#define NO_NODES_FOUND spdlog::critical("No nodes are connected");

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using boost::asio::ip::tcp;

using USetStr		    = std::unordered_set<std::string>;
using UMapStrLongDouble = std::unordered_map<std::string, long double>;
using VecSPtrTcpSocket  = std::vector < std::shared_ptr<tcp::socket> >;

struct SocketData {
	std::shared_ptr<tcp::socket> socket;
	std::string key;
};



class Network
{
public:
	
	
	explicit Network(Blockchain& chain, Block& block, UINT port) : 
		_chain(chain), 
		_block(block),
		_ioContext(),
		_port(port),
		_acceptor(_ioContext, tcp::endpoint(tcp::v4(), port)) { 
		listenForConnections(); update();
	}
	
	void run() {
		spdlog::info("Starting network...");
		menu();
		_ioContext.run();
	}

	const bool isValid() const
 	{
		if (_block.getHash() != _block.calculateHash())
		{
			
			return false;
		}
		
		if (_block.transaction.size() > MAX_BLOCK_SIZE)
		{
			return false;
		}
		for (const auto& tx : _block.transaction)
		{
			if (tx.getAmount() < 0.000001 || tx.getFrom() == "" || tx.getRecipient() == "")
			{
				return false;
			}
		}

		 

		return true;
	}

 
	 
private:
	Blockchain& _chain;         // For manipulating with our chain
	Block& _block;              // For manipulating with our chain's blocks
	net::io_context _ioContext; // For I/O Networking
	tcp::acceptor _acceptor;    // For Manipulating with our TCP 
	UINT _port;
	VecSPtrTcpSocket _sockets;
	
	std::unordered_map<std::string, SocketData> keys;
	USetStr _knownPeers;
	UMapStrLongDouble _balance;
	static size_t sock_count;

	void listenForConnections() {

		auto socket = std::make_shared<tcp::socket>(_ioContext);
		_acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& error) {
			if (!error) {
				++sock_count; // connection count +1
				spdlog::info("New connection from {}:{}", socket->remote_endpoint().address().to_string(), socket->remote_endpoint().port());
				boost::uuids::random_generator uuid_generator;
				boost::uuids::uuid node_id = uuid_generator();
				std::string node_id_str = boost::uuids::to_string(node_id);
				spdlog::info("Key: {}", node_id_str);
				for (const auto& sock : _sockets)
				{
					
					boost::asio::write(*sock, boost::asio::buffer(node_id_str + " : new node connected!\n"));
				}
				 

				std::string key = socket->remote_endpoint().address().to_string() + ":" + std::to_string(socket->remote_endpoint().port());
				keys[node_id_str] = { socket, key };

				_sockets.push_back(socket);

				listenForMessages(socket);

				listenForConnections();
			}
			else {
				spdlog::error("Error accepting connection: {}", error.message());
			}
			});
	}
	
	void handleDisconnect(std::shared_ptr<tcp::socket> socket)
	{
		for (auto it = keys.begin(); it != keys.end(); ++it)
		{
			if (it->second.socket == socket)
			{
				std::string key = it->second.key;
				keys.erase(it);
				spdlog::info("Node disconnected: {}", key);
				break;
			}
		}
	}

	 


	void listenForMessages(std::shared_ptr<tcp::socket> socket) {
		
		auto message = std::make_shared<std::string>();
		boost::asio::async_read_until(*socket, boost::asio::dynamic_buffer(*message), '\n',
			[this, socket, message](const boost::system::error_code& error, std::size_t length) {
				if (!error) {
					handleMessage(*message, socket);

					listenForMessages(socket);
				}
				else if (error == boost::asio::error::eof) {
					--sock_count;
					spdlog::critical("Client at {}:{} disconnected", socket->remote_endpoint().address().to_string(), socket->remote_endpoint().port());
					handleDisconnect(socket);
					_sockets.erase(std::remove(_sockets.begin(), _sockets.end(), socket), _sockets.end());
				}
				else {
					spdlog::warn("Error reading message from {}:{}", socket->remote_endpoint().address().to_string(), error.message());

					_sockets.erase(std::remove(_sockets.begin(), _sockets.end(), socket), _sockets.end());
				}
			});
	}

	void getNodes() 
	{
		for (auto& k : keys)
		{
			spdlog::info("{}", k.first);
		}
	}
	 
	
	void Sync(std::shared_ptr<tcp::socket> socket)
	{
		// Get the current blockchain and serialize it to JSON
		auto chain_json = _chain.toJSON(_chain);
		std::string chain_str = chain_json.dump() + "\n\n";

		// Send the blockchain to the connected node
		boost::asio::write(*socket, boost::asio::buffer(chain_str));
	}

	void handleMessage(const std::string& message, std::shared_ptr<tcp::socket> socket) {
	

		if (message.substr(0, 5) == "/mine")
			handleSyncMessage(message);

		else if (message.substr(0, 9) == "/gen_addr")
			handleAddrGenMessage(message);
		else if (message.substr(0, 3) == "/tx")
			handleTransactions();
		else if (message.substr(0, 10) == "/pendingtx")
			handlePendingTx();

		else if (message.substr(0, 6) == "/print")
			toJSONClient(socket);
		else if (message.substr(0, 8) == "/isvalid")
		{
			 
			if (isValid()) { spdlog::info("Blockchain is valid"); }
			else { spdlog::info("Blockchain is invalid"); }
		}
		else if (message.substr(0, 9) == "/getnodes")
		{
			getNodes();
		}
		else if (message.substr(0, 5) == "/sync")
		{
			// Sync(socket) here
			for (const auto& sock : _sockets)
			{
				
				Sync(sock); // sync to all nodes connecting to blockchain network 
			}
		}
		else if (message.substr(0, 11) == "/tojsonlast")
		{
			spdlog::info("{}", _chain.toJSON_last_block(_block).dump());
		}
		else if (message.substr(0, 5) == "/info")
		{
			handleInfo();
		}
		 
		else spdlog::error("Unknown message type received: {}", message);
	}


	
	void toJSONClient(std::shared_ptr<tcp::socket> sock) const noexcept
	{
		auto jsonf = _chain.toJSON(_chain).dump();
		
		if (!_sockets.empty())
		{
			for (const auto& _sock : _sockets)
			{
				if (_sock == sock)
				{
					boost::asio::write(*sock, boost::asio::buffer(jsonf));
				}
			}
		}
		else
		{
			NO_NODES_FOUND;
		}
		 
	}

	void handleHelloMessage(const std::string& message) {
		std::string peerAddress = message.substr(6, message.length() - 7);
		std::vector<std::string> parts;
		boost::split(parts, peerAddress, boost::is_any_of(":"));

		if (parts.size() == 2) {
			std::string ip = parts[0];
			std::string port = parts[1];

			if (_knownPeers.count(peerAddress) == 0) {
				spdlog::info("New peer connected: {}:{}",ip,port);

				_knownPeers.insert(peerAddress);

				try { 
					auto socket = std::make_shared<tcp::socket>(_ioContext);
					boost::asio::io_service io_service;
					tcp::resolver resolver(io_service);
					auto endpoints = resolver.resolve(ip, port);

					boost::asio::async_connect(*socket, endpoints,
						[this, socket](const boost::system::error_code& error, tcp::endpoint endpoint) {
							if (!error) {
								spdlog::info("Connected to peer at {}:{}",endpoint.address().to_string(), endpoint.port());

								_sockets.push_back(socket);

								listenForMessages(socket);
							}
							else {
								spdlog::info("Error connecting to peer at {} : {}",endpoint.address().to_string(), endpoint.port());
							}
						});
				}
				catch (const std::exception& e) {
					spdlog::info("Connection Status: {}",e.what());
				}
			}
		}
	}


	const void menu() const noexcept
	{
		spdlog::info("");
		spdlog::info("+-+-+-+-+-+-+-+-+-+COMMANDS+-+-+-+-+-+-+-+-+-+");
		spdlog::info("/isvalid		 | Usage :: /isvalid	  | (VALIDATION OF CHAIN)");
		spdlog::info("/mine		 | Usage :: /mine		  | (START MINE)");
		spdlog::info("/tx		 | Usage :: /tx `from_address` `recipient_address` `amount` | (MAKE TX)");
		spdlog::info("/pendingtx      | Usage :: /pendingtx		  | (SHOW PENDING TX-S)");
		spdlog::info("/print	         | Usage :: /print                | (PRINT BLOCKCHAIN IN JSON FORMAT)");
		spdlog::info("/getnodes       | Usage :: /getnodes             | (GET ALL CONNECTED NODE INFO.)");
		spdlog::info("/sync	         | Usage :: /sync		   | (SYNCHRONIZE BLOCKCAIN)");
		spdlog::info("/gen_addr	         | Usage :: /gen_addr		   | (GEN. UNIQUE WALLET ADDRESS)");
		spdlog::info("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+"); 
	}

	void handleSyncMessage(const std::string& message) {
		 
		spdlog::info("Miner address: ");
		std::string mineraddr;
		std::cin >> mineraddr;
		_chain.Mine(_block);
		for (const auto& sock : _sockets)
		{   
			const std::string stat = "|> NEW BLOCK MINED <|\t";
			boost::asio::write(*sock, boost::asio::buffer(stat));
			std::cout << std::endl;
			Sync(sock);
		}
		_chain.addBlock(_block);

		/* if new block is mined, server sends to all nodes message, that new block is mined
		   and then synchronizing the new chain including the new mined block with txs'   */
	}
	void handleAddrGenMessage(const std::string& message) {
		spdlog::info("Command: {}", message);
		spdlog::info("Generated Address: {}",_chain.generateAddress()); 
	}
	void handlePendingTx()
	{
		_chain.pending_transactions();
	}
	void handleMine()
	{
		spdlog::info("");
	}
	void handleTransactions()
	{
		Transaction transaction = generateNewTransaction();
		 
		_chain.addTransaction(transaction);
		 
	}

	void handleInfo() const noexcept
	{
		
		time_t now = std::time(nullptr);
		auto bc = _block.block_count;
		 
		spdlog::info("Mining Difficulty: {}",difficulty);
		spdlog::info("Max Tx per block: {}", MAX_BLOCK_SIZE);
		spdlog::info("Block reward: {}", reward);
		spdlog::info("Last block #{} | {}s ago", bc, now - _block.timestamp);
		spdlog::info("Encryption: SHA512");
		spdlog::info("Nodes Connected: {}", sock_count);
	}

	void handleToJson()
	{
		spdlog::info("JSON: {}", _chain.toJSON(_chain).dump(2));
	}

	void update() {
		auto timer = std::make_shared<boost::asio::steady_timer>(_ioContext);
		timer->expires_after(std::chrono::seconds(60));
		timer->async_wait([this, timer](const boost::system::error_code& error) {
			if (!error) {
				// Reset the timer to expire after another 10 seconds
				spdlog::info("Online: {}", sock_count); // sending to server message about Online Nodes
				if (!_sockets.empty())
				{
					for (const auto& sock : _sockets) // sending each 30 seconds message about Online Nodes
					{
						boost::asio::write(*sock,
							boost::asio::buffer("Online: " + std::to_string(sock_count)));
					}
				}
				else
				{
					NO_NODES_FOUND;
				}
				update();
			}
			else {
				spdlog::error("Error in timer callback: {}", error.message());
			}
			});
	}

	Transaction generateNewTransaction()
	{
		std::string fromAddress, toAddress;
		double amount;
		std::cout << "Enter sender address: ";
		std::cin >> fromAddress;
		std::cout << "Enter recipient address: ";
		std::cin >> toAddress;
		std::cout << "Enter amount: ";
		std::cin >> amount;

		Transaction transaction(fromAddress, toAddress, amount);
		

		return transaction;
	}
 
};

size_t Network::sock_count = 0;
#endif 