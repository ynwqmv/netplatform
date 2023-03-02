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
#define _WIN32_WINNT 0x0601


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using boost::asio::ip::tcp;

class Network
{
public:
	explicit Network(Blockchain& chain, Block& block) : _chain(chain), _block(block), _ioContext(), _acceptor(_ioContext, tcp::endpoint(tcp::v4(), 8000)) {
		listenForConnections();
	}

	void run() {
		spdlog::info("Starting network...");
		_ioContext.run();
	}


private:
	Blockchain& _chain;
	Block& _block;
	net::io_context _ioContext;
	tcp::acceptor _acceptor;
	std::vector < std::shared_ptr<tcp::socket> > _sockets;
	
std::unordered_set<std::string> _knownPeers;

	void listenForConnections() {

		spdlog::info("Listening for connections on port 8000");
		auto socket = std::make_shared<tcp::socket>(_ioContext);
		_acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& error) {
			if (!error) {
				spdlog::info("New connection from {}", socket->remote_endpoint().address().to_string());

				_sockets.push_back(socket);

				listenForMessages(socket);

				listenForConnections();
			}
			else {
				spdlog::error("Error accepting connection: {}", error.message());
			}
			});
	}
	void listenForMessages(std::shared_ptr<tcp::socket> socket) {
		auto message = std::make_shared<std::string>();
		boost::asio::async_read_until(*socket, boost::asio::dynamic_buffer(*message), '\n',
			[this, socket, message](const boost::system::error_code& error, std::size_t length) {
				if (!error) {
					handleMessage(*message);

					listenForMessages(socket);
				}
				else if (error == boost::asio::error::eof) {
					spdlog::warn("Client at {} disconnected", socket->remote_endpoint().address().to_string());

					_sockets.erase(std::remove(_sockets.begin(), _sockets.end(), socket), _sockets.end());
				}
				else {
					spdlog::warn("Error reading message from {}: {}", socket->remote_endpoint().address().to_string(), error.message());

					_sockets.erase(std::remove(_sockets.begin(), _sockets.end(), socket), _sockets.end());
				}
			});
	}

	void handleMessage(const std::string& message) {
		if (message.substr(0, 6) == "/hello") {
			handleHelloMessage(message);
		}
		else if (message.substr(0, 5) == "/sync") {
			handleSyncMessage(message);
		}
		else if (message.substr(0, 9) == "/gen_addr")
		{
			handleAddrGenMessage(message);
		}
		else if (message.substr(0, 3) == "/tx")
		{

			handleTransactions();

		}
		else if (message.substr(0, 10) == "/pendingtx")
		{
			handlePendingTx();
		}
		else if (message.substr(0, 7) == "/tojson")
		{
			handleToJson();
		}
		else if (message.substr(0, 6) == "/peers")
		{
			for (const auto& peers : _knownPeers)
			{
				spdlog::info("Peers: {}", peers);
			}
		}
		else {
			spdlog::warn("Unknown message type received: {}", message);
		}
	}
	void handleHelloMessage(const std::string& message) {
		std::string peerAddress = message.substr(6, message.length() - 7);

		if (_knownPeers.count(peerAddress) == 0) {
			spdlog::info("New peer connected: {}", peerAddress);

			_knownPeers.insert(peerAddress);
		}
	}



	void handleSyncMessage(const std::string& message) {
		 
		spdlog::info("Command: {}", message);
		_chain.Mine(_block);
		_chain.addBlock(_block);
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
	void handleToJson()
	{
		spdlog::info("JSON: {}", _chain.toJSON(_chain).dump(2));
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

 
#endif 