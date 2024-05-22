#include "libcpp-channel.hpp"
#include <iostream>
#include <thread>

using namespace lklibs;

void produce(Channel<std::string>::Producer producer)
{
    auto i = 0;

    while (true)
    {
        i++;

        // Sending string message to all consumers
        producer.send("Message " + std::to_string(i));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void consume(Channel<std::string>::Consumer consumer, const std::string& name)
{
    while (true)
    {
        // Receiving message from the producer
        auto message = consumer.receive();

        if (message.has_value())
        {
            std::cout << name << " Received: " << message.value() << std::endl;
        }
    }
}

int main()
{
    // Creating a string channel
    Channel<std::string> channel;

    // Getting producer and consumer objects
    auto producer = channel.getProducer();
    auto consumer1 = channel.getConsumer();
    auto consumer2 = channel.getConsumer();

    // Passing producer object to the first thread
    std::thread produce_thread(::produce, std::move(producer));

    // Passing consumer objects to other threads
    std::thread consume1_thread(::consume, std::move(consumer1), "Consumer1");
    std::thread consume2_thread(::consume, std::move(consumer2), "Consumer2");

    produce_thread.join();
    consume1_thread.join();
    consume2_thread.join();

    return 0;
}
