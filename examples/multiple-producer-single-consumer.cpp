#include "libcpp-channel.hpp"
#include <iostream>
#include <thread>

using namespace lklibs;

void produce(Channel<std::string>::Producer producer, const std::string& name)
{
    auto i = 0;

    while (true)
    {
        i++;

        // Sending string message to the consumer with producer name
        producer.send(name + " Message " + std::to_string(i));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void consume(Channel<std::string>::Consumer consumer)
{
    while (true)
    {
        // Receiving message from the producers
        auto message = consumer.receive();

        if (message.has_value())
        {
            std::cout << "Consumer Received: " << message.value() << std::endl;
        }
    }
}

int main()
{
    // Creating a string channel
    Channel<std::string> channel;

    // Getting producer and consumer objects
    auto producer1 = channel.getProducer();
    auto producer2 = channel.getProducer();
    auto consumer = channel.getConsumer();

    // Passing producer objects to producer threads
    std::thread producer1_thread(::produce, std::move(producer1), "Producer1");
    std::thread producer2_thread(::produce, std::move(producer2), "Producer2");

    // Passing consumer object to the second thread
    std::thread consumer_thread(::consume, std::move(consumer));

    producer1_thread.join();
    producer2_thread.join();
    consumer_thread.join();

    return 0;
}
