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

        // Sending string message to the consumer
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
            std::cout << name << "-Received: " << message.value() << std::endl;
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
    std::thread producer_thread(::produce, std::move(producer));

    // Passing consumer object to the second thread
    std::thread consumer1_thread(::consume, std::move(consumer1), "Consumer1");
    std::thread consumer2_thread(::consume, std::move(consumer2), "Consumer2");

    producer_thread.join();
    consumer1_thread.join();
    consumer2_thread.join();

    return 0;
}
