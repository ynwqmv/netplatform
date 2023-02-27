//#include "Network.h"
//#include "Blockchain.h"
//
//#include <server_http.hpp>
//int main(int argc, const char* argv[]) {
//
//     
//
//    spdlog::set_level(spdlog::level::info);
//    Blockchain chain;
//     
//    json jsonTmp;
//    
//    json jsonLastBlockTmp;
//    constexpr unsigned int port = 9999;
//  
//    boost::asio::io_context io_context;
//    Network net(io_context, port);
//    io_context.run();
//
//     
//       while (true)
//       {
//           spdlog::info("Server started on port `{}`", port);
//             
//            std::cout << "Commands: \n" << "-c, -/createAddress\t |-a, -/addTransaction\t |\n-m, -/mine\t\t | -p, -/print\t\t |\n\t\t   -e, -/exit\n/info\t\t/last_block_to_json\n";
//            std::string command;
//
//            std::getline(cin, command);
//            if (command == "-p" || command == "/print")
//            {
//                 spdlog::info("Command: {}", command);
//                 chain.print_chain();
//            }
//            else if (command == "-e" || command == "/exit")
//            {
//            spdlog::info("Closing...");
//
//            break;
//
//            }
//            else if (command == "-m" || command == "/mine") {
//                 Block block(chain.getLastBlock().getIndex() + 1);
//                 chain.Mine(block);
//                 chain.addBlock(block);
//                 spdlog::info("Block successfully mined and added to the chain.");
//                 jsonTmp = chain.toJSON(chain);
//                 jsonLastBlockTmp = chain.toJSON_last_block(block);
//
//             }
//             else if (command == "-c" || command == "/createAddress")
//             {
//                std::string addr = chain.generateAddress();
//                spdlog::info("Generated Address: {}", addr);
//             }
//                else if (command == "-a" || command == "/addTransaction")
//                {
//                    std::string sender, recipient;
//                    double amount;
//                    std::string input;
//                    std::cout << "Enter sender address: ";
//                    std::getline(std::cin, sender);
//                    std::cout << "Enter recipient address: ";
//                    std::getline(std::cin, recipient);
//                    std::cout << "Enter amount: ";
//                    std::cin >> input;
//                    std::cin.ignore(); // ignore the newline character left by std::cin
//                    try
//                    {
//                        amount = std::stod(input);
//                        Transaction transaction(sender, recipient, amount);
//                        chain.addTransaction(transaction);
//                        spdlog::info("Transaction successfully added to the mempool.");
//                    }
//                    catch (const std::invalid_argument& ia)
//                    {
//                        spdlog::info("Invalid input for amount");
//                    }
//                }
//
//                else if (command == "-j" || command == "/to_json")
//                {
//                    spdlog::info("JSON: {}", jsonTmp.dump(2));
//                }
//                else if (command == "-i" || command == "/info")
//                {
//                    chain.get_chain_info();
//                }
//                else if (command == "/last_block_to_json")
//                {
//                    spdlog::info("Last Block: {}", jsonLastBlockTmp.dump(2));
//                }
//                else if (command == "/clear")
//                {
//                    system("cls");
//                }
//                else
//                {
//                    spdlog::error("Unknown command `{}", command + '`');
//                }
//       }
//         
//         
//    return 0;
//}

#include "Network.h"
int main(int argc, char* argv[])
{
    try
    {
        // Check command line arguments.
        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << " <address> <port>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    receiver 0.0.0.0 80\n";
            std::cerr << "  For IPv6, try:\n";
            std::cerr << "    receiver 0::0 80\n";
            return EXIT_FAILURE;
        }

        auto const address = net::ip::make_address(argv[1]);
        unsigned short port = static_cast<unsigned short>(std::atoi(argv[2]));

        net::io_context ioc{ 1 };

        tcp::acceptor acceptor{ ioc, {address, port} };
        tcp::socket socket{ ioc };
        http_server(acceptor, socket);

        ioc.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}