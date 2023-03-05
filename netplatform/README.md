# Netplatform 0.8.40 (upds)
# network (upd. ![#7:55](https://github.com/ynwqmv/netplatform/master/netplatform/README.md)) 
* Contains:    
Network implementation | ![_Transmission Control Protocol (TCP)_](https://www.techtarget.com/searchnetworking/definition/TCP) 
* Used:  
![_libboost_](https://github.com/boostorg/boost)
### Review
```cpp
class Network
{
public:
	explicit Network(Blockchain& chain, Block& block)
      : _chain(chain),
        _block(block),
        _ioContext(), 
        _acceptor(_ioContext, tcp::endpoint(tcp::v4(), 8000)) {
		     // listening for new connections until connection != error
         listenForConnections();
	}

	void run() {
		spdlog::info("Starting network...");
		_ioContext.run();
	}
```

```cpp
/* Private Members */
private:
	Blockchain& _chain;
	Block& _block;
	net::io_context _ioContext;
	tcp::acceptor _acceptor;
	std::vector < std::shared_ptr<tcp::socket> > _sockets;
	
std::unordered_set<std::string> _knownPeers;

```
### Listening for connections
```cpp 
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
```
____
# (upd #11:1)
![image](https://user-images.githubusercontent.com/126266383/222915294-41e8c938-7736-4e29-bf2f-373acaa8eb56.png)
![image](https://user-images.githubusercontent.com/126266383/222915379-69f1dda5-5c81-47bb-b9ae-11db1e4620cb.png)
 

![image](https://user-images.githubusercontent.com/126266383/222915446-bcb15db7-d36d-4209-b6fa-210f0e353a30.png)
![image](https://user-images.githubusercontent.com/126266383/222915493-65a3a827-be7b-4d62-b25a-7a8718c8bbd4.png)
___

