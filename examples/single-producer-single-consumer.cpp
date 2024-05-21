#include "libcpp-channel.hpp"
#include <iostream>
#include <thread>

using namespace lklibs;

void producer(Channel<std::string>::Producer producer)
{
    auto i = 0;

    while (true)
    {
        i++;

        // Sending string message to the consumer
        producer.send("Message " + std::to_string(i));

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void consumer(Channel<std::string>::Consumer consumer)
{
    while (true)
    {
        // Receiving message from the producer
        auto message = consumer.receive();

        if (message.has_value())
        {
            std::cout << "Received: " << message.value() << std::endl;
        }
    }
}

int main()
{
    // Creating a string channel
    Channel<std::string> channel;

    // Getting producer and consumer objects
    auto producer = channel.getProducer();
    auto consumer = channel.getConsumer();

    // Passing producer object to the first thread
    std::thread producer_thread(::producer, std::move(producer));

    // Passing consumer object to the second thread
    std::thread consumer_thread(::consumer, std::move(consumer));

    producer_thread.join();
    consumer_thread.join();

    return 0;
}
