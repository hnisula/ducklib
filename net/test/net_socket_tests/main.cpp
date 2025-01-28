#include <thread>
#include <gtest/gtest.h>
#include <YourSocketLib.h>  // replace this with actual lib
// more includes as needed

// replace these with your actual send/receive functions
void sendFunc(Socket &s) {
    // send something
}

void receiveFunc(Socket &s) {
    // receive something
}

TEST(SocketTest, SuccessfulDataTransmission) {
    Socket sender, receiver;
    // setup your sockets

    std::thread senderThread(sendFunc, std::ref(sender));
    std::thread receiverThread(receiveFunc, std::ref(receiver));

    senderThread.join();
    receiverThread.join();

    // Add assertions here to verify successful transmission
    ASSERT_EQ( /*expected value*/, /*actual value*/);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}