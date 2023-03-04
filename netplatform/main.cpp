/*                                           ABOUT P2P NODES

    Choose a network protocol: You will need to decide which network protocol you want to use for your P2P network.
                            Common options include TCP/IP, UDP, or WebSockets.

    
    Implement network communication: Once you have chosen a network protocol, you will need to implement network
    communication between nodes. You can use a library like Boost.Asio to handle low-level network communication.

    Implement message exchange: Once nodes can communicate with each other, you will need to implement a protocol for
    exchanging messages between nodes. This protocol should include message types for requesting and sending blocks,
                        transactions, and other data related to the blockchain.

    Implement synchronization: With the message exchange protocol in place, you can implement synchronization logic.
    When a new node joins the network, it should request the entire blockchain from other nodes on the network.
    When a node receives a request for a block, it should respond with the block and all blocks that follow it, up
                                        to the most recent block.

    Handle conflicts: In a decentralized network, it is possible for different nodes to create conflicting blocks at
    the same height. To handle this, you will need to implement a consensus algorithm that determines which block should
        be added to the blockchain. Common consensus algorithms include Proof of Work (PoW) and Proof of Stake (PoS).

    Monitor network health: Finally, you will need to implement a monitoring system to ensure that the network remains
    healthy. This includes monitoring node uptime, block propagation times, and other metrics that can impact network
                                            performance.

*/


 


#include "Network.h"
#include "Blockchain.h"
 
 


int main(int argc, const char* argv[])
{
    try {
        Blockchain b;
        Block block;
        boost::asio::io_context io_context;
        
        
        Network net(b,block, 8001);
        
        net.run();
         
         
        
    }
    catch (std::exception& err)
    {
        spdlog::info("{}", err.what());
    }
    catch (boost::system::system_error& err)
    {
        spdlog::info("{}", err.what());
    }

} 