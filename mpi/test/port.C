#include <TMpi.h>
using namespace ROOT::Mpi;
// NOTE: test taken from http://mpi.deino.net
void port()
{
   TEnvironment env; // environment to start communication system

   auto rank = COMM_WORLD.GetRank();
   auto size = COMM_WORLD.GetSize();
   if (size < 3) {
      std::cerr << "Three processes needed to run this test.\n";
      return 0;
   }

   TPort port;
   port.Open();
   port.Print();
   ROOT_MPI_ASSERT(port.IsOpen() == kTRUE);
   port.Close();
   port.Print();
   ROOT_MPI_ASSERT(port.IsOpen() == kFALSE);
   port.Open();
   ROOT_MPI_ASSERT(port.IsOpen() == kTRUE);
   port.Print();

   Int_t data = 0;
   if (COMM_WORLD.IsMainProcess()) {
      std::cout << "0: opening ports.\n" << std::endl;
      TPort p1, p2;
      p1.Open();
      p2.Open();
      ROOT_MPI_ASSERT(p1 != p2);
      std::cout << Form("opened port1: <%s>\n", p1.GetPortName().Data()) << std::endl;
      std::cout << Form("opened port2: <%s>\n", p2.GetPortName().Data()) << std::endl;

      ROOT_MPI_ASSERT(p1.IsOpen() == kTRUE);
      ROOT_MPI_ASSERT(p2.IsOpen() == kTRUE);

      COMM_WORLD.Send(p1, 1, 0);
      COMM_WORLD.Send(p2, 2, 0);

      // creating intercomms with the ports
      std::cout << "accepting port2.\n";
      auto intercomm2 = COMM_SELF.Accept(p2, 0);
      ROOT_MPI_CHECK_COMM(intercomm2, &COMM_SELF);
      std::cout << "accepting port1.\n";
      auto intercomm1 = COMM_SELF.Accept(p1, 0);
      ROOT_MPI_CHECK_COMM(intercomm1, &COMM_SELF);
      p1.Close();
      p2.Close();

      ROOT_MPI_ASSERT(p1.IsOpen() != kTRUE);
      ROOT_MPI_ASSERT(p2.IsOpen() != kTRUE);

      std::cout << "sending 1 to process 1.\n";
      data = 1;
      intercomm1.Send(data, 0, 0);

      std::cout << "sending 2 to process 2.\n";
      data = 2;
      intercomm2.Send(data, 0, 0);

      intercomm1.Disconnect();
      intercomm2.Disconnect();
   } else if (rank == 1) {
      TPort p1;
      COMM_WORLD.Recv(p1, 0, 0);

      auto intercomm1 = COMM_SELF.Connect(p1, 0);
      intercomm1.Recv(data, 0, 0);
      ROOT_MPI_ASSERT(data == 1, &intercomm1);
      intercomm1.Disconnect();
   } else if (rank == 2) {
      TPort p2;
      COMM_WORLD.Recv(p2, 0, 0);
      /* make sure process 1 has time to do the connect before this process attempts to connect */
      gSystem->Sleep(3000);
      auto intercomm2 = COMM_SELF.Connect(p2, 0);
      intercomm2.Recv(data, 0, 0);
      ROOT_MPI_ASSERT(data == 2, &intercomm2);
      intercomm2.Disconnect();
   }
   COMM_WORLD.Barrier();

   // TODO: test  Publish/UnPublish/LookupName
}

Int_t main()
{
   port();
   return 0;
}
