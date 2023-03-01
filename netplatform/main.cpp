#include "Network.h"
#include "Blockchain.h"

 


int main(int argc, const char* argv[])
{
	Blockchain b;
	Block block;
	Network w(b,block);
	w.run();

}




 
 

